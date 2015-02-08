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

void pipe_init(pipe_t *self, void *data, uint8_t elt_size, uint8_t num_elts);   // Set up pipe
void pipe_flush(pipe_t *self);                                                  // Clear data in pipe
uint8_t pipe_write(pipe_t *self, const void *data); // Returns num bytes written, or 0 if full
uint8_t pipe_read(pipe_t *self, void *data);        // Returns num bytes read, or 0 if empty
uint8_t pipe_peek(pipe_t *self, void *data);        // Gets data without removing it from pipe
uint8_t pipe_isEmpty(pipe_t *self);                 // Checks if pipe contains data
uint8_t pipe_isFull(pipe_t *self);                  // Checks if pipe has room to write


#endif
