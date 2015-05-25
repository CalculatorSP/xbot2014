/***********************************************************************
 *
 * pipe.c
 *
 * Interrupt-safe FIFO pipe for queuing arbitrary data structures
 *
 * Copyright (c) 2014,2015 John Miller
 *
 ***********************************************************************/

#include "pipe.h"

#include <string.h>
#include <util/atomic.h>

void pipe_init(pipe_t *self, uint8_t *data, int16_t elt_size, int16_t num_elts)
{
    self->data = data;
    self->read_from = 0;
    self->write_to = 0;
    self->elt_size = elt_size;
    self->total_size = num_elts * elt_size;
}

void pipe_flush(pipe_t *self)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        self->read_from = self->write_to;
    }
}

int16_t pipe_write(pipe_t *self, const uint8_t *data)
{
    int16_t retval = 0;
    int16_t tmp_write_to;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        tmp_write_to = self->write_to + self->elt_size;
        if (tmp_write_to >= self->total_size)
            tmp_write_to = 0;
        
        if (tmp_write_to != self->read_from)
        {
            memcpy(self->data + self->write_to, data, self->elt_size);
            self->write_to = tmp_write_to;
            retval = self->elt_size;
        }
    }
    
    return retval;
}

int16_t pipe_read(pipe_t *self, uint8_t *data)
{
    int16_t retval = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (self->write_to != self->read_from)
        {
            memcpy(data, self->data + self->read_from, self->elt_size);
            self->read_from += self->elt_size;
            if (self->read_from >= self->total_size)
                self->read_from = 0;
            
            retval = self->elt_size;
        }
    }
    
    return retval;
}

int16_t pipe_peek(const pipe_t *self, uint8_t *data)
{
    int16_t retval = 0;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (self->write_to != self->read_from)
        {
            memcpy(data, self->data + self->read_from, self->elt_size);
            retval = self->elt_size;
        }
    }
    
    return retval;
}

uint8_t pipe_isEmpty(const pipe_t *self)
{
    uint8_t retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        retval = (self->write_to == self->read_from);
    }
    
    return retval;
}

uint8_t pipe_isFull(const pipe_t *self)
{
    uint8_t retval;
    int16_t tmp_write_to;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        tmp_write_to = self->write_to + self->elt_size;
        if (tmp_write_to >= self->total_size)
            tmp_write_to = 0;
        
        retval = (tmp_write_to == self->read_from);
    }
    
    return retval;
}