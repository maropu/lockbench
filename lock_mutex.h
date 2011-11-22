/*-------------------------------------------------------------------------
 *
 * lock_mutex.h - 20110720 v0.1.0
 *	  GNU PTHREAD mutex implementation
 *
 *-------------------------------------------------------------------------
 */

#ifndef LOCK_MUTEX_H
#define LOCK_MUTEX_H

#include <pthread.h>

typedef pthread_mutex_t lock_t;

static inline void
lock_init(lock_t *lock)
{
        pthread_mutex_init(lock, NULL);
}

static inline void
lock_acquire(lock_t *lock)
{
        pthread_mutex_lock(lock);
}

static inline void
lock_release(lock_t *lock)
{
        pthread_mutex_unlock(lock);
}

static inline int
lock_size()
{
	return sizeof(lock_t);
}

#endif /* LOCK_MUTEX_H */
