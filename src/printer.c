#include <printer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string_buffer.h>

#define CLEAR_SCREEN() printf("\033c")

struct printer_args {
    string_buffer* print_buf;
    watch_struct* w_struct;
    string_buffer* log_buf;
};

printer_args* printer_args_create(string_buffer* print_buf, watch_struct* w_struct, string_buffer* log_buf) {
    printer_args* p_args = malloc(sizeof(*p_args));
    if (!p_args)
        return NULL;
    if (!print_buf || !w_struct || !log_buf)
        return NULL;
    p_args->print_buf = print_buf;
    p_args->w_struct = w_struct;
    p_args->log_buf = log_buf;
    return p_args;
}
void printer_args_destroy(printer_args* p_args) {
    if (p_args)
        free(p_args);
}

static char* temp_string = NULL;

static void cleanup() {
    if(temp_string)
        free(temp_string);

}

void* printer_print(void* arg) {
    if(!arg)
        return NULL;

    pthread_cleanup_push(cleanup, NULL)

    printer_args* p_args = (printer_args*)arg;
    
    string_buffer* print_buf = p_args->print_buf;
    watch_struct* w_struct = p_args->w_struct;
    string_buffer* log_buf = p_args->log_buf;
    (void)log_buf;

    while(watch_struct_is_running(w_struct)) {

        watch_struct_printer_signal(w_struct);

        STRING_BUFFER_GET(print_buf, temp_string);

        CLEAR_SCREEN();
        printf("%s\n", temp_string);
        free(temp_string);
        temp_string = NULL;

    }

    pthread_cleanup_pop(1);

    return NULL;
}
