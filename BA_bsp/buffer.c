#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "buffer.h" 
 
void initBuffer(BufferType *buffer) {
  buffer->wrPtr = 0;
  buffer->rdPtr = 0;
}
 
uint16_t bufferCount(BufferType *buffer) {
	if (buffer->wrPtr >= buffer->rdPtr)
		return buffer->wrPtr - buffer->rdPtr;
	else
		return BUFFER_SIZE - buffer->rdPtr - buffer->wrPtr;
}

inline uint8_t isBufferFull(BufferType *buffer) {
    return (((buffer->wrPtr + 1) % BUFFER_SIZE) == buffer->rdPtr);
}

inline uint8_t isBufferEmpty(BufferType *buffer) {
    return (buffer->wrPtr == buffer->rdPtr);
}

inline uint8_t bufferEnque(BufferType *buffer, uint8_t b) {
    if (!isBufferFull(buffer)) {
        buffer->data[buffer->wrPtr++] = b;
        buffer->wrPtr %= BUFFER_SIZE;
        return 1;
    }
    return 0;
}
 
inline uint8_t bufferDeque(BufferType *buffer) {
	uint8_t b;
    if(!isBufferEmpty(buffer)) {
        b = buffer->data[buffer->rdPtr++];
        buffer->rdPtr %= BUFFER_SIZE;
        return b;
    }
    return 0;
}

