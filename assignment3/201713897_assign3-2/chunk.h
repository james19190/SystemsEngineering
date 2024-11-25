/*--------------------------------------------------------------------*/
/* chunk.h                                                            */
/* Author:  James Kyoung Ho Kim                                       */
/*--------------------------------------------------------------------*/

#ifndef _CHUNK_BASE_H_
#define _CHUNK_BASE_H_

#include <stdbool.h>
#include <unistd.h>
#include <stddef.h>

typedef struct Chunk {
   struct Chunk *next;       
   struct Chunk *prev;       
   int units;          
   int status;         
} *Chunk_T;

enum {
   CHUNK_FREE,
   CHUNK_IN_USE,
};

#define CHUNK_UNIT sizeof(struct Chunk)


int 
chunk_get_status(Chunk_T c);

void 
chunk_set_status(Chunk_T c, int status);

int
chunk_get_units(Chunk_T c);

void
chunk_set_units(Chunk_T c, int units);

Chunk_T
chunk_get_next_free_chunk(Chunk_T c);

void
chunk_set_next_free_chunk(Chunk_T c, Chunk_T next);

Chunk_T
chunk_get_prev_free_chunk(Chunk_T c);

void
chunk_set_prev_free_chunk(Chunk_T c, Chunk_T prev);

Chunk_T
chunk_get_next_adjacent(Chunk_T c, void *start, void *end);
 
#ifndef NDEBUG

int 
chunk_is_valid(Chunk_T c, void *start, void *end);

#endif /* NDEBUG */

#endif /* _CHUNK_BASE_H_ */