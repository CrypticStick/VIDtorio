// Based on implementation from https://nullprogram.com/blog/2023/09/27/
#ifndef ARENA_H_
#define ARENA_H_

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

// void *a_malloc(arena *a, <t> type, int count = 1, arena_flags flags = 0);
#define a_malloc(...)            a_mallocx(__VA_ARGS__,a_malloc4,a_malloc3,a_malloc2)(__VA_ARGS__)
#define a_mallocx(a,b,c,d,e,...) e
#define a_malloc2(a, t)          (t *)arena_alloc(a, sizeof(t), alignof(t), 1, 0)
#define a_malloc3(a, t, n)       (t *)arena_alloc(a, sizeof(t), alignof(t), n, 0)
#define a_malloc4(a, t, n, f)    (t *)arena_alloc(a, sizeof(t), alignof(t), n, f)

typedef struct arena_ {
    int8_t *beg;
    int8_t *end;
} arena;

typedef enum arena_flags_ {
    NO_ZERO = 0x01
} arena_flags;

arena arena_new(ptrdiff_t cap);

void *arena_alloc(arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, arena_flags flags);

#endif // ARENA_H_
