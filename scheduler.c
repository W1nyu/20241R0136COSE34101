#include "main.h"

// Queue utility functions
void enqueue(Process queue[], int *count, Process process) {
    if (*count < MAX_PROCESSES) {
        queue[(*count)++] = process;
    }
}

Process dequeue(Process queue[], int *count) {
    Process process = queue[0];
    for (int i = 0; i < *count - 1; i++) {
        queue[i] = queue[i + 1];
    }
    (*count)--;
    return process;
}

int is_empty(int count) {
    return count == 0;
}

void update_waiting_queue(Process waiting_queue[], int *waiting_count, Process ready_queue[], int *ready_count, int current_time) {
    for (int i = 0; i < *waiting_count; i++) {
        Process *process = &waiting_queue[i];
        if (process->io_burst_time > 0) {
            process->io_burst_time--;
            if (process->io_burst_time == 0) {
                enqueue(ready_queue, ready_count, *process);
                for (int j = i; j < *waiting_count - 1; j++) {
                    waiting_queue[j] = waiting_queue[j + 1];
                }
                (*waiting_count)--;
                i--; // Adjust index since we removed an element
            }
        }
    }
}

void move_to_ready_queue(Process processes[], int process_count, Process ready_queue[], int *ready_count, int current_time) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].arrival_time == current_time) {
            enqueue(ready_queue, ready_count, processes[i]);
        }
    }
}

void FCFS(Process processes[], int process_count, int preemptive, int chart[]) {
    Process ready_queue[MAX_PROCESSES];
    Process waiting_queue[MAX_PROCESSES];
    int ready_count = 0, waiting_count = 0;
    int current_time = 0;
    int completed_processes = 0;
    int id = 0;

    printf("FCFS Scheduling\n");
    printf("PID\tArrival\tBurst\tStart\tEnd\tWait\tTurnaround\n");

    while (completed_processes < process_count) {
        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, current_time);
        //update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);
        
        if (!is_empty(ready_count)) {   //ready queue에 process가 있다면
            Process process = dequeue(ready_queue, &ready_count);
            if (process.start_time == -1) {
                process.start_time = current_time;
            }
            if (current_time < process.arrival_time) {
                current_time = process.arrival_time;
            }

            while (process.remaining_time > 0) {
                /*
                if (process.io_interrupt_timing == 0) {
                    
                    enqueue(waiting_queue, &waiting_count, process);
                    // deque
                    for (int i = 0; i < ready_count - 1; i++) {
                        ready_queue[i] = ready_queue[i + 1];
                    }
                    ready_count--;
                    break;
                }
                */

                Update_Process(&process, 1);
                chart[current_time] = process.pid;
                current_time++;
                move_to_ready_queue(processes, process_count, ready_queue, &ready_count, current_time);
            }
            if (process.remaining_time == 0) {
                process.end_time = current_time;
                process.waiting_time = process.start_time - process.arrival_time;
                process.turnaround_time = process.end_time - process.arrival_time;

                //printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", process.pid, process.arrival_time, process.cpu_burst_time, process.start_time, process.end_time, process.waiting_time, process.turnaround_time);
                id = process.pid - 1;
                processes[id].waiting_time = process.waiting_time;
                processes[id].turnaround_time = process.turnaround_time;
                completed_processes++;
            }
        } else {
            chart[current_time] = -1;
            current_time++;
        }
    }
    chart[current_time] = -2;
}

void SJF(Process processes[], int process_count, int preemptive, int chart[]) {
    Process ready_queue[MAX_PROCESSES];
    Process waiting_queue[MAX_PROCESSES];
    int ready_count = 0, waiting_count = 0;
    int current_time = 0;
    int completed_processes = 0;
    int id = 0;

    if (preemptive) printf("SJF Scheduling(preemptive)\n");
    else printf("SJF Scheduling(non-preemptive)\n");
    
    printf("PID\tArrival\tBurst\tStart\tEnd\tWait\tTurnaround\n");

    while (completed_processes < process_count) {
        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, current_time);
        //update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);
        int min_index = -1;
        
        for (int i = 0; i < ready_count; i++) { // SJ pid 선택
            if ((min_index == -1 || ready_queue[i].remaining_time < ready_queue[min_index].remaining_time) && ready_queue[i].arrival_time <= current_time) {
                min_index = i;
            }
        }
        if (min_index == -1) {  // 아무것도 안들어옴
            chart[current_time] = -1;
            current_time++;
            continue;
        }
        Process *process = &ready_queue[min_index];
        if (process->start_time == -1) {
            process->start_time = current_time;
        }
        if (current_time < process->arrival_time) {
            current_time = process->arrival_time;
        }
        if (preemptive) {
            /*
            if (process->io_interrupt_timing == 0) {
                enqueue(waiting_queue, &waiting_count, process);
                break;
            }
            */
            Update_Process(process, 1);
            chart[current_time] = process->pid;
            current_time++;
            
        } else {
            while(process->remaining_time > 0) {
                /*
                if (process->io_interrupt_timing == 0) {
                    enqueue(waiting_queue, &waiting_count, process);
                    break;
                }
                */
                Update_Process(process, 1);
                chart[current_time] = process->pid;
                current_time++;
                move_to_ready_queue(processes, process_count, ready_queue, &ready_count, current_time);
            }
        }

        if (process->remaining_time == 0) {
            process->end_time = current_time;
            process->waiting_time = process->start_time - process->arrival_time;
            process->turnaround_time = process->end_time - process->arrival_time;

            //printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", process->pid, process->arrival_time, process->cpu_burst_time, process->start_time, process->end_time, process->waiting_time, process->turnaround_time);
            id = process->pid - 1;
            processes[id].waiting_time = process->waiting_time;
            processes[id].turnaround_time = process->turnaround_time;
            completed_processes++;
        } else {
            if (preemptive) {
                continue;
            }
            //waiting_queue[waiting_count++] = *process;
        }
        // deque
        for (int i = min_index; i < ready_count - 1; i++) {
            ready_queue[i] = ready_queue[i + 1];
        }
        ready_count--;

    }
    chart[current_time] = -2;
}

void Priority(Process processes[], int process_count, int preemptive, int chart[]) {
    Process ready_queue[MAX_PROCESSES];
    Process waiting_queue[MAX_PROCESSES];
    int ready_count = 0, waiting_count = 0;
    int current_time = 0;
    int completed_processes = 0;
    int id = 0;

    if (preemptive) printf("Priority Scheduling(preemptive)\n");
    else printf("Priority Scheduling(non-preemptive)\n");
    printf("PID\tArrival\tBurst\tStart\tEnd\tWait\tTurnaround\n");

    while (completed_processes < process_count) {
        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, current_time);
        update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);

        int min_index = -1;
        for (int i = 0; i < ready_count; i++) {
            if ((min_index == -1 || ready_queue[i].priority < ready_queue[min_index].priority) && ready_queue[i].arrival_time <= current_time) {
                min_index = i;
            }
        }

        if (min_index == -1) {
            current_time++;
            continue;
        }

        Process *process = &ready_queue[min_index];
        if (process->start_time == -1) {
            process->start_time = current_time;
        }
        if (current_time < process->arrival_time) {
            current_time = process->arrival_time;
        }

        if (preemptive) {
            /*
            if (process->io_interrupt_timing == 0) {
                enqueue(waiting_queue, &waiting_count, process);
                break;
            }
            */
            Update_Process(process, 1);
            chart[current_time] = process->pid;
            current_time++;
        } else {
            while(process->remaining_time > 0) {
                /*
                if (process->io_interrupt_timing == 0) {
                    enqueue(waiting_queue, &waiting_count, process);
                    break;
                }
                */
                Update_Process(process, 1);
                chart[current_time] = process->pid;
                current_time++;
                move_to_ready_queue(processes, process_count, ready_queue, &ready_count, current_time);
            }
        }

        if (process->remaining_time == 0) {
            process->end_time = current_time;
            process->waiting_time = process->start_time - process->arrival_time;
            process->turnaround_time = process->end_time - process->arrival_time;

            //printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", process->pid, process->arrival_time, process->cpu_burst_time, process->start_time, process->end_time, process->waiting_time, process->turnaround_time);
            id = process->pid - 1;
            processes[id].waiting_time = process->waiting_time;
            processes[id].turnaround_time = process->turnaround_time;
            completed_processes++;
        } else {
            //waiting_queue[waiting_count++] = *process;
            if (preemptive) {
                continue;
            }
        }

        for (int i = min_index; i < ready_count - 1; i++) {
            ready_queue[i] = ready_queue[i + 1];
        }
        ready_count--;
        
    }
    chart[current_time] = -2;
}

void RR(Process processes[], int process_count, int time_quantum, int chart[]) {
    Process ready_queue[MAX_PROCESSES];
    Process waiting_queue[MAX_PROCESSES];
    int ready_count = 0, waiting_count = 0;
    int current_time = 0;
    int completed_processes = 0;
    int id = 0;

    printf("Round Robin Scheduling\n");
    printf("PID\tArrival\tBurst\tStart\tEnd\tWait\tTurnaround\n");

    while (completed_processes < process_count) {
        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, current_time);
        //update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);

        if (!is_empty(ready_count)) {
            Process process = dequeue(ready_queue, &ready_count);
            if (process.start_time == -1) {
                process.start_time = current_time;
            }
            if (current_time < process.arrival_time) {
                current_time = process.arrival_time;
            }

            int time_slice = (process.remaining_time > time_quantum) ? time_quantum : process.remaining_time;
            for (int i = 0; i < time_slice; i++) {
                /*
                if (process.io_interrupt_timing != -1 && process.remaining_time == process.io_interrupt_timing) {
                    enqueue(waiting_queue, &waiting_count, process);
                    break;
                }
                */
                Update_Process(&process, 1);
                chart[current_time] = process.pid;
                current_time++;
                move_to_ready_queue(processes, process_count, ready_queue, &ready_count, current_time);
            }

            if (process.remaining_time == 0) {
                process.end_time = current_time;
                process.waiting_time = process.start_time - process.arrival_time;
                process.turnaround_time = process.end_time - process.arrival_time;

                //printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", process.pid, process.arrival_time, process.cpu_burst_time, process.start_time, process.end_time, process.waiting_time, process.turnaround_time);
                id = process.pid - 1;
                processes[id].waiting_time = process.waiting_time;
                processes[id].turnaround_time = process.turnaround_time;
                completed_processes++;
            } else {
                enqueue(ready_queue, &ready_count, process);
            }
        } else {
            chart[current_time] = -1;
            current_time++;
        }
    }
    chart[current_time] = -2;
}
