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

void sort_by_remaining_time(Process queue[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (queue[j].remaining_time > queue[j + 1].remaining_time) {
                Process temp = queue[j];
                queue[j] = queue[j + 1];
                queue[j + 1] = temp;
            }
        }
    }
}

void sort_by_priority(Process queue[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (queue[j].priority > queue[j + 1].priority) {
                Process temp = queue[j];
                queue[j] = queue[j + 1];
                queue[j + 1] = temp;
            }
        }
    }
}

void update_waiting_queue(Process waiting_queue[], int *waiting_count, Process ready_queue[], int *ready_count, int current_time) {
    Process *process = &waiting_queue[0];
    if (process->io_burst_time == 0) {
        enqueue(ready_queue, ready_count, *process);
        for (int i = 0; i < *waiting_count - 1; i++) {
            waiting_queue[i] = waiting_queue[i + 1];
        }
        (*waiting_count)--;
    }
    process->io_burst_time--;
    //printf("io burst: %d: \n", waiting_queue[0].io_burst_time);
}

void move_to_ready_queue(Process processes[], int process_count, Process ready_queue[], int *ready_count, Process waiting_queue[], int waiting_count, int current_time) {
    for (int i = 0; i < process_count; i++) {
        int is_waiting = 0;
        int is_ready = 0;

        for (int j = 0; j < waiting_count; j++) {
            if (processes[i].pid == waiting_queue[j].pid) { // 해당 프로세스가 waiting queue에 이미 있으면
                is_waiting = 1;
                //printf("in wait: %d\n", processes[i].pid);
                break;
            }
        }
        for (int j = 0; j < *ready_count; j++) {
            if  (processes[i].pid == ready_queue[j].pid) { // 해당 프로세스가 ready queue에 이미 있으면
                is_ready = 1;
                //printf("in ready: %d\n", processes[i].pid);
                break;
            }
        }
        if (!is_waiting && !is_ready && processes[i].arrival_time == current_time) {
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
    //printf("PID\tArrival\tBurst\tStart\tEnd\tWait\tTurnaround\n");

    while (completed_processes < process_count) {
        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, waiting_queue, waiting_count, current_time);
        update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);

        
        if (!is_empty(ready_count)) {   //ready queue에 process가 있다면
            Process process = dequeue(ready_queue, &ready_count);
            // interrupt 고려해서 dequeue 될 때마다 초기화
            process.start_time = current_time;

            while (process.remaining_time > 0) {
                if (process.io_interrupt_timing == 0) { // interrupt 발생 시
                    process.io_interrupt_timing = -1;           // 2번 다시 실행 안되도록 만듦

                    id = process.pid - 1;
                    processes[id].waiting_time -= (processes[id].io_burst_time + processes[id].cpu_burst_time - processes[id].remaining_time);

                    enqueue(waiting_queue, &waiting_count, process);

                    break;

                } else {
                    update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);
                    Update_Process(&process, 1);
                    chart[current_time] = process.pid;
                    current_time++;
                    
                    // 중간에 새로 들어오는 애들 queue에 추가
                    move_to_ready_queue(processes, process_count, ready_queue, &ready_count, waiting_queue, waiting_count, current_time);
                    
                }
            }
            if (process.remaining_time == 0) {
                process.end_time = current_time;
                process.waiting_time = process.start_time - process.arrival_time;
                process.turnaround_time = process.end_time - process.arrival_time;

                //printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", process.pid, process.arrival_time, process.cpu_burst_time, process.start_time, process.end_time, process.waiting_time, process.turnaround_time);
                id = process.pid - 1;
                processes[id].waiting_time += process.waiting_time;
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
    
    //printf("PID\tArrival\tBurst\tStart\tEnd\tWait\tTurnaround\n");

    while (completed_processes < process_count) {
        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, waiting_queue, waiting_count, current_time);
        update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);
        sort_by_remaining_time(ready_queue, ready_count);

        if (!is_empty(ready_count)) {
            Process process = dequeue(ready_queue, &ready_count);
            // interrupt 고려해서 dequeue 될 때마다 초기화
            process.start_time = current_time;

            if (preemptive) {   //preemptive
                if (process.io_interrupt_timing == 0) {
                    process.io_interrupt_timing = -1;           // 2번 다시 실행 안되도록 만듦

                    id = process.pid - 1;
                    processes[id].waiting_time -= (current_time - process.start_time + process.io_burst_time);

                    enqueue(waiting_queue, &waiting_count, process);

                    continue;
                    
                } else {
                    Update_Process(&process, 1);
                    chart[current_time] = process.pid;
                    current_time++;
                }
            } else {    // non-preemptive
                while(process.remaining_time > 0) {
                    if (process.io_interrupt_timing == 0) {
                        process.io_interrupt_timing = -1;           // 2번 다시 실행 안되도록 만듦

                        id = process.pid - 1;
                        //processes[id].waiting_time -= (current_time - process.start_time + process.io_burst_time);
                        processes[id].waiting_time -= (processes[id].io_burst_time + processes[id].cpu_burst_time - processes[id].remaining_time);

                        enqueue(waiting_queue, &waiting_count, process);
                        
                        break;

                    } else {
                        update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);
                        Update_Process(&process, 1);
                        chart[current_time] = process.pid;
                        current_time++;

                        // 중간에 새로 들어오는 애들 queue에 추가
                        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, waiting_queue, waiting_count, current_time);
                    }
                }      
            }
            
            if (process.remaining_time == 0) {
                process.end_time = current_time;
                process.waiting_time = process.start_time - process.arrival_time;
                process.turnaround_time = process.end_time - process.arrival_time;

                if(preemptive) {    // 158줄 process.start_time = current_time; 문제 해결용
                    process.waiting_time = process.end_time - process.arrival_time - process.cpu_burst_time;
                }

                //printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", process.pid, process.arrival_time, process.cpu_burst_time, process.start_time, process.end_time, process.waiting_time, process.turnaround_time);
                id = process.pid - 1;
                processes[id].waiting_time += process.waiting_time;
                processes[id].turnaround_time = process.turnaround_time;
                completed_processes++;

            } else {
                if (preemptive) {
                    enqueue(ready_queue, &ready_count, process);
                }
            }
        } else {
            chart[current_time] = -1;
            current_time++;
        }

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
    //printf("PID\tArrival\tBurst\tStart\tEnd\tWait\tTurnaround\n");

    while (completed_processes < process_count) {
        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, waiting_queue, waiting_count, current_time);
        update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);
        sort_by_priority(ready_queue, ready_count);

        if (!is_empty(ready_count)) {
            Process process = dequeue(ready_queue, &ready_count);
            // interrupt 고려해서 dequeue 될 때마다 초기화
            process.start_time = current_time;

            if (preemptive) {   //preemptive    deque하고 enque하므로 priority가 같으면 둘이 번갈아 가면서 실행함
                if (process.io_interrupt_timing == 0) {
                    process.io_interrupt_timing = -1;           // 2번 다시 실행 안되도록 만듦

                    id = process.pid - 1;
                    processes[id].waiting_time -= (current_time - process.start_time + process.io_burst_time);

                    enqueue(waiting_queue, &waiting_count, process);

                    continue;
                    
                } else {
                    Update_Process(&process, 1);
                    chart[current_time] = process.pid;
                    current_time++;
                }
            } else {    // non-preemptive
                while(process.remaining_time > 0) {
                    if (process.io_interrupt_timing == 0) {
                        process.io_interrupt_timing = -1;           // 2번 다시 실행 안되도록 만듦

                        id = process.pid - 1;
                        //processes[id].waiting_time -= (current_time - process.start_time + process.io_burst_time);
                        processes[id].waiting_time -= (processes[id].io_burst_time + processes[id].cpu_burst_time - processes[id].remaining_time);

                        enqueue(waiting_queue, &waiting_count, process);
                        
                        break;

                    } else {
                        update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);
                        Update_Process(&process, 1);
                        chart[current_time] = process.pid;
                        current_time++;

                        // 중간에 새로 들어오는 애들 queue에 추가
                        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, waiting_queue, waiting_count, current_time);
                    }
                }      
            }
            
            if (process.remaining_time == 0) {
                process.end_time = current_time;
                process.waiting_time = process.start_time - process.arrival_time;
                process.turnaround_time = process.end_time - process.arrival_time;

                if(preemptive) {    // 158줄 process.start_time = current_time; 문제 해결용
                    process.waiting_time = process.end_time - process.arrival_time - process.cpu_burst_time;
                }

                //printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", process.pid, process.arrival_time, process.cpu_burst_time, process.start_time, process.end_time, process.waiting_time, process.turnaround_time);
                id = process.pid - 1;
                processes[id].waiting_time += process.waiting_time;
                processes[id].turnaround_time = process.turnaround_time;
                completed_processes++;

            } else {
                if (preemptive) {
                    enqueue(ready_queue, &ready_count, process);
                }
            }
        } else {
            chart[current_time] = -1;
            current_time++;
        }

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
    int interrupt = 0;

    printf("Round Robin Scheduling\n");
    //printf("PID\tArrival\tBurst\tStart\tEnd\tWait\tTurnaround\n");

    while (completed_processes < process_count) {
        update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time); // 인터럽트 끝나고 들어올 때 맨 앞으로 들어오라고
        move_to_ready_queue(processes, process_count, ready_queue, &ready_count, waiting_queue, waiting_count, current_time);

        if (!is_empty(ready_count)) {
            Process process = dequeue(ready_queue, &ready_count);
            // interrupt 고려해서 dequeue 될 때마다 초기화
            process.start_time = current_time;

            int time_slice = (process.remaining_time > time_quantum) ? time_quantum : process.remaining_time;
            for (int i = 0; i < time_slice; i++) {
                interrupt = 0;
                if (process.io_interrupt_timing == 0) {
                    process.io_interrupt_timing = -1;           // 2번 다시 실행 안되도록 만듦

                    id = process.pid - 1;
                    processes[id].waiting_time -= process.io_burst_time;

                    enqueue(waiting_queue, &waiting_count, process);
                    interrupt = 1;
                    break;
                } else {
                    update_waiting_queue(waiting_queue, &waiting_count, ready_queue, &ready_count, current_time);
                    Update_Process(&process, 1);
                    chart[current_time] = process.pid;
                    current_time++;

                    // 중간에 새로 들어오는 애들 queue에 추가
                    move_to_ready_queue(processes, process_count, ready_queue, &ready_count, waiting_queue, waiting_count, current_time);
                }
                
            }

            if(interrupt) { // enqueue 명령어 실행못하게 만들기 위해
                continue;
            }

            if (process.remaining_time == 0) {
                process.end_time = current_time;
                //process.waiting_time = process.start_time - process.arrival_time;
                process.waiting_time = process.end_time - process.arrival_time - process.cpu_burst_time;
                process.turnaround_time = process.end_time - process.arrival_time;

                //printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", process.pid, process.arrival_time, process.cpu_burst_time, process.start_time, process.end_time, process.waiting_time, process.turnaround_time);
                id = process.pid - 1;
                processes[id].waiting_time += process.waiting_time;
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
