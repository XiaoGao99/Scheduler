#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define SIZE 100
// head node of the task list
struct node *g_head;
// initial tid
int tid = 1;
// track time
int time = 0;

Info **infos;
int index = 0;
// add a task to the list
void add(char *name, int priority, int burst)
{
  // create the task struct
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

// call the scheduler
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
    infos[index++] = info;
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
  float rate = time / (time + tid - 1);
  rate *= 100;
  printf("CPU Utilization: %.2f", rate);
}

void generate_table()
{
  print("...| ");
  for (int i = 0; i < index; i++)
  {
    printf("%-8s", infos[i]->name);
    print(" | ");
  }
  print("\n");
  print("TAT| ");
  for (int i = 0; i < index; i++)
  {
    printf("%-8d", infos[i]->tat);
    print(" | ");
  }
  print("\n");
  print("WT | ");
  for (int i = 0; i < index; i++)
  {
    printf("%-8d", infos[i]->wt);
    print(" | ");
  }
  print("\n");
  print("RT | ");
  for (int i = 0; i < index; i++)
  {
    printf("%-8d", infos[i]->rt);
    print(" | ");
  }
  print("\n");
  for (int i = 0; i < index; i++)
  {
    free(infos[i]);
  }
  free(infos);
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
      best_sofar = temp->task;
    temp = temp->next;
  }
  // delete the node from list, Task will get deleted later
  delete (&g_head, best_sofar);
  return best_sofar;
}
