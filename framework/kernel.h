#include <am.h>

#define MODULE(mod)                             \
    typedef struct mod_##mod##_t mod_##mod##_t; \
    extern mod_##mod##_t *mod;                  \
    struct mod_##mod##_t

#define MODULE_DEF(mod)                  \
    extern mod_##mod##_t __##mod##_obj;  \
    mod_##mod##_t *mod = &__##mod##_obj; \
    mod_##mod##_t __##mod##_obj

typedef Context *(*handler_t)(Event, Context *);
MODULE(os)
{
    void (*init)();
    void (*run)();
    Context *(*trap)(Event ev, Context *context);
    void (*on_irq)(int seq, int event, handler_t handler);
};

MODULE(pmm)
{
    void (*init)();
    void *(*alloc)(size_t size);
    void (*free)(void *ptr);
};