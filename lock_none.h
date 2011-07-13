/*-------------------------------------------------------------------------
 *
 * lock_none.h - 20110711 v0.1.0
 *	  To provide baseline performane compared to lock primitive ones.
 *
 *-------------------------------------------------------------------------
 */

#ifndef LOCK_NONE_H
#define LOCK_NONE_H

typedef unsigned char   lock_t;

static inline void
locks_init(int nlocks, int nworkers)
{
        /* None */
}

static inline void
lock_acquire(lock_t *lock)
{
        /* None */
}

static inline void
lock_release(lock_t *lock)
{
        /* None */
}

static inline int
lock_size()
{
	return 0;
}

#endif /* LOCK_NONE_H */
