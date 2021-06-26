#ifndef READER_H
#define READER_H

#include <string_buffer.h>
#include <watchdog.h>

typedef struct reader_args reader_args;

reader_args* reader_args_create(string_buffer* sbuf, watch_struct* w_struct, string_buffer* log_buf);
void reader_args_destroy(reader_args* r_args);


void* reader_get_cpu_data(void* arg);

#endif
