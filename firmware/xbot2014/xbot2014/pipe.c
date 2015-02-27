/***********************************************************************
 *
 * pipe.c
 *
 * Interrupt-safe FIFO pipe for queing arbitrary data structures
 *
 * Copyright (c) 2014,2015 John Miller
 *
 ***********************************************************************/

#include "pipe.h"

#include <string.h>
#include <util/atomic.h>

void pipe_init(pipe_t *self, void *data, uint16_t elt_size, uint16_t num_elts)
{
    self->data = data;
    self->read_from = 0;
    self->write_to = 0;
    self->elt_size = elt_size;
    self->num_elts = num_elts;
}

void pipe_flush(pipe_t *self)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        self->read_from = self->write_to;
    }
}

uint16_t pipe_write(pipe_t *self, const void *data)
{
    uint16_t retval = 0;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (!pipe_isFull(self))
        {
            memcpy(self->data + self->write_to * self->elt_size, data, self->elt_size);
            self->write_to = (self->write_to + 1) % self->num_elts;
            retval = self->elt_size;
        }
    }
    
    return retval;
}
uint16_t pipe_read(pipe_t *self, void *data)
{
    uint16_t retval = 0;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (pipe_peek(self, data))
        {
            self->read_from = (self->read_from + 1) % self->num_elts;
            retval = self->elt_size;
        }
    }
    
    return retval;
}

uint16_t pipe_peek(pipe_t *self, void *data)
{
    uint16_t retval = 0;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (!pipe_isEmpty(self))
        {
            memcpy(data, self->data + self->read_from * self->elt_size, self->elt_size);
            retval = self->elt_size;
        }
    }
    
    return retval;
}

uint8_t pipe_isEmpty(pipe_t *self)
{
    uint8_t retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        retval = (self->write_to == self->read_from);
    }
    
    return retval;
}

uint8_t pipe_isFull(pipe_t *self)
{
    uint8_t retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        retval = (((self->write_to + 1) % self->num_elts) == self->read_from);
    }
    
    return retval;
}