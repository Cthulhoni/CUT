#ifndef PRINTER_H
#define PRINTER_H

#include <string_buffer.h>
#include <watchdog.h>

typedef struct printer_args printer_args;

printer_args* printer_args_create(string_buffer* print_buf, watch_struct* w_struct);
void printer_args_destroy(printer_args* p_args);


void* printer_print(void* arg);

#endif
