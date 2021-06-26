#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <reader.h>
#include <analyzer.h>
#include <printer.h>
#include <string_buffer.h>

#define NUM_OF_THREADS 3U

static pthread_t threads[NUM_OF_THREADS];
static pthread_t watchdog;

static watch_struct* w_struct = NULL;
static string_buffer* sbuf = NULL;
static string_buffer* print_buf = NULL;
static reader_args* r_args = NULL;
static analyzer_args* a_args = NULL;
static printer_args* p_args = NULL;
static watchdog_args* w_args = NULL;

static void sigterm_handler(int sig) {

    (void)sig;

    if (w_struct) {
            watch_struct_stop(w_struct);
    }
}

static void cleanup(void) {
    string_buffer_destroy(sbuf);
    string_buffer_destroy(print_buf);
    printer_args_destroy(p_args);
    analyzer_args_destroy(a_args);
    reader_args_destroy(r_args);
    watch_struct_destroy(w_struct);
    watchdog_args_destroy(w_args);
}

int main(void) {

    atexit(cleanup);

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigterm_handler;
    sigaction(SIGTERM, &action, NULL);
    
    w_struct = watch_struct_create();
    watch_struct_launch(w_struct);

    sbuf = string_buffer_create();
    print_buf = string_buffer_create();
    if (!sbuf || !print_buf)
        return -1;
    r_args = reader_args_create(sbuf, w_struct);
    a_args = analyzer_args_create(sbuf, print_buf, w_struct);
    p_args = printer_args_create(print_buf, w_struct);
    if (!r_args || !a_args || !p_args)
        return -1;

    pthread_t reader = 0;
    pthread_t analyzer = 0;
    pthread_t printer = 0;

    pthread_create(&printer, NULL, printer_print, (void*)p_args);
    pthread_create(&analyzer, NULL, analyzer_process_cpu_data, (void*)a_args);
    pthread_create(&reader, NULL, reader_get_cpu_data, (void*)r_args);

    threads[0] = printer;
    threads[1] = analyzer;
    threads[2] = reader;

    w_args = watchdog_args_create(w_struct, NUM_OF_THREADS, threads);

    if (!w_args)
        return -1;

    pthread_create(&watchdog, NULL, watchdog_watch, (void*)w_args);


    pthread_join(reader, NULL);
    pthread_join(analyzer, NULL);
    pthread_join(printer, NULL);

    pthread_cancel(watchdog);
    pthread_join(watchdog, NULL);
    
    return 0;
}
