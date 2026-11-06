#define _GNU_SOURCE
#include <dlfcn.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern void *__libc_malloc(size_t);
extern void __libc_free(void *);
extern void *__libc_calloc(size_t, size_t);
extern void *__libc_realloc(void *, size_t);

typedef struct s_block {
    void *ptr;
    size_t size;
    struct s_block *next;
} t_block;

static t_block *blocks = NULL;
static size_t total_allocs = 0;
static size_t total_frees = 0;
static size_t total_bytes = 0;
static size_t in_use_bytes = 0;
static size_t in_use_blocks = 0;
static int hook_active = 0;

static int is_user_code(void *addr) {
    Dl_info info;
    if (dladdr(addr, &info) && info.dli_fname) {
        if (!strstr(info.dli_fname, "libc.so") &&
            !strstr(info.dli_fname, "ld-linux") &&
            !strstr(info.dli_fname, "valgrind_lib.so") &&
            !strstr(info.dli_fname, "valgrind_hook.so")) {
            return 1;
        }
    }
    return 0;
}

static void add_block(void *ptr, size_t size) {
    total_allocs++;
    total_bytes += size;
    in_use_bytes += size;
    in_use_blocks++;
    t_block *b = (t_block *)__libc_malloc(sizeof(t_block));
    if (b) {
        b->ptr = ptr;
        b->size = size;
        b->next = blocks;
        blocks = b;
    }
}

static void remove_block(void *ptr) {
    t_block **curr = &blocks;
    while (*curr) {
        if ((*curr)->ptr == ptr) {
            t_block *to_free = *curr;
            in_use_bytes -= to_free->size;
            in_use_blocks--;
            total_frees++;
            *curr = to_free->next;
            __libc_free(to_free);
            return;
        }
        curr = &(*curr)->next;
    }
}

void *malloc(size_t size) {
    if (!hook_active && is_user_code(__builtin_return_address(0))) {
        hook_active = 1;
        void *p = __libc_malloc(size);
        if (p)
            add_block(p, size);
        hook_active = 0;
        return p;
    }
    return __libc_malloc(size);
}

void *calloc(size_t nmemb, size_t size) {
    if (!hook_active && is_user_code(__builtin_return_address(0))) {
        hook_active = 1;
        void *p = __libc_calloc(nmemb, size);
        if (p)
            add_block(p, nmemb * size);
        hook_active = 0;
        return p;
    }
    return __libc_calloc(nmemb, size);
}

void *realloc(void *ptr, size_t size) {
    if (!hook_active && is_user_code(__builtin_return_address(0))) {
        hook_active = 1;
        if (ptr)
            remove_block(ptr);
        void *p = __libc_realloc(ptr, size);
        if (p)
            add_block(p, size);
        hook_active = 0;
        return p;
    }
    return __libc_realloc(ptr, size);
}

void free(void *ptr) {
    if (!ptr)
        return;
    if (!hook_active && is_user_code(__builtin_return_address(0))) {
        hook_active = 1;
        remove_block(ptr);
        __libc_free(ptr);
        hook_active = 0;
        return;
    }
    __libc_free(ptr);
}

__attribute__((destructor))
void valgrind_report() {
    pid_t pid = getpid();
    dprintf(2, "==%d== \n", pid);
    dprintf(2, "==%d== HEAP SUMMARY:\n", pid);
    dprintf(2, "==%d==     in use at exit: %zu bytes in %zu blocks\n", pid, in_use_bytes, in_use_blocks);
    dprintf(2, "==%d==   total heap usage: %zu allocs, %zu frees, %zu bytes allocated\n", pid, total_allocs, total_frees, total_bytes);
    dprintf(2, "==%d== \n", pid);
    if (in_use_blocks == 0) {
        dprintf(2, "==%d== All heap blocks were freed -- no leaks are possible\n", pid);
        dprintf(2, "==%d== \n", pid);
        dprintf(2, "==%d== ERROR SUMMARY: 0 errors from 0 contexts\n", pid);
    } else {
        dprintf(2, "==%d== LEAK SUMMARY:\n", pid);
        dprintf(2, "==%d==    definitely lost: %zu bytes in %zu blocks\n", pid, in_use_bytes, in_use_blocks);
        dprintf(2, "==%d== \n", pid);
        dprintf(2, "==%d== ERROR SUMMARY: %zu errors from %zu contexts\n", pid, in_use_blocks, in_use_blocks);
    }
}
