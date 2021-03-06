#ifndef ANALYZER_H
#define ANALYZER_H

#include <string_buffer.h>
#include <watchdog.h>

typedef struct analyzer_args analyzer_args;

analyzer_args* analyzer_args_create(string_buffer* sbuf, string_buffer* print_buf, watch_struct* w_struct, string_buffer* log_buf);
void analyzer_args_destroy(analyzer_args* a_args);


void* analyzer_process_cpu_data(void* arg);

#endif
