#include <analyzer.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>


#define PERCENTAGE_LINE_SIZE 16

struct analyzer_args {
    string_buffer* sbuf;
    string_buffer* print_buf;
};

analyzer_args* analyzer_args_create(string_buffer* sbuf, string_buffer* print_buf) {
    analyzer_args* a_args = malloc(sizeof(*a_args));
    if (!a_args)
        return NULL;
    a_args->sbuf = sbuf;
    a_args->print_buf = print_buf;
    return a_args;
}

void analyzer_args_destroy(analyzer_args* a_args) {
    free(a_args);
}

void* analyzer_process_cpu_data(void* arg) {
    if (!arg)
        return NULL;

    analyzer_args* a_args = (analyzer_args*) arg;

    string_buffer* sbuf = a_args->sbuf;
    string_buffer* print_buf = a_args->print_buf;

    unsigned long long* last_idle = NULL;
    unsigned long long* last_total = NULL;

    /* Assumes number of cores doesn't change during execution */
    size_t cores = 0;

    while (true) {
        string_buffer_lock(sbuf);
        if (string_buffer_is_empty(sbuf)) {
            string_buffer_wait_put(sbuf);
        }
        char* temp = string_buffer_get(sbuf);
        string_buffer_call_put(sbuf);
        string_buffer_unlock(sbuf);
        
        if (!last_idle || !last_total) {
            
            for (size_t i = 0; i < strlen(temp); ++i) {
                if (temp[i] == '\n')
                    ++cores;
            }
            last_idle  = calloc(cores, sizeof(*last_idle));
            last_total = calloc(cores, sizeof(*last_total));
            if (!last_idle || !last_total)
                return NULL;
        }
        char* percentage_data = NULL;
        if (cores != 0) {
            percentage_data = malloc(sizeof(*percentage_data) * (cores * (PERCENTAGE_LINE_SIZE)));
            if (!percentage_data)
                return NULL;

            *percentage_data = '\0';
        }

        const char* newline = "\n";
        char* line = strtok(temp, newline);
        if (line) {
            line = strtok(NULL, newline);
        }

        size_t cpu_id;
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
        while (line) {
            sscanf(line, "cpu%zu %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
                   &cpu_id, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

            user = user - guest;
            nice = nice - guest_nice;
            unsigned long long total_idle = idle + iowait;
            unsigned long long total_non_idle = user + nice + system + irq + softirq + steal + guest + guest_nice;

            
            if (last_total[cpu_id] != 0 && last_total[cpu_id] != total_idle + total_non_idle) {
                /*fix types and conversion here*/
                double percentage = 100.0 - ((double)(total_idle - last_idle[cpu_id]) * 100.0) / ((double)(total_idle + total_non_idle - last_total[cpu_id]));

                char* buffer = malloc(sizeof(*buffer) * PERCENTAGE_LINE_SIZE);
                if (!buffer)
                    return NULL;
                sprintf(buffer, "cpu%zu %.1lf%%\n", cpu_id, percentage);
                strcat(percentage_data, buffer);
                free(buffer);
            }

            
            last_idle[cpu_id] = total_idle;
            last_total[cpu_id] = total_idle + total_non_idle;

            line = strtok(NULL, newline);
        }


        /* First string to enter print_buf is always empty */

        string_buffer_lock(print_buf);
        if (string_buffer_is_full(print_buf)) {
            string_buffer_wait_get(print_buf);
        }
        string_buffer_put(print_buf, percentage_data);
        string_buffer_call_get(print_buf);
        string_buffer_unlock(print_buf);

        free(percentage_data);
        free(temp);
    }

    if (last_total)
        free(last_total);
    if (last_idle)
        free(last_idle);

    return NULL;
    
}
