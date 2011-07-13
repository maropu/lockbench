/*-------------------------------------------------------------------------
 *
 * lockbench.h - 20110711 v0.1.0
 *	  Test codes for lock_xxx.h (benchmarks for cpu primitive locks)
 *
 *-------------------------------------------------------------------------
 */

#if !defined(__GNUC__)
#error "gcc is required."
#endif

#if !defined(__i686__)
#error "micro architecture is not supported."
#endif

#if !defined(LOCKIMPL)
#error "LOCKIMPL is not defined."
#endif

#include LOCKIMPL

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>

#include "err_utils.h"

#define CACHELINE_SZ    64
#define WORKERS_MAX     32
#define LOCKS_MAX       4

#define _rdtsc(t)       \
        do {            \
                __asm__ __volatile__ ("cpuid" : : : "eax", "ebx", "ecx", "edx");        \
                __asm__ __volatile__ ("rdtsc" : "=A" (t));                              \
        } while (0)

typedef union {
	unsigned long long      value;
	char    pad[CACHELINE_SZ];
} counter_t;

struct _thread_arg {
        union {
                lock_t          lock;
                char    pad[CACHELINE_SZ];
        } lock_t;

        volatile counter_t      counter;
};

static void _usage(char *msg, ...);
static void *_start_worker(void *arg)  __attribute__((noinline));

static int              count_per_cycle;

int main(int argc, char **argv)
{
        int             i;
        int             N;
        int             L;
        int             T;
        long long int   st;
        long long int   et;
        long long int   total_counter;
        char            *end;
        struct _thread_arg      th[WORKERS_MAX];
        pthread_t               tid[WORKERS_MAX];

        /* Parameter settings */
        if (argc < 5)
                _usage(NULL);

        count_per_cycle = strtol(argv[1], &end, 10);

        if ((*end != '\0') || (count_per_cycle <= 0) || (errno == ERANGE))
                _usage("counter increments per cycle '%s' invalid", argv[1]);

        N = strtol(argv[2], &end, 10);

        if ((*end != '\0') || (count_per_cycle <= 0) || (errno == ERANGE))
		_usage("worker count '%s' invalid", argv[2]);

        if (N > WORKERS_MAX)
                eoutput("too many workers");

        T = strtol(argv[3], &end, 10);

        if ((*end != '\0') || (count_per_cycle <= 0) || (errno == ERANGE))
		_usage("timeout '%s' invalid", argv[3]);

        L = strtol(argv[4], &end, 10);

        if ((*end != '\0') || (count_per_cycle <= 0) || (errno == ERANGE))
		_usage("lock count '%s' invalid", argv[4]);

        if (L > LOCKS_MAX)
                eoutput("too many locks");

        /* Initialization */
        memset(&th, 0x00, sizeof(struct _thread_arg) * LOCKS_MAX);

        /* Timer checkpoint */
        _rdtsc(st);

        /* Start workers */
        for (i = 0; i < N; i++)
                pthread_create(&tid[i], NULL, _start_worker, &th[i / (N / L)]);

	/* Let workers run for a while */
	sleep(T);
	
        /* Cancel workers */
        for (i = 0; i < N; i++)
                pthread_cancel(tid[i]);

        /* Timer checkpoint */
        _rdtsc(et);

        /* Aggregate data */
        for (i = 0, total_counter = 0; i < L; i++)
                total_counter += th[i].counter.value;

        /* Display data */
        for (i = 0; i < L; i++) {
                printf("lock %d counter: %llu\n", i, th[i].counter.value);
                printf("lock %d rdtsc: %lld\n", i, et - st);
                printf("lock %d per-cycle rdtsc: %.1e\n", i, (double)(et - st) / th[i].counter.value);
                printf("lock %d cycle throughput: %.1e\n", i, (double)th[i].counter.value / (et - st));
        }

        printf("---\n");

        printf("Averaged per-cycle rdtsc: %.1e\n", (double)(et - st) / ((double)total_counter / L));
        printf("Averaged cycle throughput: %.1e\n", ((double)total_counter / L) / (et - st));

        return EXIT_SUCCESS;
}

/*--- Intra functions below ---*/

void
_usage(char *msg, ...)
{
        fprintf(stderr, "Usage: locks <increment per cycle> <worker count> <seconds> <lock count>\n");

        if (msg != NULL) {
                va_list vargs;

                va_start(vargs, msg);
                vfprintf(stderr, msg, vargs);
                va_end(vargs);

                fprintf(stderr, "\n");
        }

        exit(1);
}

void *
_start_worker(void *arg)
{
        int     i;
        struct _thread_arg      *p;

        p = (struct _thread_arg *)arg;

        if (count_per_cycle == 1) {
                while (1) {
                        lock_acquire(&p->lock_t.lock);
                        ++(p->counter.value);
                        lock_release(&p->lock_t.lock);
                }
        } else {
                while (1) {
                        lock_acquire(&p->lock_t.lock);
                        for (i = 0; i < count_per_cycle; i++)
                                ++(p->counter.value);
                        lock_release(&p->lock_t.lock);
                }
        }

        return NULL;
}

