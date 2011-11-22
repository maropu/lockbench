/*-------------------------------------------------------------------------
 *
 * lock_pthread_spinlock.h - 20111123 v0.1.0
 *	  A spin-lock implementation implemented by GNU PTHREAD
 *
 *-------------------------------------------------------------------------
 */

#ifndef LOCK_PTHREAD_SPINLOCK_H
#define LOCK_PTHREAD_SPINLOCK_H

#include <pthread.h>

typedef pthread_spinlock_t      lock_t;

static inline void
lock_init(lock_t *lock)
{
        pthread_spin_init(lock, NULL);
}

static inline void
lock_acquire(volatile lock_t *lock)
{
        pthread_spin_lock(lock);
}

static inline void
lock_release(volatile lock_t *lock)
{
        pthread_spin_unlock(lock);
}

static inline int
lock_size()
{
	return sizeof(lock_t);
}

#endif /* LOCK_PTHREAD_SPINLOCK_H */

