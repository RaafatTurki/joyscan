#include <stdio.h>
#pragma once

char *log_type_names[] = {"DEBUG", "MAIN"};

typedef enum {
    DEBUG,
    MAIN,
} LogTypes;

void print_log(LogTypes type, const char *msg) {
    if (type == DEBUG && !DEBUG_MODE) return;
    printf("%s: %s\n", log_type_names[type], msg);
}

