#include <logger.h>
#include <stdio.h>

struct logger_args {
    string_buffer* log_buf;
};

logger_args* logger_args_create(string_buffer* log_buf) {
    logger_args* l_args = malloc(sizeof(*l_args));
    if (!l_args)
        return NULL;
    if (!log_buf)
        return NULL;
    l_args->log_buf = log_buf;
    return l_args;
}
void logger_args_destroy(logger_args* l_args) {
    if (l_args)
        free(l_args);
}
static FILE* log_file = NULL;
static char* temp_string = NULL;

static void cleanup() {
    if (log_file)
        fclose(log_file);
    if(temp_string)
        free(temp_string);

}

void* logger_print(void* arg) {
    if (!arg)
        return NULL;

    pthread_cleanup_push(cleanup, NULL)

    log_file = fopen("log.txt", "w+");
    if (!log_file)
        return NULL;


    logger_args* l_args = (logger_args*)arg;

    string_buffer* log_buf = l_args->log_buf;

    while (true) {

        string_buffer_lock(log_buf);
        if (string_buffer_is_empty(log_buf)) {
            string_buffer_wait_put(log_buf);
        }
        temp_string = string_buffer_get(log_buf);
        string_buffer_call_put(log_buf);
        string_buffer_unlock(log_buf);

        fputs(temp_string, log_file);
        free(temp_string);
        temp_string = NULL;

    }


    pthread_cleanup_pop(1);

    return NULL;
}