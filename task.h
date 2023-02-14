/**
 * Representation of a task in the system.
 */

#ifndef TASK_H
#define TASK_H

// representation of a task
typedef struct task {
    char *name;
    int tid;
    int priority;
    int burst;
} Task;

typedef struct info {
    char *name;
    int tat;
    int wt;
    int rt;
} Info;

#endif
