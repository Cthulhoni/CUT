#include <watchdog.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>

#define WATCHDOG_WAIT_TIME 3U

struct watch_struct {
    atomic_bool running;
    atomic_bool reader_signal;
    atomic_bool analyzer_signal;
    atomic_bool printer_signal;
};

watch_struct* watch_struct_create(void) {
    watch_struct* w_struct = malloc(sizeof(*w_struct));
    if (!w_struct)
        return NULL;

    atomic_init(&w_struct->running, false);
    atomic_init(&w_struct->reader_signal, false);
    atomic_init(&w_struct->analyzer_signal, false);
    atomic_init(&w_struct->printer_signal, false);

    return w_struct;
}
void watch_struct_destroy(watch_struct* w_struct) {
    if (w_struct)
        free(w_struct);
}

bool watch_struct_check(watch_struct* w_struct) {
    if (!w_struct)
        return false;
    return atomic_load(&w_struct->reader_signal)
                  && atomic_load(&w_struct->analyzer_signal) 
                  && atomic_load(&w_struct->printer_signal);
}

void watch_struct_launch(watch_struct* w_struct) {
    if (!w_struct)
        return;
    atomic_store(&w_struct->running, true);
}

void watch_struct_stop(watch_struct* w_struct) {
    if (!w_struct)
        return;
    atomic_store(&w_struct->running, false);
}

bool watch_struct_is_running(watch_struct* w_struct) {
    if (!w_struct)
        return false;
    return atomic_load(&w_struct->running);
}

void watch_struct_reader_signal(watch_struct* w_struct) {
    if (!w_struct)
        return;
    atomic_store(&w_struct->reader_signal, true);
} 

void watch_struct_analyzer_signal(watch_struct* w_struct) {
    if (!w_struct)
        return;
    atomic_store(&w_struct->analyzer_signal, true);
} 

void watch_struct_printer_signal(watch_struct* w_struct) {
    if (!w_struct)
        return;
    atomic_store(&w_struct->printer_signal, true);
} 

struct watchdog_args {
    watch_struct* w_struct;
    size_t num_of_threads;
    pthread_t threads[];
};

watchdog_args* watchdog_args_create(watch_struct* w_struct, size_t num_of_threads, pthread_t threads[]) {
    watchdog_args* w_args = malloc(sizeof(*w_args) + num_of_threads * sizeof(*w_args->threads));
    if (!w_args)
        return NULL;
    if (!w_struct || !threads)
        return NULL;
    w_args->w_struct = w_struct;
    w_args->num_of_threads = num_of_threads;
    memcpy(w_args->threads, threads, num_of_threads * sizeof(*threads));

    return w_args;
}

void watchdog_args_destroy(watchdog_args* w_args) {
    if (w_args)
        free(w_args);
}

void* watchdog_watch(void* arg) {
    if (!arg)
        return NULL;

    watchdog_args* w_args = (watchdog_args*) arg;
    watch_struct* w_struct = w_args->w_struct;

    while(true) {

        sleep(WATCHDOG_WAIT_TIME);

        bool check = watch_struct_check(w_struct);

        if (!check) {
            atomic_store(&w_struct->running, false);

            for (size_t i = 0; i < w_args->num_of_threads; ++i) {
                if(w_args->threads[i]) {
                    pthread_cancel(w_args->threads[i]);
                    pthread_join(w_args->threads[i], NULL);
                }
            }

            perror("Watchdog closed the program\n");
            return NULL;
        }

        atomic_store(&w_struct->reader_signal, false);
        atomic_store(&w_struct->analyzer_signal, false);
        atomic_store(&w_struct->printer_signal, false);
    }

}
