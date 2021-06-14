#include <analyzer.h>
#include <stdlib.h>

#include <stdio.h>

#include <string_buffer.h>

void* analyzer_process_cpu_data(void* arg) {
    if (!arg) {
        return NULL;
    }

    string_buffer* sbuf = *(string_buffer**)arg;

    while (true) {
        string_buffer_lock(sbuf);

        if (string_buffer_is_empty(sbuf)) {
            string_buffer_wait_put(sbuf);
        }

        char* temp = string_buffer_get(sbuf);
        string_buffer_call_put(sbuf);
        string_buffer_unlock(sbuf);
        printf("%s\n\n", temp);
        free(temp);
    }

    return NULL;
    
}
