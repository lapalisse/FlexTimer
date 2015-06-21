//
//  flextimer.h
//  TimeInterrupt
//
//  Created by Ludovic Bertsch on 19/06/2015.
//  Copyright (c) 2015 Ludovic Bertsch. All rights reserved.
//

#ifndef TimeInterrupt_flextimer_h
#define TimeInterrupt_flextimer_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <limits.h>

// Not really used: for further extensions!
typedef enum {
    CATCH_UP = 1,
    DO_NOT_CATCH_UP = 2,
    REMOVE_WHEN_FINISHED = 4,
    DEACTIVATE_WHEN_FINISHED = 8
} TIMER_TYPE;

// Not really used
#define DEFAULT_PARAMETERS (CATCH_UP|REMOVE_WHEN_FINISHED)

// Super important value: to be used for "repeat" parameter!
#define RUN_FOREVER (-1)

// Super important value: size of timer array (limited memory allocation scheme)
#define NB_MAX_TIMERS (10)

typedef long int32;
typedef unsigned long uint32;

// This could be important: we want some constraints here!
// INT_MAX would be in theory the best, but... if we miss it, we're screwed!
#define DELAY_MAX_VALUE (INT_MAX/2)

typedef struct timer_t {
    uint32 delay;           // Delay between 2 ticks! RUN_FOREVER otherwise
    uint32 next_interrupt;  // Next tick time
    uint32 repeat;          // How many repeats
    struct timer_t* next;   // Next timer in list (chronologically ordered)
    char display;           // 'A' to 'Z' character for display
    void (*do_it)();        // Action to perform at tick
    void* parameter;        // Parameter to do_it: void* --> can be any object!
} timer_t;

extern void debug_timers();

extern void init_timers();
extern int at_least_one_timer();
extern void check_for_interrupt();
extern void wait_for_interrupt();
extern void loop_for_interrupts();
extern timer_t* insert_timer(uint32 delay, uint32 repeat, void (*do_something)(), void* do_it_paramter);

extern void sleep_time_units(uint32 delay);

#endif
