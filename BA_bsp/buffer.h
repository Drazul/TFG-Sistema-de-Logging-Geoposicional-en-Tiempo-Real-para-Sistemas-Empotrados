#ifndef __BUFFER_H
#define __BUFFER_H

#include <stdint.h>

#define BUFFER_SIZE 100

/**< Tipos de buffer circular */
typedef struct
{
    uint8_t wrPtr;
    uint8_t rdPtr;
    uint8_t data[BUFFER_SIZE];
} BufferType;

void initBuffer(BufferType *buffer);
uint16_t bufferCount(BufferType *buffer);
uint8_t isBufferFull(BufferType *buffer);
uint8_t isBufferEmpty(BufferType *buffer);
uint8_t bufferEnque(BufferType *buffer, uint8_t b);
uint8_t bufferDeque(BufferType *buffer);

#endif
