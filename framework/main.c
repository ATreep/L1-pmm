#include <common.h>
#include <klib.h>

#define max(a, b) (a < b ? b : a)
#define min(a, b) (a < b ? a : b)

#define MAX_ALLOC_NUM 200

enum ops
{
    OP_ALLOC = 1,
    OP_FREE
};

struct malloc_op
{
    enum ops type;
    union
    {
        size_t sz;  // OP_ALLOC: size
        void *addr; // OP_FREE: address
    };
};

struct mem
{
    void *addr;
    size_t sz;
};

struct malloc_op random_op(struct mem *alloc_addr_arr, int arr_sz)
{
    int sign = rand() % 2;

    if (arr_sz == MAX_ALLOC_NUM)
    {
        sign = 1;
    }

    if (arr_sz == 0) sign = 0;
        
    if (sign == 0)
    {
        // Require 128K + Noise 
        return (struct malloc_op) {.type = OP_ALLOC, .sz = (1 << 17) + (cpu_current() << 10)};
    }
    else
    {
        return (struct malloc_op) {.type = OP_FREE, .addr = alloc_addr_arr[arr_sz - 1].addr};
    }
}

void alloc_check(void *ptr, size_t sz, struct mem *alloc_addr_arr, int arr_sz)
{
    if (ptr == NULL)
    {
        panic("Fail to allocate. Heap may be full.");
    }

    for (int i = 0; i < arr_sz - 1; i++)
    {
        if (max(ptr, alloc_addr_arr[i].addr) <= min(ptr + sz, alloc_addr_arr[i].addr + alloc_addr_arr[i].sz))
        {
            printf("[!] Overlapped: mem[%p, %p] asgn[%p, %p]\n", 
                ptr, ptr + sz, alloc_addr_arr[i].addr, alloc_addr_arr[i].addr + alloc_addr_arr[i].sz);
            halt(1);
        }
    }
}

void stress_test()
{
    struct mem *alloc_addr_arr = pmm->alloc(sizeof(struct mem) * MAX_ALLOC_NUM);
    int arr_sz = 0;
    while (1)
    {
        struct malloc_op op = random_op(alloc_addr_arr, arr_sz);

        switch (op.type)
        {
            case OP_ALLOC:
            {
                void *ptr = pmm->alloc(op.sz);

                alloc_addr_arr[arr_sz++] = (struct mem){.addr = ptr, .sz = op.sz};

                alloc_check(ptr, op.sz, alloc_addr_arr, arr_sz);

                break;
            }
            case OP_FREE:
            {
                arr_sz--;

                pmm->free(op.addr);
                
                break;
            }
        }
    }
    pmm->free(alloc_addr_arr);
}

int main()
{
    os->init();
    mpe_init(stress_test);
    return 0;
}