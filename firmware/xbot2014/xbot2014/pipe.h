#ifndef __xbot2014__pipe__
#define __xbot2014__pipe__

#include <stdint.h>

typedef struct
{
    int16_t write_to;
    int16_t read_from;
    int16_t elt_size;
    int16_t total_size;
    uint8_t *data;
} pipe_t;

void pipe_init(pipe_t *self, uint8_t *data, int16_t elt_size, int16_t num_elts); // Set up pipe
void pipe_flush(pipe_t *self);                         // Clear data in pipe
int16_t pipe_write(pipe_t *self, const uint8_t *data); // Returns num bytes written, or 0 if full
int16_t pipe_read(pipe_t *self, uint8_t *data);        // Returns num bytes read, or 0 if empty
int16_t pipe_peek(const pipe_t *self, uint8_t *data);  // Gets data without removing it from pipe
uint8_t pipe_isEmpty(const pipe_t *self);              // Checks if pipe contains data
uint8_t pipe_isFull(const pipe_t *self);               // Checks if pipe has room to write


#endif
