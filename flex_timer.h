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
    FT_CATCH_UP = 0,
    FT_DO_NOT_CATCH_UP = 1,
    FT_REMOVE_WHEN_FINISHED = 4,
    FT_DEACTIVATE_WHEN_FINISHED = 8
} FT_timer_option_t;

// Not really used
#define FT_DEFAULT_PARAMETERS (CATCH_UP|REMOVE_WHEN_FINISHED)

// Super important value: to be used for "repeat" parameter!
#define FT_RUN_FOREVER (-1)

// Super important value: size of timer array (limited memory allocation scheme)
#define FT_NB_MAX_TIMERS (10)

typedef long int32;
typedef unsigned long uint32;

// This could be important: we want some constraints here!
// INT_MAX would be in theory the best, but... if we miss it, we're screwed!
#define DELAY_MAX_VALUE (INT_MAX/2)

typedef struct FT_timer_t {
    uint32 delay;           // Delay between 2 ticks! RUN_FOREVER otherwise
    uint32 next_interrupt;  // Next tick time
    uint32 repeat;          // How many repeats
    struct FT_timer_t* next;   // Next timer in list (chronologically ordered)
    char display;           // 'A' to 'Z' character for display
    void (*do_it)();        // Action to perform at tick
    void* parameter;        // Parameter to do_it: void* --> can be any object!
} FT_timer_t;

extern void FT_debug_timers();

extern void FT_init_timers();
extern int FT_at_least_one_timer();
extern void FT_check_for_interrupt();
extern void FT_wait_for_interrupt();
extern void FT_loop_for_interrupts();
extern FT_timer_t* FT_insert_timer(uint32 delay, uint32 repeat, void (*do_something)(), void* do_it_paramter);

extern void FT_sleep_time_units(uint32 delay);

#endif
