// Author: Xiao Gao
// Date: Feb 12 2023
// Description: Perform SJF scheduling algorithm to schedule tasks for CPU. 

#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

// head node of the list
struct node *g_head;

// counter to increment and create tid's
int tid = 1;

// var to keep track of time
int time = 0;

// infos array to store all the time info
Info **infos;
int idx = 0;

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

  // holds the current shortest task burst time
  int shortest = INT_MAX;

  while (temp != NULL)
  {
    if (temp->task->burst < shortest)
    {
      shortest = temp->task->burst;
      best_sofar = temp->task;
    }
    else if (temp->task->burst == shortest)
    {
      if (comesBefore(temp->task->name, best_sofar->name))
      {
        best_sofar = temp->task;
      }
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
  // iterate through the list of tasks
  while (g_head != NULL)
  {
    Task *task = (Task *)malloc(sizeof(Task));
    Info *info = (Info *)malloc(sizeof(Info));
    // identify the next task to be run
    task = pickNextTask();

    // run the next task
    run(task, task->burst);

    // increment the current time
    time += task->burst;
    fill_info(info, task);
    infos[task->tid - 1] = info;
    idx++;
    free(task);
    printf("        Time is now: %d\n", time);
  }
}

void fill_info(Info *info, Task *task)
{
  info->name = task->name;
  // TAT = Exit time - Arrival time, in this case, arrival time = 0
  // so TAT = exit time
  info->tat = time;
  // in FCFS, RT = WT = TAT - burst time
  info->rt = time - task->burst;
  info->wt = time - task->burst;
}

void print_utilization()
{
  printf("\n");
  float finish_time = (float)time;
  float dispatcher_time = finish_time + (float)tid - 2;
  float rate = finish_time / dispatcher_time;
  rate *= 100;
  printf("CPU Utilization: %.2f%c\n", rate, 37);
}

void generate_table()
{
  printf("\n");
  printf("...| ");
  for (int i = 0; i < idx; i++)
  {
    printf("%-5s", infos[i]->name);
    printf(" | ");
  }
  printf("\n");
  printf("TAT| ");
  for (int i = 0; i < idx; i++)
  {
    printf("%-5d", infos[i]->tat);
    printf(" | ");
  }
  printf("\n");
  printf("WT | ");
  for (int i = 0; i < idx; i++)
  {
    printf("%-5d", infos[i]->wt);
    printf(" | ");
  }
  printf("\n");
  printf("RT | ");
  for (int i = 0; i < idx; i++)
  {
    printf("%-5d", infos[i]->rt);
    printf(" | ");
  }
  printf("\n");
  for (int i = 0; i < idx; i++)
  {
    free(infos[i]);
  }
  free(infos);
  free(g_head);
}
