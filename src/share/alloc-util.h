/* SPDX-License-Identifier: LGPL-2.1+ */
#pragma once

#include <alloca.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"

#define new(t, n) ((t*) malloc_multiply(n, sizeof(t)))

#define new0(t, n) ((t*) calloc((n), sizeof(t)))

#define newa(t, n) ((t*) alloca(sizeof(t)*(n)))

#define newa0(t, n) ((t*) alloca0(sizeof(t)*(n)))

#define newdup(t, p, n) ((t*) memdup_multiply(p, sizeof(t), (n)))

#define malloc0(n) (calloc(1, (n)))

static inline void *mfree(void *memory) {
        free(memory);
        return NULL;
}

void* memdup(const void *p, size_t l) _alloc_(2);

static inline void freep(void *p) {
        free(*(void**) p);
}

#define _cleanup_free_ _cleanup_(freep)

static inline bool size_multiply_overflow(size_t size, size_t need) {
        return _unlikely_(need != 0 && size > (SIZE_MAX / need));
}

_malloc_  _alloc_(1, 2) static inline void *malloc_multiply(size_t size, size_t need) {
        if (size_multiply_overflow(size, need))
                return NULL;

        return malloc(size * need);
}

_alloc_(2, 3) static inline void *realloc_multiply(void *p, size_t size, size_t need) {
        if (size_multiply_overflow(size, need))
                return NULL;

        return realloc(p, size * need);
}

_alloc_(2, 3) static inline void *memdup_multiply(const void *p, size_t size, size_t need) {
        if (size_multiply_overflow(size, need))
                return NULL;

        return memdup(p, size * need);
}

void* greedy_realloc(void **p, size_t *allocated, size_t need, size_t size);
void* greedy_realloc0(void **p, size_t *allocated, size_t need, size_t size);

#define GREEDY_REALLOC(array, allocated, need)                          \
        greedy_realloc((void**) &(array), &(allocated), (need), sizeof((array)[0]))

#define GREEDY_REALLOC0(array, allocated, need)                         \
        greedy_realloc0((void**) &(array), &(allocated), (need), sizeof((array)[0]))

#define alloca0(n)                                      \
        ({                                              \
                char *_new_;                            \
                size_t _len_ = n;                       \
                _new_ = alloca(_len_);                  \
                (void *) memset(_new_, 0, _len_);       \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#define alloca_align(size, align)                                       \
        ({                                                              \
                void *_ptr_;                                            \
                size_t _mask_ = (align) - 1;                            \
                _ptr_ = alloca((size) + _mask_);                        \
                (void*)(((uintptr_t)_ptr_ + _mask_) & ~_mask_);         \
        })

#define alloca0_align(size, align)                                      \
        ({                                                              \
                void *_new_;                                            \
                size_t _size_ = (size);                                 \
                _new_ = alloca_align(_size_, (align));                  \
                (void*)memset(_new_, 0, _size_);                        \
        })
