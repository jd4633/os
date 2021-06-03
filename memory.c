#include "oslabs.h"

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id) {
    // Find best block
    int block = -1;
    for (int i=0; i< *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            if ((block == -1) || (memory_map[i].segment_size < memory_map[block].segment_size)) {
                block = i;
            }
        }
    }

    // If there's no fit, then return the null block
    if (block == -1) {
        struct MEMORY_BLOCK null_block = {0,0,0,0};
        return null_block;
    }

    // Split the block if needed
    if (memory_map[block].segment_size > request_size) {
        (*map_cnt)++;
        for (int i=block+2; i< *map_cnt; i++) {
            memory_map[i] = memory_map[i-1];
        };
        struct MEMORY_BLOCK free_block = {
            memory_map[block].start_address + request_size,
            memory_map[block].end_address,
            memory_map[block].segment_size - request_size,
            0};
        memory_map[block+1] = free_block;
        memory_map[block].end_address = memory_map[block].start_address + request_size - 1;
        memory_map[block].segment_size = request_size;
    }

    // Allocate the block to the process
    memory_map[block].process_id = process_id;

    return memory_map[block];
}

struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id) {
    // Find first block
    int block = -1;
    for (int i=0; i< *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            block = i;
            break;
        }
    }

    // If there's no fit, then return the null block
    if (block == -1) {
        struct MEMORY_BLOCK null_block = {0,0,0,0};
        return null_block;
    }

    // Split the block if needed
    if (memory_map[block].segment_size > request_size) {
        (*map_cnt)++;
        for (int i=block+2; i< *map_cnt; i++) {
            memory_map[i] = memory_map[i-1];
        };
        struct MEMORY_BLOCK free_block = {
            memory_map[block].start_address + request_size,
            memory_map[block].end_address,
            memory_map[block].segment_size - request_size,
            0};
        memory_map[block+1] = free_block;
        memory_map[block].end_address = memory_map[block].start_address + request_size - 1;
        memory_map[block].segment_size = request_size;
    }

    // Allocate the block to the process
    memory_map[block].process_id = process_id;

    return memory_map[block];
}

struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id) {
    // Find best block
    int block = -1;
    for (int i=0; i< *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            if ((block == -1) || (memory_map[i].segment_size > memory_map[block].segment_size)) {
                block = i;
            }
        }
    }

    // If there's no fit, then return the null block
    if (block == -1) {
        struct MEMORY_BLOCK null_block = {0,0,0,0};
        return null_block;
    }

    // Split the block if needed
    if (memory_map[block].segment_size > request_size) {
        (*map_cnt)++;
        for (int i=block+2; i< *map_cnt; i++) {
            memory_map[i] = memory_map[i-1];
        };
        struct MEMORY_BLOCK free_block = {
            memory_map[block].start_address + request_size,
            memory_map[block].end_address,
            memory_map[block].segment_size - request_size,
            0};
        memory_map[block+1] = free_block;
        memory_map[block].end_address = memory_map[block].start_address + request_size - 1;
        memory_map[block].segment_size = request_size;
    }

    // Allocate the block to the process
    memory_map[block].process_id = process_id;

    return memory_map[block];
}

struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK   memory_map[MAPMAX],int *map_cnt, int process_id, int last_address) {
    // Find first block greater than the last address
    int block = -1;
    for (int i=0; i< *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].start_address >= last_address && memory_map[i].segment_size >= request_size) {
            block = i;
            break;
        }
    }

    // If no block is found, find the free block with the lowest address
    if (block = -1) {
        for (int i=0; i< *map_cnt; i++) {
            if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
                block = i;
                break;
            }
        }
    }

    // If there's still no fit, then return the null block
    if (block == -1) {
        struct MEMORY_BLOCK null_block = {0,0,0,0};
        return null_block;
    }

    // Split the block if needed
    if (memory_map[block].segment_size > request_size) {
        (*map_cnt)++;
        for (int i=block+2; i< *map_cnt; i++) {
            memory_map[i] = memory_map[i-1];
        };
        struct MEMORY_BLOCK free_block = {
            memory_map[block].start_address + request_size,
            memory_map[block].end_address,
            memory_map[block].segment_size - request_size,
            0};
        memory_map[block+1] = free_block;
        memory_map[block].end_address = memory_map[block].start_address + request_size - 1;
        memory_map[block].segment_size = request_size;
    }

    // Allocate the block to the process
    memory_map[block].process_id = process_id;

    return memory_map[block];
}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt) {
    int block=-1;
    for (int i=0; i< *map_cnt; i++) {
        if (memory_map[i].start_address == freed_block.start_address) {
            block = i;
            break;
        }
    }

    memory_map[block].process_id = 0;

    if (block >= 1 && memory_map[block-1].process_id == 0) {
        block--;
        memory_map[block].end_address = memory_map[block+1].end_address;
        memory_map[block].segment_size = memory_map[block].segment_size + memory_map[block+1].segment_size;

        (*map_cnt)--;
        for (int i=block+1; i< *map_cnt; i++) {
            memory_map[i] = memory_map[i+1];
        }
    }

    if (block < *map_cnt && memory_map[block+1].process_id == 0) {
        memory_map[block].end_address = memory_map[block+1].end_address;
        memory_map[block].segment_size = memory_map[block].segment_size + memory_map[block+1].segment_size;

        (*map_cnt)--;
        for (int i=block+1; i< *map_cnt; i++) {
            memory_map[i] = memory_map[i+1];
        }
    }
}