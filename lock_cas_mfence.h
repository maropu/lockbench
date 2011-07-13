/*-------------------------------------------------------------------------
 *
 * lock_none.h - 20110711 v0.1.0
 *	  Naive spin-lock implementation with compare-and-swap
 *
 *-------------------------------------------------------------------------
 */

#ifndef LOCK_CAS_H
#define LOCK_CAS_H

typedef unsigned char   lock_t;

static inline void
locks_init(int nlocks, int nworkers)
{
        /* None */
}

static inline void
lock_acquire(volatile lock_t *lock)
{
        register lock_t _old;
        register lock_t _new;

        _new = 1;

        while(1) {
                __asm__ __volatile__(
                        "   xorb       %0,%0    \n"
                        "   mfence              \n"
                        "   cmpxchgb   %2,%1    \n"
                        :"+a"(_old), "+m"(*lock)
                        :"q"(_new), "a"(_old)
                        :"memory", "cc");

                if(_old == 0)
                        break;

                __asm__ __volatile__(
                        " rep; nop      \n");
        }
}

static inline void
lock_release(volatile lock_t *lock)
{
        register lock_t _res;

        _res = 0;

        __asm__ __volatile__(
                "   xchgb   %0,%1       \n"
                :"+q"(_res), "+m"(*lock)
                ::"memory", "cc");
}

static inline int
lock_size()
{
	return sizeof(lock_t);
}

#endif /* LOCK_CAS_H */
