#ifndef LOGGER_H
#define LOGGER_H

#include <string_buffer.h>
#include <watchdog.h>

typedef struct logger_args logger_args;

logger_args* logger_args_create(string_buffer* log_buf);
void logger_args_destroy(logger_args* l_args);

void* logger_print(void* arg);

#endif
