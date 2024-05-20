
#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <syscalls.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>
#include <mystdlib.h>
#include <drivers/amd_am79c973.h>
#include <net/etherframe.h>

// #define GRAPHICSMODE

using namespace myos;
using namespace myos::common;
using namespace myos::syscalls;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;
using namespace myos::net;
using namespace myos::mystd;

class InteractiveMouse : public MouseEventHandler
{
    int8_t x, y;
public:
    InteractiveMouse()
    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);        
    }
    static bool isMouseClicked;
    void OnMouseDown(uint8_t button)
    {
        isMouseClicked = !isMouseClicked;
    }
    static void waitForMouseClick()
    {
        InterruptManager::isWaitingForInput = true;
        while (!isMouseClicked);
        isMouseClicked = false;
        InterruptManager::isWaitingForInput = false;        
    }
    
};
bool InteractiveMouse::isMouseClicked = false;







void long_running_program(int n)
{
    printf("\n------------(Long Running Program)------------\n");

    int result = 0;
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            result += i * j;
        }
    }
    printf("Result: ");
    printfDigit(result);
    printf("\n");
    exit();
}


void Collatz_IO(){
    printf("\n------------(Collatz)------------");
    int n;

    while (true)
    {
        scanf("\nEnter a number: ", &n);

        if (n == 0)
            continue;
        
        if (n == 444)
        {
            printf("Exiting Collatz program");
            exit();
        }
        
        printfDigit(n);
        printf(": ");
        
        while (n != 1)
        {
            if (n % 2 == 0)
                n /= 2;
            else
                n = 3 * n + 1;

            printfDigit(n);
            printf("-");
        }
    }
    
    exit();
}


void binarySearch_IO(int arr[], int size, int key) {
   printf("\n------------(Binary Search)------------ \n");

   

   while (1)
   {
        printf("Waiting for mouse click: ");
        InteractiveMouse::waitForMouseClick();
        
        uint8_t l = 0, r = size - 1;
        printf("Output: ");
        while (l <= r) {
            uint8_t mid = l + (r - l) / 2;
            if (arr[mid] == key){
                printfDigit(mid);
                printf("\n");
            }
            if (arr[mid] < key)
                l = mid + 1;
            else
                r = mid - 1;
        }
   }   
    exit();
}

void linearSearch_IO(int arr[], int size, int key) {

    printf("\n------------(Linear Search)------------ \n");


    while (true)
    {
        printf("Waiting for mouse click: ");
        InteractiveMouse::waitForMouseClick();
        printf("Output: ");
        for(int i=0; i< size; i++) {
            if(arr[i] == key) {
                printfDigit(i);
                printf("\n");
            }
        }
    }
    exit();
}

void collatz(int n){
    printf("\n------------(collatz)------------ \n");
    printfDigit(n);
    printf(": ");
    while (n != 1)
    {
        if (n % 2 == 0)
            n /= 2;
        else
            n = 3 * n + 1;        

        printfDigit(n);
        printf(" ");
        sleep(1000000);
    }
    printf("\n");
    exit();
}

void binarySearch(int arr[], int size, int key) {
   printf("\n------------(Binary Search)------------ \n");
   uint8_t l = 0, r = size - 1;
   printf("Output: ");
   while (l <= r) {
      uint8_t mid = l + (r - l) / 2;
      if (arr[mid] == key){
         printfDigit(mid);
         printf("\n");
         exit();
      }
      if (arr[mid] < key)
         l = mid + 1;
      else
         r = mid - 1;
    sleep(1000000);

   }    
    printf("Not found");
    exit();
}

void linearSearch(int arr[], int size, int key) {

    printf("\n------------(Linear Search)------------ \n");
    printf("Output: ");

    for(int i=0; i< size; i++) {
        if(arr[i] == key){
            printfDigit(i);
            printf("\n");
            exit();
        }
        sleep(1000000);
    }
    printf("Not Found");
    exit();
}

void taskC()
{
    printf("1. child called execv is running\n");
    for (int i = 0; i < 100000000; i++)
    printf("");
    printf("1. child called execv is exited\n");
    exit();
}

void syscalls_test()
{
    int pid = fork();
    if(pid == 0)
    {
        int pid2 = fork();
        if (pid2 == 0)
        {
            printf("2. Child is running\n");
            for (int i = 0; i < 100000000; i++)
                printf("");
            printf("2. Child is exited\n");
            exit();
        }
        else
        {
            printf("1. Child is running\n");
            waitpid(pid2);
            int execpid = execve(taskC);
            for (int i = 0; i < 100000000; i++)
                printf("");
            waitpid(execpid);
            printf("1. Child is exited\n");
            exit();
        }
    }
    else
    {
        printf("Parent is running\n");
        for (int i = 0; i < 100000000; i++)
            printf("");
        waitpid(pid);
        printf("Parent is exited\n");
        exit();
    }
}



void init_partA_1()
{
    /*This Strategy is loading each program 3 times, starting them and will enter an 
    infinite loop until all the processes terminate. */
    const int num_process = 3*2;
    int pids[num_process];

    for(int i = 0; i <num_process; i++)
    {
        pids[i] = fork();
        if(pids[i] == 0)
        {
            switch(i%2)
            {
                case 0:
                    collatz(2713);
                    break;
                case 1:
                    long_running_program(1000);
                    break;
                default:
                    break;
            }
        }
    }
    for(int i = 0; i< num_process; i++)
        waitpid(pids[i]);

    
    printf("All child finished and collected. Init is exiting...\n");
    exit();
}

void init_partC_1()
{
    /*This Strategy is loading each program 3 times, starting them and will enter an 
    infinite loop until all the processes terminate. */
    
    int arr[] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};
    const int num_process = 4;
    int pids[num_process];

    int random_number = rand() % num_process;

    for(int i = 0; i <num_process; i++)
    {
        pids[i] = fork();
        if(pids[i] == 0)
        {
            switch(random_number)
                {
                    case 0:
                        Collatz_IO();
                        break;
                    case 1:
                        binarySearch_IO(arr, 10, 60);
                        break;
                    case 2:
                        linearSearch_IO(arr, 10, 60);
                        break;
                    case 3:
                        long_running_program(1000);
                        break;
                    default:
                        break;
                }
        }
    }


    for(int i = 0; i< num_process; i++)
        waitpid(pids[i]);

    
    printf("All child finished and collected. Init is exiting...\n");
    exit();
}

void init_partC_2()
{
    /*In this strategy, the operating system randomly chooses one of the programs and loads 
        it into memory multiple times, each time creating a new process. These processes then 
        enter an infinite loop, awaiting interactive input events such as mouse clicks or 
        keyboard presses. Upon receiving an input event, the process reacts accordingly before 
        returning to its idle state.  */
    int arr[] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};
    const int num_process = 4;
    int pids[num_process];


    for(int i = 0; i < num_process; i++)
    {
        pids[i] = fork();
        if(pids[i] == 0)
        {
            switch(i)
                {
                    case 0:
                        nice(Priority::LOW);
                        binarySearch_IO(arr, 10, 60);
                        break;
                    case 1:
                        nice(Priority::LOW);
                        long_running_program(1000);
                        break;
                    case 2:
                        nice(Priority::LOW);
                        linearSearch_IO(arr, 10, 60);
                        break;
                    case 3:    
                        nice(Priority::HIGH);
                        Collatz_IO();
                        break;
                    default:
                        break;
                }
        }
    }


    for(int i = 0; i< num_process; i++)
        waitpid(pids[i]);

    
    printf("All child finished and collected. Init is exiting...\n");
    exit();
}

void init_partB_1()
{
    /*The first strategy is randomly choosing one of the programs and 
        loading it into memory 10 times (Same program 10 different processes), 
        start them and will enter an infinite loop until all the processes terminate. */
    const int num_process = 10;
    int pids[num_process];
    int arr[] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};

    int random_number = rand() % 4;
    

    for(int i = 0; i < 10; i++)
    {
        pids[i] = fork();
        if(pids[i] == 0)
        {
            switch(random_number)
            {
                case 0:
                    collatz(9999999);
                    break;
                case 1:
                    binarySearch(arr, 10, 60);
                    break;
                case 2:
                    linearSearch(arr, 10, 60);
                    break;
                case 3:
                    long_running_program(1000);
                    break;
                default:
                    break;
            }
        }
    }
    for(int i = 0; i< num_process; i++)
        waitpid(pids[i]);

    printf("All child finished and collected. Init is exiting...\n");
    exit();
}


void init_partB_2()
{
    /*Second Strategy is choosing 2 out 4 programs randomly and loading 
        each program 3 times start them and will enter an infinite loop until all 
        the processes terminate.  */

    const int num_process = 3*2;
    int pids[num_process];
    int arr[] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};

    int random_numbers[2];
    random_numbers[0] = rand() % 4;
    do
    {
        random_numbers[1] = rand() % 4;
    } while (random_numbers[0] == random_numbers[1]);
    
    
    for(int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            pids[i] = fork();
            if(pids[i] == 0)
            {
                switch(random_numbers[i])
                {
                    case 0:
                        collatz(9999999);
                        break;
                    case 1:
                        binarySearch(arr, 10, 60);
                        break;
                    case 2:
                        linearSearch(arr, 10, 60);
                        break;
                    case 3:
                        long_running_program(1000);
                        break;
                    default:
                        break;
                }
            }
        }
        
    }

    for(int i = 0; i< num_process; i++)
        waitpid(pids[i]);

    printf("All child finished and collected. Init is exiting...\n");
    exit();
}

/*Third Strategy, init process will initialize Process Table and ready 
        queue, let the collatz program is in the ready queue with the lowest 
        priority, and after the 5th interrupt (we expect that the collatz lasts very 
        longer than this of course), remaining programs will arrive as their 
        priorities are the same.  .  */
void init_partB_3()
{
    
    const int num_process = 4;
    
    int pids[num_process];
    int arr[] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170, 200, 250,
                 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900};


    // Initialize Process Table
    for (int i = 0; i < num_process; i++)
    {
        pids[i] = fork();
        
        if (pids[i] == 0)
        {
            switch(i)
            {
                
                case 0:
                    nice(Priority::HIGH);
                    collatz(7542186342);
                    break;
                case 1:
                    binarySearch(arr, 25, 20);
                    break;
                case 2:
                    linearSearch(arr, 25, 20);
                    break;
                case 3:
                    long_running_program(10000);
                    break;
                default:
                    break;
            }
        }
    }

    for(int i = 0; i< num_process; i++)
        waitpid(pids[i]);

    printf("All child finished and collected. Init is exiting...\n");
    exit();
}

void init_partB_4()
{
    printf("init_partB_4 is running\n");
    const int num_process = 4;
    
    int pids[num_process];
    int arr[] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170, 200, 250,
                 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900};


    // Initialize Process Table
    for (int i = 0; i < num_process; i++)
    {
        pids[i] = fork();
        
        if (pids[i] == 0)
        {
            switch(i)
            {
                
                case 0:
                    nice(Priority::VERY_HIGH);
                    collatz(12415);
                    break;
                case 1:
                    binarySearch(arr, 25, 20);
                    break;
                case 2:
                    linearSearch(arr, 25, 20);
                    break;
                case 3:
                    long_running_program(10000);
                    break;
                default:
                    break;
            }
        }
    }

    for(int i = 0; i< num_process; i++)
        waitpid(pids[i]);

    printf("All child finished and collected. Init is exiting...\n");
    exit();
}







typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("Welcome to My Operating System LOOS\n");


    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);    
    void* allocated = memoryManager.malloc(1024);

    GlobalDescriptorTable gdt;    
    TaskManager taskManager(&gdt);

    // PART A

    #ifdef SYSCALL_TEST
        Task task1(&gdt, syscalls_test);
        taskManager.AddTask(&task1);
    #endif

    #ifdef PARTA_1
        Task initTask(&gdt, init_partA_1);
        taskManager.AddTask(&initTask);
    #endif



    // PART B
    #ifdef PARTB_1
        Task initTask(&gdt, init_partB_1);
        taskManager.AddTask(&initTask);
    #endif
  
    #ifdef PARTB_2
        Task initTask(&gdt, init_partB_2);
        taskManager.AddTask(&initTask);
    #endif

    #ifdef PARTB_3
        Task initTask(&gdt, init_partB_3);
        taskManager.AddTask(&initTask);
    #endif

    #ifdef PARTB_4
        Task initTask(&gdt, init_partB_4);
        taskManager.AddTask(&initTask);
    #endif


    // PART C
    #ifdef PARTC_1
        Task initTask(&gdt, init_partC_1);
        taskManager.AddTask(&initTask);
    #endif
  
    #ifdef PARTC_2
        Task initTask(&gdt, init_partC_2);
        taskManager.AddTask(&initTask);
    #endif


    
    DriverManager drvManager;
    
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80);

    InteractiveMouse mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);

    PrintfKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);

    drvManager.AddDriver(&keyboard);
    drvManager.AddDriver(&mouse);
    drvManager.ActivateAll();
    interrupts.Activate();

    
    while(1)
    {
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}
