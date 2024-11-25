/*--------------------------------------------------------------------*/
/* chunk.c                                                            */
/* Author:  James Kyoung Ho Kim                                       */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "chunk.h"



int
chunk_get_status(Chunk_T c)
{
   return c->status;
}

void
chunk_set_status(Chunk_T c, int status)
{
   c->status = status;
}

int
chunk_get_units(Chunk_T c)
{
   return c->units;
}

void
chunk_set_units(Chunk_T c, int units)
{
   c->units = units;
}

Chunk_T
chunk_get_next_free_chunk(Chunk_T c)
{
  return c->next;
}

void
chunk_set_next_free_chunk(Chunk_T c, Chunk_T next)
{
   c->next = next;
}

Chunk_T
chunk_get_prev_free_chunk(Chunk_T c)
{
  return c->next;
}

void
chunk_set_prev_free_chunk(Chunk_T c, Chunk_T prev)
{
   c->prev = prev;
}

Chunk_T
chunk_get_next_adjacent(Chunk_T c, void* start, void* end)
{
   Chunk_T n;

   assert((void *)c >= start);

   n = c + c->units + 1;

   if ((void *)n >= end)
      return NULL;
   
   return n;
}

#ifndef NDEBUG

int 
chunk_is_valid(Chunk_T c, void *start, void *end)

{
   assert(c != NULL);
   assert(start != NULL);
   assert(end != NULL);

   if (c < (Chunk_T)start)
      {fprintf(stderr, "Bad heap start\n"); return 0; }
   if (c >= (Chunk_T)end)
      {fprintf(stderr, "Bad heap end\n"); return 0; }
   if (c->units == 0)
      {fprintf(stderr, "Zero units\n"); return 0; }
   return 1;
}
#endif