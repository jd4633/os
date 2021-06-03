#include <stdlib.h>
#include "oslabs.h"
#define TRUE 1
#define FALSE 0


struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.process_id == 0) {
        return new_request;
    }

    request_queue[*queue_cnt] = new_request;
    (*queue_cnt)++;
    return current_request;
}

struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX],int *queue_cnt) {
    if (*queue_cnt == 0) {
        struct RCB null_rcb = {0, 0, 0, 0, 0};
        return null_rcb;
    }

    int next = 0;
    int next_at = request_queue[0].arrival_timestamp;
    for (int i=1; i< *queue_cnt; i++) {
        if (request_queue[i].arrival_timestamp < next_at) {
            next = i;
            next_at = request_queue[i].arrival_timestamp;
        }
    }

    struct RCB next_rcb = request_queue[next];
    for (int i=next; i< (*queue_cnt)-1; i++) {
        request_queue[i] = request_queue[i+1];
    }
    (*queue_cnt)--;

    return next_rcb;
}

struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.process_id == 0) {
        return new_request;
    }

    request_queue[*queue_cnt] = new_request;
    (*queue_cnt)++;
    return current_request;
}

struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt,int current_cylinder) {
    if (*queue_cnt == 0) {
        struct RCB null_rcb = {0, 0, 0, 0, 0};
        return null_rcb;
    }

    int next = 0;
    int next_cylinder_distance = abs(request_queue[0].cylinder - current_cylinder);
    int next_at = request_queue[0].arrival_timestamp;
    for (int i=1; i< *queue_cnt; i++) {
        int current_cylinder_distance = abs(request_queue[i].cylinder - current_cylinder);
        if (current_cylinder_distance == next_cylinder_distance) {
            if (request_queue[i].arrival_timestamp < next_at) {
                next = i;
                next_cylinder_distance = current_cylinder_distance;
                next_at = request_queue[i].arrival_timestamp;
            }
        } else if (current_cylinder_distance < next_cylinder_distance) {
            next = i;
            next_cylinder_distance = current_cylinder_distance;
            next_at = request_queue[i].arrival_timestamp;        
        }
    }

    struct RCB next_rcb = request_queue[next];
    for (int i=next; i< (*queue_cnt)-1; i++) {
        request_queue[i] = request_queue[i+1];
    }
    (*queue_cnt)--;

    return next_rcb;
}

struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.process_id == 0) {
        return new_request;
    }

    request_queue[*queue_cnt] = new_request;
    (*queue_cnt)++;
    return current_request;
}

int find_next_rcb_up(struct RCB request_queue[QUEUEMAX],int queue_cnt, int current_cylinder) {
    int next = -1;
    int next_cylinder_distance = -1;
    int next_at = -1;

    for (int i=0; i<queue_cnt; i++) {
        int replace_next = FALSE;
        if (request_queue[i].cylinder >= current_cylinder) {
            if (next == -1) {            
                replace_next = TRUE;
            } else {
                int current_cylinder_distance = request_queue[i].cylinder - current_cylinder;
                if (current_cylinder_distance < next_cylinder_distance) {
                    replace_next = TRUE;
                } else if (current_cylinder_distance == next_cylinder_distance) {
                    if (request_queue[i].arrival_timestamp < next_at) {
                        replace_next = TRUE;
                    }
                }
            }
            if (replace_next == TRUE) {
                next = i;
                next_cylinder_distance = request_queue[i].cylinder - current_cylinder;
                next_at = request_queue[i].arrival_timestamp;
            }
        }
    }

    return next;
}

int find_next_rcb_down(struct RCB request_queue[QUEUEMAX],int queue_cnt, int current_cylinder) {
    int next = -1;
    int next_cylinder_distance = -1;
    int next_at = -1;

    for (int i=0; i<queue_cnt; i++) {
        int replace_next = FALSE;
        if (request_queue[i].cylinder <= current_cylinder) {
            if (next == -1) {
                replace_next = TRUE;
            } else {
                int current_cylinder_distance = current_cylinder - request_queue[i].cylinder;
                if (current_cylinder_distance < next_cylinder_distance) {
                    replace_next = TRUE;
                } else if (current_cylinder_distance == next_cylinder_distance) {
                    if (request_queue[i].arrival_timestamp < next_at) {
                        replace_next = TRUE;
                    }
                }
            }
            if (replace_next == TRUE) {
                next = i;
                next_cylinder_distance = current_cylinder - request_queue[i].cylinder;
                next_at = request_queue[i].arrival_timestamp;
            }
        }
    }

    return next;
}

struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX],int  *queue_cnt, int current_cylinder, int scan_direction) {
    if (*queue_cnt == 0) {
        struct RCB null_rcb = {0, 0, 0, 0, 0};
        return null_rcb;
    }

    int next = -1;
    if (scan_direction == 1) {
        next = find_next_rcb_up(request_queue, *queue_cnt, current_cylinder);
        if (next == -1) {
            next = find_next_rcb_down(request_queue, *queue_cnt, current_cylinder);
        }
    } else {
        next = find_next_rcb_down(request_queue, *queue_cnt, current_cylinder);
        if (next == -1) {
            next = find_next_rcb_up(request_queue, *queue_cnt, current_cylinder);
        }
    }

    struct RCB next_rcb = request_queue[next];
    for (int i=next; i< (*queue_cnt)-1; i++) {
        request_queue[i] = request_queue[i+1];
    }
    (*queue_cnt)--;

    return next_rcb;
}
