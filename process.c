#include "main.h"

void Create_Process(Process processes[], int process_count) {
    for (int i = 0; i < process_count; i++) {
        processes[i].pid = i + 1;
        processes[i].arrival_time = rand() % 10;
        processes[i].cpu_burst_time = rand() % 10 + 1;
        processes[i].priority = rand() % 10 + 1;
        processes[i].remaining_time = processes[i].cpu_burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].start_time = -1;
        processes[i].end_time = 0;
        processes[i].io_burst_time = 0;
        processes[i].io_interrupt_timing = -1;
    }

    // Assign IO burst to two random processes
    for (int i = 0; i < 2; i++) {
        int index = rand() % process_count;
        if (processes[index].cpu_burst_time > 1) {
            processes[index].io_burst_time = rand() % (processes[index].cpu_burst_time - 1) + 1;
            processes[index].io_interrupt_timing = rand() % processes[index].cpu_burst_time;
        }
    }

    printf("\nPID\tArrival\tBurst\tPriority\tIO Timing\tIO Burst\n");
    for (int i = 0; i < process_count; i++) {
        printf("%d\t%d\t%d\t%d\t\t%d\t\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].cpu_burst_time,
               processes[i].priority, processes[i].io_interrupt_timing, processes[i].io_burst_time);
    }
    printf("\n");
}

void Update_Process(Process *process, int elapsed_time) {
    process->remaining_time -= elapsed_time;
    if (process->remaining_time < 0) {
        process->remaining_time = 0;
    }

    if (process->io_interrupt_timing > 0) {
        process->io_interrupt_timing -= elapsed_time;
        if (process->io_interrupt_timing < 0) {
            process->io_interrupt_timing = 0;
        }
    }
}
