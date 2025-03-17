#include <common.h>

static void os_init() {
    pmm->init();
}

static void os_run() {
    printf("Hello from CPU #%d!\n", cpu_current());
    while(1); // The user-defined OS entry should not be returned
}

MODULE_DEF(os) = {
    .init = os_init,
    .run = os_run
};