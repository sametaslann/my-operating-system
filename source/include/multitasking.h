 
#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace myos
{

    enum Status{READY, RUNNING, WAITING, TERMINATED};
    static common::uint32_t next_pid = 1;

    
    struct CPUState
    {
        common::uint32_t eax;
        common::uint32_t ebx;
        common::uint32_t ecx;
        common::uint32_t edx;

        common::uint32_t esi;
        common::uint32_t edi;
        common::uint32_t ebp;

        /*
        common::uint32_t gs;
        common::uint32_t fs;
        common::uint32_t es;
        common::uint32_t ds;
        */
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
        Status status;
        common::uint32_t pid;
        common::uint32_t ppid;
        common::uint16_t waitingPid;


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
        GlobalDescriptorTable* gdt;
        Task* getTaskByPid(common::uint32_t pid);

    public:
        TaskManager(GlobalDescriptorTable* gdt);
        ~TaskManager();
        bool AddTask(Task* task);
        void PrintProcessTable();


        common::uint32_t sys_execve(void entry_point());
        common::uint32_t sys_fork(CPUState* cpustate);
        bool sys_waitpid(common::uint32_t childPid);
        bool sys_exit();

        CPUState* Schedule(CPUState* cpustate);
    };
    
    
    
}


#endif