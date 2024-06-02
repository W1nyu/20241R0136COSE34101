#include "main.h"

void Evaluation(Process processes[], int process_count) {
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    for (int i = 0; i < process_count; i++) {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }
    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / process_count);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / process_count);
}
