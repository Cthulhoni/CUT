#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stdbool.h>

typedef struct string_buffer string_buffer;

string_buffer* string_buffer_create(void);
void string_buffer_destroy(string_buffer* sbuf);
char* string_buffer_get(string_buffer* sbuf);
void string_buffer_put(string_buffer* sbuf, char* elem);
bool string_buffer_is_full(const string_buffer* sbuf);
bool string_buffer_is_empty(const string_buffer* sbuf);
void string_buffer_lock(string_buffer* sbuf);
void string_buffer_unlock(string_buffer* sbuf);
void string_buffer_call_get(string_buffer* sbuf);
void string_buffer_wait_get(string_buffer* sbuf);
void string_buffer_call_put(string_buffer* sbuf);
void string_buffer_wait_put(string_buffer* sbuf);


#endif
