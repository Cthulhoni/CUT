#include <reader.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <string_buffer.h>

#define SLEEP_TIME 1U

struct reader_args {
    string_buffer* sbuf;
    watch_struct* w_struct;
    string_buffer* log_buf;
};

reader_args* reader_args_create(string_buffer* sbuf, watch_struct* w_struct, string_buffer* log_buf) {
    reader_args* r_args = malloc(sizeof(*r_args));
    if (!r_args)
        return NULL;
    if (!sbuf || !w_struct || !log_buf)
        return NULL;
    r_args->sbuf = sbuf;
    r_args->w_struct = w_struct;
    r_args->log_buf = log_buf;
    return r_args;
}
void reader_args_destroy(reader_args* r_args) {
    if (r_args)
        free(r_args);
}

static FILE* stat_file = NULL;
static char* raw_data = NULL;

static void cleanup() {
    if (stat_file)
        fclose(stat_file);
    
    if (raw_data)
        free(raw_data);
}

static inline bool line_starts_with(const char* line, const char* start) {
    return strncmp(line, start, strlen(start)) == 0;
}

void* reader_get_cpu_data(void* arg) {
    if (!arg) {
        return NULL;
    }

    pthread_cleanup_push(cleanup, NULL)

    reader_args* r_args = (reader_args*)arg;

    string_buffer* sbuf = r_args->sbuf;
    watch_struct* w_struct = r_args->w_struct;
    string_buffer* log_buf = r_args->log_buf;
    (void)log_buf;

    STRING_BUFFER_PUT(log_buf, "Reader launched\n");

    while (watch_struct_is_running(w_struct)) {

        watch_struct_reader_signal(w_struct);
        stat_file = fopen("/proc/stat", "r");
        if (!stat_file)
            return NULL;

        sleep(SLEEP_TIME);

        char buffer[2048];
        
        size_t number_of_characters = 0;

        while (fgets(buffer, sizeof(buffer), stat_file) && line_starts_with(buffer, "cpu")) {
            number_of_characters += strlen(buffer);
        }

        rewind(stat_file);

        raw_data = malloc(sizeof(*raw_data) * (number_of_characters + 1));
        if (!raw_data)
            return NULL;
        *raw_data = '\0';
        while (fgets(buffer, sizeof(buffer), stat_file) && line_starts_with(buffer, "cpu")) {
            strcat(raw_data, buffer);
        }

        STRING_BUFFER_PUT(sbuf, raw_data);

        free(raw_data);
        raw_data = NULL;

        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        fclose(stat_file);
        stat_file = NULL;
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    }
    STRING_BUFFER_PUT(log_buf, "Reader finished loop\n");


    pthread_cleanup_pop(1);

    return NULL;
}
