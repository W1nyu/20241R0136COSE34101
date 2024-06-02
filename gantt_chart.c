#include "main.h"

void Gantt_Chart(int chart[], int time_length, const char *algorithm) {
    printf("Gantt Chart for %s:\n", algorithm);
    printf("Time | ");

    for (int i = 0; i < time_length; i++) {
        if (chart[i] == -2) {
            break;
        }
        if (chart[i] == -1) {
            printf("IDLE ");
        } else {
            printf("P%d ", chart[i]);
        }
    }
    printf("\n");
}
