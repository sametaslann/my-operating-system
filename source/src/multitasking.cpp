
#include <multitasking.h>
#include <mystdlib.h>



using namespace myos;
using namespace myos::common;
using namespace myos::mystd ;




Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
{
    ppid = -1;
    pid = 0;
    waitingPid = -1;
    status = Status::READY;
    priority = Priority::MEDIUM;


    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    
    cpustate -> eax = 0;
    cpustate -> ebx = 0;
    cpustate -> ecx = 0;
    cpustate -> edx = 0;
    cpustate -> esi = 0;
    cpustate -> edi = 0;
    cpustate -> ebp = 0;
    cpustate -> eip = (common::uint32_t)entrypoint;
    cpustate -> cs = gdt->CodeSegmentSelector();
    cpustate -> eflags = 0x202;
    

}
Task::Task()
{
    status = Status::READY;
    waitingPid = -1;
}

Task::~Task()
{
}

        
TaskManager::TaskManager(GlobalDescriptorTable *gdt)
{
    gdt = gdt;
    numTasks = 0;
    currentTask = -1;
    // isScheduleActive = true;
    interrupt_count = 0;
}

TaskManager::~TaskManager()
{
}


bool TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    task->pid = next_pid++;   
    tasks[numTasks++] = task;
    return true;
}


common::uint32_t TaskManager::sys_fork(CPUState* cpustate)
{
    if(numTasks >= 256)
        return 0;

    Task* parent = tasks[currentTask];
    Task* childTask = new Task(gdt, (void(*)()) cpustate->eip);
    childTask->ppid = parent->pid;
    AddTask(childTask);


    for (int i = 0; i < 4096 ; i++)
    {
        childTask->stack[i] = parent->stack[i];
    }


    common::uint32_t currentTaskOffset = ((common::uint32_t)cpustate) - (common::uint32_t)(parent->stack) ;
    childTask->cpustate = (CPUState*)(childTask->stack +  currentTaskOffset);


    childTask->cpustate->ebx = 0;
    return childTask->pid;
}



bool TaskManager::sys_exit()
{
    if(numTasks <= 0)
        return false;
    
    tasks[currentTask]->status = Status::TERMINATED;
}

bool TaskManager::sys_waitpid(common::uint32_t childPid)
{
    if (getTaskByPid(childPid)->status == Status::TERMINATED)
        return false;

    if (childPid == tasks[currentTask]->pid)
        return false;
    
    tasks[currentTask]->status = Status::WAITING;
    tasks[currentTask]->waitingPid = childPid;
    return true;
}

uint32_t TaskManager::sys_execve(void entry_point())
{
    if(numTasks >= 256)
        return 0;

    Task* task = new Task(gdt, entry_point);

    task->cpustate->ebx = 0;
    task->status = Status::READY;
    task->ppid = tasks[currentTask]->pid;
    AddTask(task);
    return task->pid;
}

void TaskManager::sys_nice(Priority priority)
{
    printf("Priority changed to: ");
    tasks[currentTask]->priority = priority;
    printfHex((int)priority);
}


Task* TaskManager::getTaskByPid(uint32_t pid)
{
    for (int i = 0; i < numTasks; i++)
    {
        if(tasks[i]->pid == pid )
            return tasks[i];
    }
    return nullptr;
}

void TaskManager::PrintProcessTable(){

    printf("\n");
    for (int i = 0; i < numTasks; i++)
    {
        printf("| PID: ");
        printfDigit(tasks[i]->pid);
        printf("| PPID: ");
        printfDigit(tasks[i]->ppid);
        printf("| PRIORITY: ");
        printfDigit((int)tasks[i]->priority);
        printf("| EX TIME: ");
        printfDigit(tasks[i]->executionTime);
        switch (tasks[i]->status)
        {
            case RUNNING:
                printf(" RUNNING ");
                break;
            case WAITING:
                printf(" WAITING ");
                break;
            case TERMINATED:
                printf(" TERMINATED ");
                break;
            case READY:
                printf(" READY ");
                break;
            default:
                break;
        }
        // printf("| ESP: ");
        // printfHex32((uint32_t)tasks[i]->cpustate);
        // printf("| EIP: ");
        // printfHex32(tasks[i]->cpustate->eip);
        // printf("| Ebx: ");
        // printfHex32(tasks[i]->cpustate->ebx);
        printf("\n");
    }

    #ifdef SLOW
    for (int i = 0; i < 10000000; i++)
    {
        printf("");
    }
    #endif
    
    
}


bool TaskManager::isWaitingForChild(int pid){

    for (int i = 0; i < numTasks; i++)
    {
        if(tasks[i]->ppid == pid && tasks[i]->status != Status::TERMINATED)
            return true;
    }
    return false;
}

int TaskManager::findNextTaskByPriority()
{

    if (currentTask < 0)
    {
        currentTask = 0;
        tasks[currentTask]->status = Status::RUNNING;
        return currentTask;
    }
    int next_task = (currentTask + 1) % numTasks;
    int i = next_task;

    int highest_priority_task = -1;  
    int highest_priority = 1000; 


    while(next_task != currentTask)
    {
        if (tasks[next_task]->status == Status::TERMINATED)
        {
            next_task = (next_task + 1) % numTasks;
            continue;
        }
        
        if(tasks[next_task]->status == Status::WAITING) 
        {
            if(!isWaitingForChild(tasks[next_task]->pid))
            {
                tasks[next_task]->waitingPid = -1;
                tasks[next_task]->status = Status::READY;
            }
        }

        if( tasks[next_task]->status == Status::READY && ( (int) tasks[next_task]->priority < highest_priority))
        {
            highest_priority = tasks[next_task]->priority;
            highest_priority_task = next_task;
        }
       
        next_task = (next_task + 1) % numTasks;

    }

    if (tasks[currentTask]->status == Status::RUNNING)
        if (tasks[highest_priority_task]->priority > tasks[currentTask]->priority)
            highest_priority_task = currentTask;
            
    if (highest_priority_task == -1)
        highest_priority_task = currentTask;
    

    // printf("Highest_priority_task: ");
    // printfHex(highest_priority_task);
    // printf("\n");

    if (tasks[currentTask]->status == Status::RUNNING)
        tasks[currentTask]->status = Status::READY;

    tasks[highest_priority_task]->status = Status::RUNNING;    
    return highest_priority_task;
}

int TaskManager::findNextTask()
{
    int next_task = (currentTask + 1) % numTasks;

    while (tasks[next_task]->status != Status::READY && next_task != currentTask)
    {
        if(tasks[next_task]->status == Status::WAITING) //Check if the task is waiting for a child
        {
            if(!isWaitingForChild(tasks[next_task]->pid))
            {
                tasks[next_task]->waitingPid = -1;
                tasks[next_task]->status = Status::READY;
                continue;
            }
        }
        next_task = (next_task + 1) % numTasks;
    }
    
    if (tasks[currentTask]->status == Status::RUNNING)
        tasks[currentTask]->status = Status::READY;

    tasks[next_task]->status = Status::RUNNING;    
    
    return next_task;
}



CPUState* TaskManager::Schedule(CPUState* cpustate)
{    
    if(numTasks <= 0)
        return cpustate;

    if(currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;

    if (currentTask == 0 && tasks[currentTask]->status == Status::TERMINATED) // if the init task is terminated
    {
        PrintProcessTable();    
        return cpustate;
    }

    int oldTask = currentTask;



    #ifdef PARTA_1
        currentTask = findNextTask();
    #endif

    #ifdef SYSCALL_TEST
        currentTask = findNextTask();
    #endif




    #ifdef PARTB_1
        currentTask = findNextTask();
    #endif

    #ifdef PARTB_2
        currentTask = findNextTask();
    #endif

    #ifdef PARTB_3    
        if (interrupt_count == 5)
        {
            for (int i = 0; i < numTasks; i++)
                tasks[i]->priority = Priority::MEDIUM;
        }
        else if(interrupt_count < 5)
            interrupt_count++;

        currentTask = findNextTaskByPriority();
    #endif


    #ifdef PARTB_4
        if (interrupt_count == 5 )
        {
            // if it has been working more than 2 times, increase its priority
            interrupt_count = 0;
            if (tasks[currentTask]->executionTime > 2)
                tasks[currentTask]->priority = (Priority)((int)tasks[currentTask]->priority + 1); 
            
        }
        else if(interrupt_count < 5)
            interrupt_count++;

        currentTask = findNextTaskByPriority();
        
        if (oldTask == currentTask)
            tasks[currentTask]->executionTime++;
        else
            tasks[currentTask]->executionTime = 0;
    #endif


    #ifdef PARTC_1
        currentTask = findNextTask();
    #endif

    #ifdef PARTC_2
        // if the task has been working for 3 times, change the task
        if (oldTask == currentTask)
            tasks[currentTask]->executionTime++;
            
        if (tasks[currentTask]->executionTime == 3)
        {
            tasks[currentTask]->executionTime = 0;
            currentTask = findNextTask();
        }
        else
        {
            currentTask = findNextTaskByPriority();
        }
        
    #endif

    #ifdef SHOW
        PrintProcessTable();    
    #endif


    return tasks[currentTask]->cpustate;
}
    
    