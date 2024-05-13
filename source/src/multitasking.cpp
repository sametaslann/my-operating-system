
#include <multitasking.h>

using namespace myos;
using namespace myos::common;

void printf(char*);
void printfHex(common::uint8_t key);
void printfHex32(common::uint32_t key);


Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
{
    ppid = 0;
    pid = 0;
    waitingPid = -1;
    status = Status::READY;

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
    

}

TaskManager::~TaskManager()
{
}

void sleep(int seconds)
{
    for(int i = 0; i < 100000000 * seconds; i++)
    {
        printf("");
    }
}

bool TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    task->pid=next_pid++;   
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


Task* TaskManager::getTaskByPid(uint32_t pid)
{
    for (int i = 0; i < numTasks; i++)
    {
        if(tasks[i]->pid == pid )
            return tasks[i];
    }
}

void TaskManager::PrintProcessTable(){

    printf("\n");

    for (int i = 0; i < numTasks; i++)
    {
        printf("| PID: ");
        printfHex(tasks[i]->pid);

        // printf("| PPID: ");
        // printfHex(tasks[i]->ppid);
        // printf("| Status: ");

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
        printf("| ESP: ");
        printfHex32((uint32_t)tasks[i]->cpustate);
        printf("| EIP: ");
        printfHex32(tasks[i]->cpustate->eip);
        printf("| Ebx: ");
        printfHex32(tasks[i]->cpustate->ebx);
        printf("\n");

        // for(int j = 0; j < 10000000; j++)
        //     printf("");
    }
    
}




CPUState* TaskManager::Schedule(CPUState* cpustate)
{
    if(numTasks <= 0)
        return cpustate;


    if(currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;
    

    
    int next_task = (currentTask + 1) % numTasks;
    while (tasks[next_task]->status != Status::READY)
    {
        if(tasks[next_task]->status == Status::WAITING)
        {
            if(getTaskByPid(tasks[next_task]->waitingPid)->status == Status::TERMINATED)
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
        
    currentTask = next_task;    
    tasks[currentTask]->status = Status::RUNNING;
    
    PrintProcessTable();    
    return tasks[currentTask]->cpustate;
}

    