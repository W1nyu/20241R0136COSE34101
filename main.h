#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 10
#define MAX_CHART_LENGTH 300

typedef struct {
    int pid;
    int arrival_time;
    int cpu_burst_time;
    int io_burst_time;
    int priority;
    int remaining_time;
    int io_interrupt_timing;
    int waiting_time;
    int turnaround_time;
    int start_time;
    int end_time;
} Process;

void Create_Process(Process processes[], int process_count);
void Update_Process(Process *process, int elapsed_time);
void Config();
void Schedule(Process processes[], int process_count, int time_quantum);
void FCFS(Process processes[], int process_count, int preemptive, int chart[]);
void SJF(Process processes[], int process_count, int preemptive, int chart[]);
void Priority(Process processes[], int process_count, int preemptive, int chart[]);
void RR(Process processes[], int process_count, int time_quantum, int chart[]);
void Gantt_Chart(int chart[], int time_length, const char *algorithm);
void Evaluation(Process processes[], int process_count);

#endif
