#include "ringBuffer/ringBuffer.h"

void ring_buffer_init(ring_buffer_t *rb, uint8_t *buffer, uint16_t capacity)
{
    if(!VALID_RING_BUFFER(rb))
        return ;

    rb->buffer = buffer;
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
}

bool ring_buffer_is_empty(ring_buffer_t *rb)
{
    if(!VALID_RING_BUFFER(rb))
        return true;

    return rb->head == rb->tail;
}

bool ring_buffer_is_full(ring_buffer_t *rb)
{
    if(!VALID_RING_BUFFER(rb))
        return false;

    return ((rb->head + 1) % rb->capacity) == rb->tail;
}

bool ring_buffer_write(ring_buffer_t *rb, uint8_t data)
{
    if(!VALID_RING_BUFFER(rb))
        return false;

    if(ring_buffer_is_full(rb))
        rb->tail = (rb->tail + 1) % rb->capacity;
    rb->buffer[rb->head] = data;

    uint16_t next_head = (rb->head + 1) % rb->capacity;
    rb->head = next_head;
    return true;
}

bool ring_buffer_read(ring_buffer_t *rb, uint8_t *data)
{
    if(!VALID_RING_BUFFER(rb) || ring_buffer_is_empty(rb))
        return false;

    *data = rb->buffer[rb->tail];

    uint16_t next_tail = (rb->tail + 1) % rb->capacity;
    rb->tail = next_tail;
    return true;
}

uint16_t ring_buffer_count(ring_buffer_t *rb)
{
    if(!VALID_RING_BUFFER(rb))
        return 0;

    return (rb->head - rb->tail + rb->capacity) % rb->capacity;
}

void ring_buffer_flush(ring_buffer_t *rb)
{
    if(!VALID_RING_BUFFER(rb))
        return ;

    rb->head = 0;
    rb->tail = 0;
}
