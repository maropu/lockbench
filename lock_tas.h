/*-------------------------------------------------------------------------
 *
 * lock_tas.h - 20110711 v0.1.0
 *	  Naive spin-lock implementation with test-and-set
 *
 *-------------------------------------------------------------------------
 */

#ifndef LOCK_TAS_H
#define LOCK_TAS_H

typedef unsigned char   lock_t;

static inline void
locks_init(int nlocks, int nworkers)
{
        /* None */
}

static inline void
lock_acquire(volatile lock_t *lock)
{
        register lock_t _ret;

        _ret = 1;

        while(1) {
                __asm__ __volatile__(
                        "   cmpb    $0,%1       \n"
                        "   jne     1f          \n"
                        "   xchgb   %0,%1       \n"
                        "1:                     \n"
                        :"+q"(_ret), "+m"(*lock)
                        ::"memory", "cc");

                if(!_ret)
                        break;

                __asm__ __volatile__(
                        " rep; nop              \n");
        }
}

static inline void
lock_release(volatile lock_t *lock)
{
        register lock_t _res;

        _res = 0;

        __asm__ __volatile__(
                "   xchgb   %0,%1   \n"
                :"+q"(_res), "+m"(*lock)
                ::"memory", "cc");
}

static inline int
lock_size()
{
	return sizeof(lock_t);
}

#endif /* LOCK_TAS_H */
