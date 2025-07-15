#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define VALID_RING_BUFFER(rb) ((rb) != NULL && (rb)->buffer != NULL)

typedef struct {
    uint8_t *buffer;
    uint16_t head;
    uint16_t tail;
    uint16_t capacity;
} ring_buffer_t;

/**
 * @brief Initializes a ring buffer.
 *
 * @param rb Pointer to the ring_buffer_t structure to initialize.
 * @param buffer Pointer to the underlying uint8_t array to use as the buffer storage.
 * @param capacity The maximum number of elements the buffer can hold.
 */
void ring_buffer_init(ring_buffer_t *rb, uint8_t *buffer, uint16_t capacity);

/**
 * @brief Writes a single byte of data into the ring buffer.
 *
 * @param rb Pointer to the ring_buffer_t structure to initialize.
 * @param data The byte of data to write.
 *
 * @return true if the data was successfully written, false otherwise.
 */
bool ring_buffer_write(ring_buffer_t *rb, uint8_t data);

/**
 * @brief Reads a single byte of data from the ring buffer.
 *
 * @param rb Pointer to the ring_buffer_t structure.
 * @param data Pointer to a uint8_t variable where the read data will be stored.
 *
 * @return true if data was successfully read (buffer was not empty), false otherwise.
 */
bool ring_buffer_read(ring_buffer_t *rb, uint8_t *data);

/**
 * @brief Returns the number of elements currently stored in the ring buffer.
 *
 * @param rb Pointer to the ring_buffer_t structure.
 *
 * @return The number of elements in the buffer.
 */
uint16_t ring_buffer_count(ring_buffer_t *rb);

/**
 * @brief Checks if the ring buffer is empty.
 *
 * @param rb Pointer to the ring_buffer_t structure.
 *
 * @return true if the buffer is empty, false otherwise.
 */
bool ring_buffer_is_empty(ring_buffer_t *rb);

/**
 * @brief Checks if the ring buffer is full.
 *
 * @param rb Pointer to the ring_buffer_t structure.
 *
 * @return true if the buffer is full, false otherwise.
 */
bool ring_buffer_is_full(ring_buffer_t *rb);

/**
 * @brief Flushes (empties) the ring buffer by resetting head and tail pointers.
 *
 * @param rb Pointer to the ring_buffer_t structure.
 */
void ring_buffer_flush(ring_buffer_t *rb);

#endif // RING_BUFFER_H

