#ifndef TASK_H
#define TASK_H

#include "types.h"

//4kb stack per task
#define TASK_STACK_SIZE 4096
#define MAX_TASKS 16

#define TASK_READY 0
#define TASK_RUNNING 1
#define TASK_BLOCKED 2
#define TASK_TERMINATED 3

//implmenting round robin scheduler initially
struct task
{
    //store a task's stack when context switching. all the necessary state will have been pushed onto this stack beforehand, so this can be popped off when context switching back.
    uint32_t esp;
    uint32_t pid;
    uint8_t state;
    struct task* next;
};

//create new kernel thread/task at this function
struct task* task_create(void (*entry)(void));

//voluntarily yield cpu
void task_yield();

struct task* task_current();

void task_init();

#endif
