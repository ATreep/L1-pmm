#ifndef SPINLOCK_H__
#define SPINLOCK_H__

typedef int spinlock_t;
// static inline int atomic_xchg(volatile int *addr, int newval)
// {
//     int result;
//     asm volatile ("lock xchg %0, %1" 
//         : "+m"(*addr), "=a"(result) 
//         : "1"(newval) 
//         : "memory"
//     );
//     return result;
// }

inline void spin_lock(spinlock_t *lk)
{
    while (1)
    {
        int value = atomic_xchg(lk, 1);
        if (value == 0)
        {
            break;
        }
    }
}

inline void spin_unlock(spinlock_t *lk)
{
    atomic_xchg(lk, 0);
}

#endif