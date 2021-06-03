#include <stdio.h>
#include "oslabs.h"

int min(int a, int b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    new_process.arrival_timestamp = timestamp;
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    } else if (new_process.process_priority < current_process.process_priority) {
        current_process.remaining_bursttime = current_process.remaining_bursttime - (timestamp - current_process.execution_starttime);
        current_process.execution_endtime = 0;
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    } else {
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        return current_process;
    }
}

struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {    
    struct PCB return_pcb = {0, 0, 0, 0, 0, 0, 0};
    if (*queue_cnt == 0) { 
        return return_pcb;
    }

    int highest_priority_thread = 0;
    int highest_priority_value = ready_queue[0].process_priority;
    for (int i=1; i< *queue_cnt; i++) {
        if (ready_queue[i].process_priority < highest_priority_value) {
            highest_priority_thread = i;
            highest_priority_value = ready_queue[i].process_priority;
        }
    }
    return_pcb = ready_queue[highest_priority_thread];
    return_pcb.execution_starttime = timestamp;
    return_pcb.execution_endtime = timestamp + return_pcb.remaining_bursttime;

    int last_element = *queue_cnt - 1;
    if (highest_priority_thread < last_element) {
        ready_queue[highest_priority_thread] = ready_queue[last_element];
    }
    (*queue_cnt)--;

    return return_pcb;
}

struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    new_process.arrival_timestamp = timestamp;
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    } else if (new_process.remaining_bursttime < (current_process.remaining_bursttime - (timestamp - current_process.execution_starttime))) {
        current_process.execution_endtime = 0;
        current_process.remaining_bursttime = current_process.remaining_bursttime - (timestamp - current_process.execution_starttime);
        current_process.execution_starttime = 0;
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    } else {
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        return current_process;
    }
}

struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    struct PCB return_pcb = {0, 0, 0, 0, 0, 0, 0};
    if (*queue_cnt == 0) { 
        return return_pcb;
    }

    int srtp_thread = 0;
    int srtp_value = ready_queue[0].remaining_bursttime;
    for (int i=1; i< *queue_cnt; i++) {
        if (ready_queue[i].remaining_bursttime < srtp_value) {
            srtp_thread = i;
            srtp_value = ready_queue[i].remaining_bursttime;
        }
    }
    return_pcb = ready_queue[srtp_thread];
    return_pcb.execution_starttime = timestamp;
    return_pcb.execution_endtime = timestamp + return_pcb.remaining_bursttime;

    int last_element = *queue_cnt - 1;
    if (srtp_thread < last_element) {
        ready_queue[srtp_thread] = ready_queue[last_element];
    }
    (*queue_cnt)--;

    return return_pcb;
}

struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum) {
    new_process.arrival_timestamp = timestamp;
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        int new_endtime = min(timestamp + new_process.total_bursttime, timestamp + time_quantum);
        new_process.execution_endtime = new_endtime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    } else {
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        return current_process;
    }
}

struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum) {
    struct PCB return_pcb = {0, 0, 0, 0, 0, 0, 0};
    if (*queue_cnt == 0) { 
        return return_pcb;
    }

    int next_thread = 0;
    int min_arrival_time = ready_queue[0].arrival_timestamp;
    for (int i=1; i< *queue_cnt; i++) {
        if (ready_queue[i].arrival_timestamp < min_arrival_time) {
            next_thread = i;
            min_arrival_time = ready_queue[i].arrival_timestamp;
        }
    }

    return_pcb = ready_queue[next_thread];
    for (int i=next_thread; i< (*queue_cnt - 1); i++) {
        ready_queue[i] = ready_queue[i+1];
    }
    (*queue_cnt)--;

    return_pcb.execution_starttime = timestamp;
    int new_endtime = min(timestamp + return_pcb.remaining_bursttime, timestamp + time_quantum);
    return_pcb.execution_endtime = new_endtime;

    return return_pcb;
}

struct PCB handle_end_of_time_quantum_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, int timestamp, int time_quantum) {
    struct PCB return_pcb = {0, 0, 0, 0, 0, 0, 0};

    if (current_process.process_id == 0) return return_pcb;

    current_process.remaining_bursttime = current_process.remaining_bursttime - time_quantum;
    if (current_process.remaining_bursttime > 0) {
        current_process.arrival_timestamp = timestamp;
        current_process.execution_starttime = 0;
        current_process.execution_endtime = 0;
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;
    }

    int next_thread = 0;
    int min_arrival_time = ready_queue[0].arrival_timestamp;
    for (int i=1; i< *queue_cnt; i++) {
        if (ready_queue[i].arrival_timestamp < min_arrival_time) {
            next_thread = i;
            min_arrival_time = ready_queue[i].arrival_timestamp;
        }
    }

    return_pcb = ready_queue[next_thread];
    for (int i=next_thread; i< (*queue_cnt - 1); i++) {
        ready_queue[i] = ready_queue[i+1];
    }
    (*queue_cnt)--;

    return_pcb.execution_starttime = timestamp;
    int new_endtime = min(timestamp + return_pcb.remaining_bursttime, timestamp + time_quantum);
    return_pcb.execution_endtime = new_endtime;

    return return_pcb;
}