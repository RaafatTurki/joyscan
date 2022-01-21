/* only supports ints, can be abstracted to void pointers if other types are needed */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "log.h"
#pragma once

typedef struct {
    int *buffer;
    int used;
    int size;
} Vector;

void vec_init(Vector *v, int init_items_count) {
    v->buffer = malloc(sizeof(int) * init_items_count);
    v->size = init_items_count;
    v->used = 0;
}

void vec_append(Vector *v, int val) {
    // v->used is the number of used entries, because v->buffer[v->used++] updates v->used only after the array has been accessed.
    // therefore a->used can go up to a->size
    if (v->used == v->size) {
        if (v-> size == 0) {
            v->size = 1;
        } else {
            v->size *= 2;
        }
        v->buffer = realloc(v->buffer, v->size * sizeof(int));
    }
    v->buffer[v->used] = val;
    v->used++;
}

void vec_remove(Vector *v, int i) {
    assert(i >= 0);
    assert(i < v->used);

    while (i < v->used-1) {
        v->buffer[i] = v->buffer[i+1];
        i++;
    }
    v->used--;
}

int vec_get(Vector *v, int i) {
    assert(i >= 0);
    assert(i < v->used);
    return v->buffer[i];
}

int vec_len(Vector *v) {
    return v->used;
}

/* TODO use print_log() */
void vec_print(Vector *v) {
    for(int i = 0; i < v->used; i++) {
        printf("%d ", v->buffer[i]);
    }
    for(int i = 0; i < v->size - v->used; i++) {
        printf(". ");
    }
    printf("\n");
    /* print_log(DEBUG, TextFormat(":C")); */
}

void vec_free(Vector *v) {
    free(v->buffer);
    v->buffer = NULL;
    v->used = v->size = 0;
}

// -1 if false, [0 - inf] if true
int vec_has_val(Vector *v, int target_val) {
    for (int i = 0; i < vec_len(v); i++) {
        int val = vec_get(v, i);
        if (val == target_val) return i;
    }
    return -1;
}

int vec_next(Vector *v, int i) {
    assert(i >= 0);
    assert(i < v->used);
    int new_i;

    if (i == v->used-1) {
        new_i = 0;
    } else {
        new_i = i+1;
    }
    return new_i;
}

int vec_prev(Vector *v, int i) {
    assert(i >= 0);
    assert(i < v->used);
    int new_i;
    if (i == 0) {
        new_i = v->used-1;
    } else {
        new_i = i-1;
    }
    return new_i;
}
