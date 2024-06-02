#include "main.h"

void Config() {
    srand(time(0));
}

void Schedule(Process processes[], int process_count, int time_quantum) {
    int chart[MAX_CHART_LENGTH];

    FCFS(processes, process_count, 0, chart);
    Gantt_Chart(chart, MAX_CHART_LENGTH, "FCFS");
    Evaluation(processes, process_count);
    printf("\n\n");
    /*

    SJF(processes, process_count, 0, chart);
    Gantt_Chart(chart, MAX_CHART_LENGTH, "Non-Preemptive SJF");
    Evaluation(processes, process_count);
    printf("\n\n");
    
    SJF(processes, process_count, 1, chart);
    Gantt_Chart(chart, MAX_CHART_LENGTH, "Preemptive SJF");
    Evaluation(processes, process_count);
    printf("\n\n");
    

    Priority(processes, process_count, 0, chart);
    Gantt_Chart(chart, MAX_CHART_LENGTH, "Non-Preemptive Priority");
    Evaluation(processes, process_count);
    printf("\n\n");


    Priority(processes, process_count, 1, chart);
    Gantt_Chart(chart, MAX_CHART_LENGTH, "Preemptive Priority");
    Evaluation(processes, process_count);
    printf("\n\n");


    RR(processes, process_count, time_quantum, chart);
    Gantt_Chart(chart, MAX_CHART_LENGTH, "Round Robin");
    Evaluation(processes, process_count);
    printf("\n\n");
    */
}

int main() {
    int process_count;
    int time_quantum;

    printf("Enter the number of processes (1-10): ");
    scanf("%d", &process_count);

    if (process_count <= 0 || process_count > 10) {
        printf("Invalid number of processes. Please enter a value between 1 and 10.\n");
        return 1;
    }

    printf("Enter the time quantum for Round Robin: ");
    scanf("%d", &time_quantum);

    if (time_quantum <= 0) {
        printf("Invalid time quantum. Please enter a value greater than 0.\n");
        return 1;
    }

    Config();

    Process processes[MAX_PROCESSES];
    Create_Process(processes, process_count);

    Schedule(processes, process_count, time_quantum);

    return 0;
}
