 
#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace myos
{

    enum Status{READY, RUNNING, WAITING, TERMINATED};
    enum Priority{VERY_HIGH, HIGH, MEDIUM, LOW, VERY_LOW};

    static common::uint32_t next_pid = 0;
    // bool isScheduleActive = true;


    struct CPUState
    {
        common::uint32_t eax;
        common::uint32_t ebx;
        common::uint32_t ecx;
        common::uint32_t edx;

        common::uint32_t esi;
        common::uint32_t edi;
        common::uint32_t ebp;

        common::uint32_t error;

        common::uint32_t eip;
        common::uint32_t cs;
        common::uint32_t eflags;
        common::uint32_t esp;
        common::uint32_t ss;        
    } __attribute__((packed));

    
    
    
    class Task
    {
        friend class TaskManager;
        private:
            common::uint8_t stack[4096]; // 4 KiB
            CPUState* cpustate;

            common::uint32_t pid;
            common::uint32_t ppid;
            common::uint32_t waitingPid;

            Status status;
            Priority priority;
            int executionTime = 0;
        
        public:
            Task(GlobalDescriptorTable *gdt, void entrypoint());
            Task();
            ~Task();
    };
    
    
    class TaskManager
    {
    private:
        Task* tasks[256];
        int numTasks;
        int currentTask;
        
        bool isWaitingForChild(int pid);
        common::uint32_t interrupt_count;

        GlobalDescriptorTable* gdt;
        Task* getTaskByPid(common::uint32_t pid);

        void PrintProcessTable();
        int findNextTask();
        int findNextTaskByPriority();

    public:
        TaskManager(GlobalDescriptorTable* gdt);
        ~TaskManager();
        bool AddTask(Task* task);
        CPUState* Schedule(CPUState* cpustate);

        /*system calls*/
        common::uint32_t sys_execve(void entry_point());
        common::uint32_t sys_fork(CPUState* cpustate);
        bool sys_waitpid(common::uint32_t childPid);
        bool sys_exit();
        void sys_nice(Priority priority);

    };
    
    
    
}


#endif