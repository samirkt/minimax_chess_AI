
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#include "timer.h"

static void (*callback)(void);
static struct itimerval it_val;

static void handler(int signum);

void timer_start(void (*callback_fcn)(void), time_t sec, time_t us){

	// set callback function
	callback = callback_fcn;

	// set interval
	it_val.it_value.tv_sec = sec;
	it_val.it_value.tv_usec = us;

	// one-shot timer
	it_val.it_interval.tv_sec = 0;
    it_val.it_interval.tv_usec = 0;

	assert(signal(SIGALRM, handler) != SIG_ERR);

	assert(setitimer(ITIMER_REAL, &it_val, NULL) != -1);
}

static void handler(int signum){

	// unused parameter, this stops compiler warning
	(void) signum;

	callback();
}