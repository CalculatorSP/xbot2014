#include "pipe.h"

#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>

void _pipe_init(pipe_t *self, uint8_t elt_size, uint8_t num_elts)
{
    void *data;
    
    data = malloc(elt_size * num_elts);
    memset(data, 0x00, elt_size * num_elts);
    
    self->data = data;
    self->read_from = 0;
    self->write_to = 0;
    self->elt_size = elt_size;
    self->num_elts = num_elts;
}

void _pipe_write(pipe_t *self, const void *data)
{
    memcpy(self->data + self->write_to * self->elt_size, data, self->elt_size);
    self->write_to = (self->write_to + 1) % self->num_elts;
}

void *_pipe_read_ptr(pipe_t *self)
{
    void *data = _pipe_peek_ptr(self);
    self->read_from = (self->read_from + 1) % self->num_elts;
    
    return data;
}

void _pipe_read_data(pipe_t *self, void *data)
{
    _pipe_peek_data(self, data);
    self->read_from = (self->read_from + 1) % self->num_elts;
}

void *_pipe_peek_ptr(pipe_t *self)
{
    return self->data + self->read_from * self->elt_size;
}

void _pipe_peek_data(pipe_t *self, void *data)
{
    memcpy(data, self->data + self->read_from * self->elt_size, self->elt_size);
}

uint8_t _pipe_isEmpty(pipe_t *self)
{
    return (self->write_to == self->read_from) ? 1 : 0;
}

uint8_t _pipe_isFull(pipe_t *self)
{
    return (((self->write_to + 1) % self->num_elts) == self->read_from) ? 1 : 0;
}