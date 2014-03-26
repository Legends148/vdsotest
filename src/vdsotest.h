#ifndef VDSOTEST_H
#define VDSOTEST_H

#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

#include "compiler.h"

struct ctx {
	volatile sig_atomic_t should_stop;
	struct itimerspec duration;
	cpu_set_t cpus_allowed;
	unsigned long long max_fails;
	unsigned long long fails;
};

struct bench_results {
	int dummy;
};

struct test_suite {
	const char *name; /* name of the API under test */

	/* Estimate speedup obtained by using vDSO implementation vs syscall */
	int (*bench)(struct ctx *ctx, struct bench_results *res);

	/* Check for inconsistencies between vDSO and syscall
	 * implemenations, usually by rapidly switching between the
	 * two modes and comparing results obtained.
	 *
	 * FIXME: distinguish between self-consistency (vDSO-only) and
	 * vDSO vs kernel consistency.  Or assume that doing vDSO vs
	 * kernel will catch everything.
	 */
	int (*verify)(struct ctx *ctx);

	/* Check for ABI inconsistencies, within reason - e.g. vDSO
	 * may get SIGSEGV where syscall may return EFAULT.
	 */
	int (*abi)(struct ctx *ctx);
};

void register_testsuite(const struct test_suite *ts);

void ctx_start_timer(struct ctx *ctx);

static inline bool test_should_stop(const struct ctx *ctx)
{
	return ctx->should_stop;
}

void log_failure(struct ctx *ctx, const char *fmt, ...) __printf(2, 3);

#endif