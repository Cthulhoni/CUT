#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct watch_struct watch_struct;

watch_struct* watch_struct_create(void);
void watch_struct_destroy(watch_struct* w_struct);
bool watch_struct_check(watch_struct* w_struct);
void watch_struct_clear_signals(watch_struct* w_struct);
void watch_struct_launch(watch_struct* w_struct);
void watch_struct_stop(watch_struct* w_struct);
bool watch_struct_is_running(watch_struct* w_struct);
void watch_struct_reader_signal(watch_struct* w_struct);
void watch_struct_analyzer_signal(watch_struct* w_struct);
void watch_struct_printer_signal(watch_struct* w_struct);


typedef struct watchdog_args watchdog_args;

watchdog_args* watchdog_args_create(watch_struct* w_struct, size_t num_of_threads, pthread_t threads[]);
void watchdog_args_destroy(watchdog_args* w_args);

void* watchdog_watch(void* arg);

#endif
