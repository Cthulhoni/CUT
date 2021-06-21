#include <reader.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <string_buffer.h>

#define SLEEP_TIME 1U

static inline bool line_starts_with(const char* line, const char* start) {
    return strncmp(line, start, strlen(start)) == 0;
}


void* reader_get_cpu_data(void* arg) {
    if (!arg) {
        return NULL;
    }

    string_buffer* sbuf = (string_buffer*)arg;

    while (true) {
        FILE* stat_file = fopen("/proc/stat", "r");
        if (!stat_file)
            return NULL;

        sleep(SLEEP_TIME);

        char buffer[2048];
        
        size_t number_of_characters = 0;

        while (fgets(buffer, sizeof(buffer), stat_file) && line_starts_with(buffer, "cpu")) {
            number_of_characters += sizeof(buffer);
        }

        rewind(stat_file);

        char* raw_data = malloc(sizeof(*raw_data) * (number_of_characters + 1));
        if (!raw_data)
            return NULL;
        *raw_data = '\0';
        while (fgets(buffer, sizeof(buffer), stat_file) && line_starts_with(buffer, "cpu")) {
            strcat(raw_data, buffer);
        }


        string_buffer_lock(sbuf);
        if (string_buffer_is_full(sbuf)) {
            string_buffer_wait_get(sbuf);
        }
        string_buffer_put(sbuf, raw_data);
        string_buffer_call_get(sbuf);
        string_buffer_unlock(sbuf);

        free(raw_data);

        fclose(stat_file);

    }

    return NULL;
}
