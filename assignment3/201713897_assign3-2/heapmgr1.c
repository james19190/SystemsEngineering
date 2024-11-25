/*--------------------------------------------------------------------*/
/* heapmgr1.c                                                         */
/* Author: James Kyoung Ho Kim                                        */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "chunk.h"

#define FALSE 0
#define TRUE  1

// Function Prototyppes
void *heapmgr_malloc(size_t size);
void heapmgr_free(void *ptr);
static void init_my_heap(void);
static Chunk_T allocate_more_memory(size_t units);
static Chunk_T merge_chunks(Chunk_T c);
static Chunk_T split_chunk(Chunk_T c, size_t units);

enum {MEMALLOC_MIN = 1024};

/* Global variables */
static Chunk_T g_free_head = NULL; 
static void *g_heap_start = NULL, *g_heap_end = NULL; 

/*--------------------------------------------------------------------*/
/* init_my_heap: 
 * Initialize data structures and global variables for chunk management. 
 */
static void init_my_heap(void) {
   g_heap_start = g_heap_end = sbrk(0);
   if (g_heap_start == (void *)-1) {
      fprintf(stderr, "sbrk(0) failed\n");
      exit(-1);
   }
}

/*--------------------------------------------------------------------*/
/* heapmgr_malloc:
 * Dynamically allocate memory capable of holding size bytes. 
 * Substitute for GNU malloc().                                 
 */
void *heapmgr_malloc(size_t size) {

    // Initialize the heap if it hasn't been done yet
    if (!g_heap_start) {
        init_my_heap();
    }

    size_t units_needed = (size + CHUNK_UNIT - 1) / CHUNK_UNIT + 1;
    Chunk_T current_chunk = g_free_head;

    // Traverse the free list
    while (current_chunk) {
        if (chunk_get_units(current_chunk) >= units_needed) {
            // Remove the found chunk from the free list
            if (current_chunk == g_free_head) {
                g_free_head = chunk_get_next_free_chunk(current_chunk);
            } else {
                Chunk_T previous_chunk = g_free_head;
                while (chunk_get_next_free_chunk(previous_chunk) != current_chunk) {
                    previous_chunk = chunk_get_next_free_chunk(previous_chunk);
                }
                chunk_set_next_free_chunk(previous_chunk, chunk_get_next_free_chunk(current_chunk));
            }
            return (void *)((char *)split_chunk(current_chunk, units_needed) + CHUNK_UNIT);
        }
        current_chunk = chunk_get_next_free_chunk(current_chunk);
    }
    current_chunk = allocate_more_memory(units_needed);
    return current_chunk ? (void *)((char *)split_chunk(current_chunk, units_needed) + CHUNK_UNIT) : NULL;
}

/*--------------------------------------------------------------------*/
/* heapmgr_free:
 * Releases dynamically allocated memory. 
 * Substitute for GNU free().                                   */
/*--------------------------------------------------------------------*/
void heapmgr_free(void *m) {
   if (!m) return;

   Chunk_T c = (Chunk_T)((char *)m - CHUNK_UNIT);
   chunk_set_status(c, CHUNK_FREE);

   c = merge_chunks(c); // Merge with adjacent free chunks if possible
   chunk_set_next_free_chunk(c, g_free_head);
   g_free_head = c; // Add to the front of the free list
}

/*--------------------------------------------------------------------*/
/* split_chunk:
 * Split 'c' into two chunks s.t. one chunk has 'units' size and
 * the other has (original size - 'units' - 1).
 * Returns the chunk with 'units' size.
 */
static Chunk_T split_chunk(Chunk_T c, size_t units) {
    if (chunk_get_units(c) > units) {
        Chunk_T new_chunk = (Chunk_T)((char *)c + (units + 1) * CHUNK_UNIT);
        size_t remaining_units = chunk_get_units(c) - units - 1;
        chunk_set_units(new_chunk, remaining_units);
        chunk_set_status(new_chunk, CHUNK_FREE);
        chunk_set_next_free_chunk(new_chunk, chunk_get_next_free_chunk(c));
        chunk_set_next_free_chunk(c, NULL);
        chunk_set_units(c, units);
    }
    chunk_set_status(c, CHUNK_IN_USE);
    return c;
}

/*--------------------------------------------------------------------*/
/* merge_chunks:
 * Merge two adjacent chunks and return the merged chunk.
 * Returns NULL on error. 
 */
static Chunk_T merge_chunks(Chunk_T c) {

    Chunk_T next = chunk_get_next_adjacent(c, g_heap_start, g_heap_end);
    
    // Check if the next chunk is valid and free
    if (next != NULL) {

        if (chunk_get_status(next) == CHUNK_FREE) {
            size_t new_size = chunk_get_units(c) + chunk_get_units(next) + 1;
            chunk_set_units(c, new_size);
            Chunk_T next_free = chunk_get_next_free_chunk(next);
            chunk_set_next_free_chunk(c, next_free);
        }
    }
    
    return c; 
}

/*--------------------------------------------------------------------*/
/* allocate_more_memory: 
 * Allocate a new chunk which is capable of holding 'units' chunk
 * units in memory by increasing the heap, and return the new
 * chunk. This function also performs chunk merging with "prev" if possible
 * after allocating a new chunk. 
 */
static Chunk_T allocate_more_memory(size_t units) {

    size_t total_size = (units + 1) * CHUNK_UNIT;
    Chunk_T new_chunk = (Chunk_T)sbrk(total_size);
    
    // Check for allocation failure
    if (new_chunk == (Chunk_T)-1) {
        return NULL; // Return NULL if sbrk failed
    }

    g_heap_end = sbrk(0);

    chunk_set_units(new_chunk, units);
    chunk_set_status(new_chunk, CHUNK_FREE);

    return new_chunk; // Return the pointer to the newly allocated chunk
}
