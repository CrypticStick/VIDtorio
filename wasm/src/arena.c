// Based on implementation from https://nullprogram.com/blog/2023/09/27/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arena.h"

arena arena_new(ptrdiff_t cap) {
    arena a = {0};
    a.beg = malloc(cap);
    a.end = a.beg ? a.beg+cap : 0;
    return a;
}

void *arena_alloc(arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, arena_flags flags) {
    ptrdiff_t padding = -(intptr_t)a->beg & (align - 1);
    ptrdiff_t available = a->end - a->beg - padding;
    if (available < 0 || count > available/size) {
        printf("Error: out of memory\n");
        abort();
    }
    void *p = a->beg + padding;
    a->beg += padding + count*size;
    return (flags & NO_ZERO) ?  p : memset(p, 0, count*size);
}
