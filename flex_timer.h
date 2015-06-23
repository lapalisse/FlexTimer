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

// Super important value: size of timer array (if using limited memory allocation scheme)
#define FT_NB_MAX_TIMERS (10)


//////////////////////////////////
// Configuration section!
//////////////////////////////////

//#define _FT_ARDUINO_MS
//#define _FT_ARDUINO_uS
//#define _FT_NORMAL_SECOND
#define _FT_NORMAL_MS
//#define _FT_NORMAL_uS
//#define _FT_EXPERIMENTAL


#ifdef _FT_ARDUINO_MS
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (INT_MAX)
#define FT_TIME_MEASURE_HALF_MASK (INT_MAX/2)
#define FT_ONE_SECOND (1000)
#endif

#ifdef _FT_ARDUINO_uS
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (INT_MAX)
#define FT_TIME_MEASURE_HALF_MASK (INT_MAX/2)
#define FT_ONE_SECOND (1000000)
#endif

#ifdef _FT_NORMAL_SECOND
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (INT_MAX)
#define FT_TIME_MEASURE_HALF_MASK (INT_MAX/2)
#define FT_ONE_SECOND (1)
#endif

#ifdef _FT_NORMAL_MS
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (INT_MAX)
#define FT_TIME_MEASURE_HALF_MASK (INT_MAX/2)
#define FT_ONE_SECOND (1000)
#endif

#ifdef _FT_NORMAL_uS
typedef unsigned int time_measure_t; // 32 bits!
#define FT_TIME_MEASURE_COMPLETE_MASK (INT_MAX)
#define FT_TIME_MEASURE_HALF_MASK (INT_MAX/2)
#define FT_ONE_SECOND (1000000)
#endif

#ifdef _FT_EXPERIMENTAL
// 11 bit depth for testing to the limits!
typedef unsigned short time_measure_t;
//#define FT_TIME_MEASURE_COMPLETE_MASK (2047)
//#define FT_TIME_MEASURE_HALF_MASK (1023)
#define FT_TIME_MEASURE_COMPLETE_MASK (8191)
#define FT_TIME_MEASURE_HALF_MASK (4095)
#define FT_ONE_SECOND (1000)
#endif
//////////////////////////////////

#define _FT_SIMPLE_TIMER_ALLOCATION
//#define _FT_SIMPLE_TIMER_WITH_MALLOC : NOT IMPLEMENTED!!!
//#define _FT_TIMER_MALLOC

/////////////////////////////////

#define FT_ONE_MINUTE ((FT_ONE_SECOND)*60)
#define FT_ONE_HOUR   ((FT_ONE_SECOND)*60*60)
#define FT_ONE_DAY    ((FT_ONE_SECOND)*60*60*24)
#define FT_ONE_WEEK   ((FT_ONE_SECOND)*60*60*24*7)

typedef struct FT_timer_t {
    time_measure_t delay;           // Delay between 2 ticks! RUN_FOREVER otherwise
    time_measure_t next_interrupt;  // Next tick time
    int repeat;                     // How many repeats
    struct FT_timer_t* next;        // Next timer in list (chronologically ordered)
    char display;           // 'A' to 'Z' character for display/debug
    void (*do_it)();        // Action to perform at tick
    void* parameter;        // Parameter to do_it: void* --> can be a pointer to any object!
} FT_timer_t;

// Display all timers in a reader friendly way
extern void FT_debug_timers();

// Published interface:
extern void FT_init_timers();
extern int FT_at_least_one_timer();
extern void FT_check_for_interrupt();
extern void FT_wait_for_interrupt();
extern void FT_loop_for_interrupts();
extern FT_timer_t* FT_insert_timer(time_measure_t delay, int repeat, void (*do_something)(), void* do_it_paramter);

// Examples/tools
extern void do_tick(void* param, FT_timer_t* timer);
extern void do_nothing(void* param, FT_timer_t* timer);

// Crazy useful: force timers that may be in synchrony to fire at different times!
extern void FT_desynchronize_timers(time_t* timer[], int nb_timers);

// Crazy useful: desynchronize with a simpler algorithm (result not garanteed but likely!)
extern void FT_randomize_all_timers();
extern void FT_randomize_timer(FT_timer_t* timer);

// Maybe it's better to use these than the direct ones?
extern time_measure_t FT_get_time_units();
extern void FT_sleep_time_units(time_measure_t delay);
extern void FT_force_sleep_time_units(time_measure_t delay);

#endif

