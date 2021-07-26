
#ifndef TIMER_TIMER_H_
#define TIMER_TIMER_H_

/*
 *  This small library was inspired by this stackoverflow post: 
 *  https://stackoverflow.com/questions/19758101/signal-handler-to-stop-timer-in-c
 *  
 *  timer_start() :
 * 		callback-fcn: function to be called upon the expiration of the timer.
 * 		sec & us: together these parameters additively specify how long the timer should be
 *
 *  MS_TO_US() : 
 * 		this is provided for convenience
 *
 *  This library uses setitimer() POSIX calls. For more info see:
 *  http://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html
 *  http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html#Elapsed-Time
 */

#include <sys/time.h>

#define MS_TO_US(x)		(x * 1000)

void timer_start(void (*callback_fcn)(void), time_t sec, time_t us);

#endif