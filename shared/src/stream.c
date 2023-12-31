#include "stream.h"

static void __stream_send(int toSocket, void* streamToSend, uint32_t bufferSize) {
    uint8_t header = 0;
    uint32_t size = 0;
    ssize_t bytesSent = send(toSocket, streamToSend, sizeof(header) + sizeof(size) + bufferSize, 0);
    if (bytesSent == -1) {
        printf("\e[0;31m__stream_send: Error en el envío del buffer [%s]\e[0m\n", strerror(errno));
    }
}

static void* __stream_create(uint8_t header, t_buffer* buffer) {
    void* streamToSend = malloc(sizeof(header) + sizeof(buffer->size) + buffer->size);
    int offset = 0;
    memcpy(streamToSend + offset, &header, sizeof(header));
    offset += sizeof(header);
    memcpy(streamToSend + offset, &(buffer->size), sizeof(buffer->size));
    offset += sizeof(buffer->size);
    memcpy(streamToSend + offset, buffer->stream, buffer->size);
    return streamToSend;
}

void stream_send_buffer(int toSocket, uint8_t header, t_buffer* buffer) {
    void* stream = __stream_create(header, buffer);
    __stream_send(toSocket, stream, buffer->size);
    free(stream);
}

void stream_send_empty_buffer(int toSocket, uint8_t header) {
    t_buffer* emptyBuffer = buffer_create();
    stream_send_buffer(toSocket, header, emptyBuffer);
    buffer_destroy(emptyBuffer);
}

void stream_recv_buffer(int fromSocket, t_buffer* destBuffer) {
    ssize_t msgBytes = recv(fromSocket, &(destBuffer->size), sizeof(destBuffer->size), 0);
    if (msgBytes == -1) {
        printf("\e[0;31mstream_recv_buffer: Error en la recepción del buffer [%s]\e[0m\n", strerror(errno));
    } else if (destBuffer->size > 0) {
        destBuffer->stream = malloc(destBuffer->size);
        recv(fromSocket, destBuffer->stream, destBuffer->size, 0);
    }
}

void stream_recv_empty_buffer(int fromSocket) {
    t_buffer* buffer = buffer_create();
    stream_recv_buffer(fromSocket, buffer);
    buffer_destroy(buffer);
}

uint8_t stream_recv_header(int fromSocket) {
    uint8_t header;
    ssize_t msgBytes = recv(fromSocket, &header, sizeof(header), 0);
    if (msgBytes == -1) {
        printf("\e[0;31mstream_recv_buffer: Error en la recepción del header [%s]\e[0m\n", strerror(errno));
    }
    return header;
}