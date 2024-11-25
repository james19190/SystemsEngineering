/*--------------------------------------------------------------------*/
/* heapmgr2.c                                                         */
/* Author: James Kyoung Ho Kim                                        */
/*--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include "chunk.h"

#define number_Bins 10

static Chunk_T bins[number_Bins];
static void *g_heap_start = NULL, *g_heap_end = NULL;

/*--------------------------------------------------------------------*/
/* Initialize the heap and the bins for managing free chunks */
static void init_heap() {
    g_heap_start = sbrk(0);  // Obtain the current position of the heap
    g_heap_end = g_heap_start; // Assign the starting position to g_heap_end

    if (g_heap_start == (void *)-1) {
        fprintf(stderr, "Error: Unable to initialize heap with sbrk\n");
        exit(-1); // Exit with an error if sbrk fails
    }

    // Set all bin pointers to NULL, preparing for future allocations
    for (int index = 0; index < number_Bins; index++) {
        bins[index] = NULL; // Initialize each bin in the array to NULL
    }
}

/*--------------------------------------------------------------------*/
/* Determine the bin index for the given size */
static int get_bin_pos(int units) {
    if (units <= 0) {
        return 0; // Handle case where units is 0 or negative
    }

    int curr = 0;
    while (units > 1) {
        units >>= 1;
        curr++;
    }
    
    return (curr >= number_Bins) ? (number_Bins - 1) : curr; // Ensure idx does not exceed bounds
}

/*--------------------------------------------------------------------*/
/* Add a chunk to the appropriate bin */
static void add_to_bin(Chunk_T c) {
    int idx = get_bin_pos(chunk_get_units(c));
    Chunk_T temp = bins[idx]; // Temporarily store the current head of the bin
    chunk_set_next_free_chunk(c, temp); // Set the next free chunk for the new chunk
    bins[idx] = c; // Update the bin to point to the new chunk
}

/*--------------------------------------------------------------------*/
/* Increase the heap size to allocate a new chunk of memory */
static Chunk_T allocate_memory(int units) {
    size_t total_bytes_needed = (units + 1) * CHUNK_UNIT; 
    Chunk_T chunk_pointer = (Chunk_T)sbrk(total_bytes_needed); 
    
    if (chunk_pointer == (Chunk_T)-1) {
        return NULL; // Return NULL if sbrk fails to allocate memory
    }

    g_heap_end = sbrk(0); 
    chunk_set_units(chunk_pointer, units); 
    chunk_set_status(chunk_pointer, CHUNK_FREE); 

    return chunk_pointer; // Return the pointer to the newly allocated chunk
}

/*--------------------------------------------------------------------*/
/* Adjust the current chunk after merging with the next chunk */
static void adjust_chunk_after_merge(Chunk_T current_chunk, Chunk_T next_chunk) {
    int combined_units = chunk_get_units(current_chunk) + chunk_get_units(next_chunk) + 1;
    chunk_set_units(current_chunk, combined_units);
    chunk_set_next_free_chunk(current_chunk, chunk_get_next_free_chunk(next_chunk));
    
    Chunk_T subsequent_free = chunk_get_next_free_chunk(next_chunk);
    if (subsequent_free != NULL) {
        chunk_set_prev_free_chunk(subsequent_free, current_chunk);
    }
}

/*--------------------------------------------------------------------*/
/* Join two adjacent free chunks into a single larger chunk */
static Chunk_T merge_chunks(Chunk_T current_chunk) {
    Chunk_T next_chunk = chunk_get_next_adjacent(current_chunk, g_heap_start, g_heap_end);

    if (next_chunk != NULL && chunk_get_status(next_chunk) == CHUNK_FREE) {
        adjust_chunk_after_merge(current_chunk, next_chunk);
    }
    
    return current_chunk; // Return the merged chunk
}

/*--------------------------------------------------------------------*/
/* Divide a chunk into two if it exceeds the requested size */
static Chunk_T split_chunk(Chunk_T c, int units) {
    int current_units = chunk_get_units(c);

    if (current_units <= units) {
        chunk_set_status(c, CHUNK_IN_USE);
        return c; // No splitting needed
    }

    Chunk_T new_chunk = (Chunk_T)((char *)c + (units + 1) * CHUNK_UNIT);
    size_t leftover_units = current_units - units - 1;

    chunk_set_units(new_chunk, leftover_units);
    chunk_set_status(new_chunk, CHUNK_FREE);
    add_to_bin(new_chunk); // Add the new chunk to the free list

    chunk_set_units(c, units); // Update the original chunk's size
    chunk_set_status(c, CHUNK_IN_USE); // Mark it as in use

    return c; // Return the updated chunk
}

/*--------------------------------------------------------------------*/
/* Find and remove a suitable chunk from the free list */
static Chunk_T find_and_remove_chunk(int required_units, int bin_index) {
    Chunk_T current_chunk = bins[bin_index];
    
    while (current_chunk != NULL) {
        if (chunk_get_units(current_chunk) >= required_units) {
            // Detach the found chunk from the free list
            if (current_chunk == bins[bin_index]) {
                bins[bin_index] = chunk_get_next_free_chunk(current_chunk);
            } else {
                Chunk_T prev_chunk = bins[bin_index];
                while (chunk_get_next_free_chunk(prev_chunk) != current_chunk) {
                    prev_chunk = chunk_get_next_free_chunk(prev_chunk);
                }
                chunk_set_next_free_chunk(prev_chunk, chunk_get_next_free_chunk(current_chunk));
            }
            return current_chunk; // Return the found chunk
        }
        current_chunk = chunk_get_next_free_chunk(current_chunk);
    }
    return NULL; // Return NULL if no suitable chunk is found
}

/*--------------------------------------------------------------------*/
/* Dynamically allocate memory based on the requested size */
void *heapmgr_malloc(int size_request) {
    if (g_heap_start == NULL) {
        init_heap(); // Ensure the heap has been initialized
    }

    int required_units = (size_request + CHUNK_UNIT - 1) / CHUNK_UNIT + 1; // Determine the number of units needed
    int bin_index = get_bin_pos(required_units); // Get the corresponding bin index

    Chunk_T current_chunk = find_and_remove_chunk(required_units, bin_index); // Find and remove a suitable chunk

    if (current_chunk != NULL) {
        return (void *)((char *)split_chunk(current_chunk, required_units) + CHUNK_UNIT);
    }

    // If no appropriate chunk is available, expand the heap
    Chunk_T new_chunk = allocate_memory(required_units);
    if (new_chunk != NULL) {
        return (void *)((char *)split_chunk(new_chunk, required_units) + CHUNK_UNIT);
    }

    return NULL; // Return NULL if allocation fails
}

/*--------------------------------------------------------------------*/
/* Free the allocated memory and return it to the pool */
void heapmgr_free(void *curr) {
    if (curr == NULL) {
        return;
    }

    Chunk_T chunk = (Chunk_T)((char *)curr - CHUNK_UNIT);
    chunk_set_status(chunk, CHUNK_FREE);
    chunk = merge_chunks(chunk);
    add_to_bin(chunk);
}
