#include <printer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string_buffer.h>

#define CLEAR_SCREEN() printf("\033c")

void* printer_print(void* arg) {
    
    string_buffer* print_buf = (string_buffer*)arg;

    while(true) {

        string_buffer_lock(print_buf);
        if (string_buffer_is_empty(print_buf)) {
            string_buffer_wait_put(print_buf);
        }
        char* temp = string_buffer_get(print_buf);
        string_buffer_call_put(print_buf);
        string_buffer_unlock(print_buf);

        CLEAR_SCREEN();
        printf("%s\n", temp);
        free(temp);

    }

}
