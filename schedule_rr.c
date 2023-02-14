// Author: Xiao Gao
// Date: Feb 13 2023
// Description: Perform RR scheduling algorithm to schedule tasks for CPU. 


#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

// time quantum
const int quantum = 10;

// head node of the list
struct node *g_head;

// head node for temporary list
struct node *temp_head;

// counter to increment and create tid's
int tid = 1;

// keep track of time
int time = 0;

// infos array to store all the time info
Info **infos;

// used to compute utilization rate
int num_of_runs = 0;
// declartion of helper method
Task *pickNextTask();
void fill_info(Info *info, Task *task);

// add a task to the list
void add(char *name, int priority, int burst)
{
    // create the task to be inserted into the list
    Task *task = (Task *)malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;
    task->tid = tid;

    // increment tid for next task
    tid++;

    // insert the task into the list
    insert(&g_head, task);
}

bool comesBefore(char *a, char *b) { return strcmp(a, b) < 0; }

// based on traverse from list.c
// finds the task whose name comes first in dictionary
Task *pickNextTask()
{
    // if list is empty, nothing to do
    if (!g_head)
        return NULL;

    struct node *temp;
    temp = g_head;
    Task *best_sofar = temp->task;

    while (temp != NULL)
    {
        if (comesBefore(temp->task->name, best_sofar->name))
        {
            best_sofar = temp->task;
        }
        temp = temp->next;
    }
    // delete the node from list, Task will get deleted later
    delete (&g_head, best_sofar);
    return best_sofar;
}

// invoke the scheduler
void schedule()
{
    // allocate memory for Info array
    infos = malloc(100 * sizeof(infos));
    for (int i = 0; i < 100; i++)
        infos[i] = NULL;
    temp_head = NULL;
    // iterate through the list of tasks
    while (g_head != NULL)
    {
        Task *task = NULL;
        // identify the next task to be run
        task = pickNextTask();

        int run_time = -1;
        if (task->burst < quantum)
        {
            run_time = task->burst;
        }
        else if (task->burst >= quantum)
        {
            run_time = quantum;
        }

        int idx = task->tid - 1;
        if (infos[idx] == NULL)
        {
            Info *info = (Info *)malloc(sizeof(Info));
            info->name = task->name;
            info->rt = -1;
            info->wt = task->burst;
            infos[idx] = info;
        }

        // run the next task and increment num_of_runs
        run(task, run_time);
        num_of_runs++;
        // subtract current run time from total burst time
        task->burst -= run_time;
        // increment the current total time
        time += run_time;
        if (infos[idx]->rt == -1)
            infos[idx]->rt = time - run_time;
        // if there is remaining burst time in the process, add it to the temp list
        if (task->burst > 0)
        {
            insert(&temp_head, task);
        }
        else
        {
            infos[idx]->tat = time;
            infos[idx]->wt = time - infos[idx]->wt;
        }

        printf("        Time is now: %d\n", time);

        // if the main list is empty and the temp list is not, add the values from the temp list into the main list
        if (g_head == NULL && temp_head != NULL)
        {
            struct node *temp = temp_head;
            while (temp != NULL)
            {
                insert(&g_head, temp->task);
                temp = temp->next;
            }

            // delete all values from temp list
            while (temp_head != NULL)
            {
                delete (&temp_head, temp_head->task);
            }
        }
    }
}

void fill_info(Info *info, Task *task)
{
}

void print_utilization()
{
    printf("\n");
    float finish_time = (float)time;
    float dispatcher_time = finish_time + (float)num_of_runs;
    float rate = finish_time / dispatcher_time;
    rate *= 100;
    printf("CPU Utilization: %.2f%c\n", rate, 37);
}

void generate_table()
{
    printf("\n");
    printf("...| ");
    for (int i = 0; i < tid - 1; i++)
    {
        printf("%-5s", infos[i]->name);
        printf(" | ");
    }
    printf("\n");
    printf("TAT| ");
    for (int i = 0; i < tid - 1; i++)
    {
        printf("%-5d", infos[i]->tat);
        printf(" | ");
    }
    printf("\n");
    printf("WT | ");
    for (int i = 0; i < tid - 1; i++)
    {
        printf("%-5d", infos[i]->wt);
        printf(" | ");
    }
    printf("\n");
    printf("RT | ");
    for (int i = 0; i < tid - 1; i++)
    {
        printf("%-5d", infos[i]->rt);
        printf(" | ");
    }
    printf("\n");
    for (int i = 0; i < 100; i++)
    {
        free(infos[i]);
    }
    free(infos);
    free(g_head);
}
