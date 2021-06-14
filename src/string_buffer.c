#include <string_buffer.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define STRING_BUFFER_MAX_SIZE 100

struct string_buffer {
    char* buffer[STRING_BUFFER_MAX_SIZE];
    size_t head;
    size_t tail;
    size_t size;
    pthread_mutex_t mutex;
    pthread_cond_t can_put;
    pthread_cond_t can_get;
};

string_buffer* string_buffer_create(void) {
    string_buffer* sbuf = malloc(sizeof(*sbuf));
    if (!sbuf)
        return NULL;
    
    *sbuf = (string_buffer) {
                                .buffer = {0},
                                .head = 0,
                                .tail = 0,
                                .size = 0,
                                .mutex = PTHREAD_MUTEX_INITIALIZER,
                                .can_put = PTHREAD_COND_INITIALIZER,
                                .can_get = PTHREAD_COND_INITIALIZER
                            };
    
    return sbuf;
}
void string_buffer_destroy(string_buffer* sbuf) {
    if (!sbuf)
        return;
    
    if (sbuf->size > 0) {
        if (sbuf->tail < sbuf->head) {
            for (size_t i = sbuf->tail; i < sbuf->head; ++i)
                free(sbuf->buffer[i]);
        }
        else {
            for (size_t i = sbuf->tail; i < STRING_BUFFER_MAX_SIZE; ++i) {
                free(sbuf->buffer[i]);
            }
            for (size_t i = 0; i < sbuf->head; ++i)
                free(sbuf->buffer[i]);
        }
    }

    pthread_cond_destroy(&sbuf->can_put);
    pthread_cond_destroy(&sbuf->can_get);
    pthread_mutex_destroy(&sbuf->mutex);

    free(sbuf);
}

char* string_buffer_get(string_buffer* sbuf) {
    if (!sbuf)
        return NULL;

    if (string_buffer_is_empty(sbuf))
        return NULL;

    char* res = malloc(sizeof(char) * (strlen(sbuf->buffer[sbuf->tail]) + 1));
    strcpy(res, sbuf->buffer[sbuf->tail]);
    free(sbuf->buffer[sbuf->tail]);
    sbuf->tail = (sbuf->tail + 1) % STRING_BUFFER_MAX_SIZE;
    sbuf->size = sbuf->size - 1;
    return res;
}

void string_buffer_put(string_buffer* sbuf, char* elem) {
    if (string_buffer_is_full(sbuf) || !elem || !sbuf)
        return;

    sbuf->buffer[sbuf->head] = malloc(sizeof(char) * (strlen(elem) + 1));
    strcpy(sbuf->buffer[sbuf->head], elem);
    sbuf->head = (sbuf->head + 1) % STRING_BUFFER_MAX_SIZE;
    sbuf->size = sbuf->size + 1;
    
}

bool string_buffer_is_full(const string_buffer* sbuf) {
    if (!sbuf)
        return NULL;
    return sbuf->size == STRING_BUFFER_MAX_SIZE;
}

bool string_buffer_is_empty(const string_buffer* sbuf) {
    if (!sbuf)
        return NULL;
    return sbuf->size == 0;
}

void string_buffer_lock(string_buffer* sbuf) {
    if (!sbuf)
        return;
    pthread_mutex_lock(&sbuf->mutex);
}

void string_buffer_unlock(string_buffer* sbuf) {
    if (!sbuf)
        return;
    pthread_mutex_unlock(&sbuf->mutex);
}

void string_buffer_call_get(string_buffer* sbuf) {
    if (!sbuf)
        return;
    pthread_cond_signal(&sbuf->can_put);
}

void string_buffer_wait_get(string_buffer* sbuf) {
    if (!sbuf)
        return;
    pthread_cond_wait(&sbuf->can_get, &sbuf->mutex);
}

void string_buffer_call_put(string_buffer* sbuf) {
    if (!sbuf)
        return;
    pthread_cond_signal(&sbuf->can_get);
}

void string_buffer_wait_put(string_buffer* sbuf) {
    if (!sbuf)
        return;
    pthread_cond_wait(&sbuf->can_put, &sbuf->mutex);
}
