#ifndef __xbot2014__pipe__
#define __xbot2014__pipe__

#include <stdint.h>

typedef struct
{
    uint8_t write_to;
    uint8_t read_from;
    uint8_t elt_size;
    uint8_t num_elts;
    void *data;
} pipe_t;

void _pipe_init(pipe_t *self, uint8_t elt_size, uint8_t num_elts);
void _pipe_write(pipe_t *self, const void *data);
void *_pipe_read_ptr(pipe_t *self);
void _pipe_read_data(pipe_t *self, void *data);
void *_pipe_peek_ptr(pipe_t *self);
void _pipe_peek_data(pipe_t *self, void *data);
uint8_t _pipe_isEmpty(pipe_t *self);
uint8_t _pipe_isFull(pipe_t *self);


#endif
