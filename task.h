#ifndef __TASK_H__
#define __TASK_H__

#include "process.h"

void init_tasks(void);

typedef struct task *Task;

Task Task_create(int priority, void (*code)(void));

Task *get_next_scheduled_task(void);
void *schedule_task(Task task);

void release_processor();

#endif
