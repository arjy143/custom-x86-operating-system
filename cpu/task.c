#include "task.h"
#include "types.h"
#include "memory.h"
#include "panic.h"

extern void context_switch(uint32_t* old_esp_ptr, uint32_t new_esp);

static struct task* current_task = NULL;
static struct task* task_list = NULL;
static uint32_t next_pid = 1;

void task_init()
{
    //create task for kernel main
    struct task* main_task = (struct task*)malloc(sizeof(struct task));
    main_task->pid = 0;
    main_task->state = TASK_RUNNING;
    
    //initially point to itself
    main_task->next = main_task;
    
    current_task = main_task;
    task_list = main_task;
}

struct task* task_create(void (*entry)(void))
{
    struct task* task = (struct task*)malloc(sizeof(struct task));
    task->pid = next_pid++;
    task->state = TASK_READY;

    //allocate a stack for the task
    uint8_t* stack = (uint8_t*)malloc(TASK_STACK_SIZE);
    uint32_t* stack_top = (uint32_t*)(stack + TASK_STACK_SIZE);

    //stack must be set up so that context_switch can return into the entry function
    *(--stack_top) = (uint32_t)entry;

    //zeroing registers ebp, edi, esi, ebx
    *(--stack_top) = 0;
    *(--stack_top) = 0;
    *(--stack_top) = 0;
    *(--stack_top) = 0;

    task->esp = (uint32_t)stack_top;

    //insert into the circular linked list
    task->next = current_task->next;
    current_task->next = task;

    return task;
}

void task_yield()
{
    //dont switch if there's no other task to switch to
    if (current_task == NULL || current_task->next == current_task)
    {
        return;
    }

    struct task* old = current_task;
    struct task* new_task = current_task->next;

    old->state = TASK_READY;
    new_task->state = TASK_RUNNING;
    current_task = new_task;

    context_switch(&old->esp, new_task->esp);
}

struct task* task_current()
{
    return current_task;
}

