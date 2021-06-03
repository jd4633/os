#include <stdio.h>
#include "oslabs.h"

void print_pages_in_memory(struct PTE page_table[TABLEMAX],int table_cnt) {
    printf("Pages in memory: ");
    for (int i=0; i<table_cnt; i++) {
        if (page_table[i].is_valid) {
            printf ("%d %d,", i, page_table[i].last_access_timestamp);
        }
    }
    printf("\n");
}

int process_page_access_fifo(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, 
                             int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {

    int frame = -1;
    int page_to_replace = -1;                            

    // If page is already in memory, update the page table and return the frame number
    if (page_table[page_number].is_valid) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

    // Otherwise, if there is a free frame, update the page table and return the frame number
    if ((*frame_cnt) > 0) {
        frame = frame_pool[(*frame_cnt)-1];
        (*frame_cnt)--;
        page_table[page_number].frame_number = frame;
        page_table[page_number].is_valid = 1;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return page_table[page_number].frame_number;
    }

    // Otherwise, start by identifying the first page to arrive in memory
    for (int i=0; i< *table_cnt; i++) {
        if (page_table[i].is_valid) {
            if (page_to_replace < 0) {
                page_to_replace = i;
            } else {
                if (page_table[i].arrival_timestamp < page_table[page_to_replace].arrival_timestamp) {
                    page_to_replace = i;
                }
            }
        }
    }

    // Now replace that page's frame with the new page
    frame = page_table[page_to_replace].frame_number;
    page_table[page_to_replace].frame_number = 0;
    page_table[page_to_replace].is_valid = 0;
    page_table[page_to_replace].arrival_timestamp = 0;
    page_table[page_to_replace].last_access_timestamp = 0;
    page_table[page_to_replace].reference_count = 0;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].is_valid = 1;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame;
}

int count_page_faults_fifo(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {
    int page_faults = 0;
    for (int t=0; t<reference_cnt; t++) {
        int page = refrence_string[t];
        //printf("Request for page: %d\n", page);
        
        if (!page_table[page].is_valid) {
            //printf("Page fault, loading page to memory\n");
            process_page_access_fifo(page_table, &table_cnt, page, frame_pool, &frame_cnt, t);
            page_faults++;
        } else {
            page_table[page].last_access_timestamp = t;
            page_table[page].reference_count++;            
            //printf("Page in memory, no page fault\n");
        }
    }

    return page_faults;
}

int process_page_access_lru(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {
    int frame = -1;
    int page_to_replace = -1;                            

    // If page is already in memory, update the page table and return the frame number
    if (page_table[page_number].is_valid) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

    // Otherwise, if there is a free frame, update the page table and return the frame number
    if ((*frame_cnt) > 0) {
        frame = frame_pool[(*frame_cnt)-1];
        (*frame_cnt)--;
        page_table[page_number].frame_number = frame;
        page_table[page_number].is_valid = 1;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return page_table[page_number].frame_number;
    }

    // Otherwise, start by identifying the least recently used page
    for (int i=0; i< *table_cnt; i++) {
        if (page_table[i].is_valid) {
            if (page_to_replace < 0) {
                page_to_replace = i;
            } else {
                if (page_table[i].last_access_timestamp < page_table[page_to_replace].last_access_timestamp) {
                    page_to_replace = i;
                }
            }
        }
    }

    // Now replace that page's frame with the new page
    frame = page_table[page_to_replace].frame_number;
    page_table[page_to_replace].frame_number = 0;
    page_table[page_to_replace].is_valid = 0;
    page_table[page_to_replace].arrival_timestamp = 0;
    page_table[page_to_replace].last_access_timestamp = 0;
    page_table[page_to_replace].reference_count = 0;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].is_valid = 1;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame;
}

int count_page_faults_lru(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {
    int page_faults = 0;
    for (int t=0; t<reference_cnt; t++) {
        int page = refrence_string[t];
        //print_pages_in_memory(page_table, table_cnt);
        //printf("Request for page: %d\n", page);
        
        if (!page_table[page].is_valid) {
            //printf("Page fault, loading page to memory\n");
            process_page_access_lru(page_table, &table_cnt, page, frame_pool, &frame_cnt, t);
            page_faults++;
        } else {
            page_table[page].last_access_timestamp = t;
            page_table[page].reference_count++;               
            //printf("Page in memory, no page fault\n");
        }
    }

    return page_faults;
}

int process_page_access_lfu(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {
    int frame = -1;
    int page_to_replace = -1;                            

    // If page is already in memory, update the page table and return the frame number
    if (page_table[page_number].is_valid) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = page_table[page_number].reference_count + 1;
        return page_table[page_number].frame_number;
    }

    // Otherwise, if there is a free frame, update the page table and return the frame number
    if ((*frame_cnt) > 0) {
        frame = frame_pool[(*frame_cnt)-1];
        (*frame_cnt)--;
        page_table[page_number].frame_number = frame;
        page_table[page_number].is_valid = 1;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return page_table[page_number].frame_number;
    }

    // Otherwise, start by identifying the least frequently used page
    for (int i=0; i< *table_cnt; i++) {
        if (page_table[i].is_valid) {
            if (page_to_replace < 0) {
                page_to_replace = i;
            } else {
                if (page_table[i].reference_count < page_table[page_to_replace].reference_count) {
                    page_to_replace = i;
                } else if (page_table[i].reference_count == page_table[page_to_replace].reference_count &&
                           page_table[i].arrival_timestamp < page_table[page_to_replace].arrival_timestamp){
                    page_to_replace = i;
                }
            }
        }
    }

    // Now replace that page's frame with the new page
    frame = page_table[page_to_replace].frame_number;
    page_table[page_to_replace].frame_number = 0;
    page_table[page_to_replace].is_valid = 0;
    page_table[page_to_replace].arrival_timestamp = 0;
    page_table[page_to_replace].last_access_timestamp = 0;
    page_table[page_to_replace].reference_count = 0;
    page_table[page_number].frame_number = frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].is_valid = 1;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame;
}

int count_page_faults_lfu(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {
    int page_faults = 0;
    for (int t=0; t<reference_cnt; t++) {
        int page = refrence_string[t];
        //printf("Request for page: %d\n", page);
        
        if (!page_table[page].is_valid) {
            //printf("Page fault, loading page to memory\n");
            process_page_access_lfu(page_table, &table_cnt, page, frame_pool, &frame_cnt, t);
            page_faults++;
        } else {
            page_table[page].last_access_timestamp = t;
            page_table[page].reference_count++;             
            //printf("Page in memory, no page fault\n");
        }
    }

    return page_faults;
}