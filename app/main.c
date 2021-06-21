#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <reader.h>
#include <analyzer.h>
#include <printer.h>
#include <string_buffer.h>


int main(void) {

    string_buffer* sbuf = string_buffer_create();
    string_buffer* print_buf = string_buffer_create();
    analyzer_args* a_args = analyzer_args_create(sbuf, print_buf);

    pthread_t reader, analyzer, printer;

    pthread_create(&printer, NULL, printer_print, (void*)print_buf);
    pthread_create(&analyzer, NULL, analyzer_process_cpu_data, (void*)a_args);
    pthread_create(&reader, NULL, reader_get_cpu_data, (void*)sbuf);



    pthread_join(reader, NULL);
    pthread_join(analyzer, NULL);
    pthread_join(printer, NULL);

    /* Never reached */
    string_buffer_destroy(sbuf);
    string_buffer_destroy(print_buf);
    analyzer_args_destroy(a_args);
    return 0;
}
