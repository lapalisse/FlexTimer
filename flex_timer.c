//
//  flextimer.c
//  TimeInterrupt
//
//  Created by Ludovic Bertsch on 19/06/2015.
//  Copyright (c) 2015 Ludovic Bertsch. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "flextimer.h"

//=========================== Start of configuration section ==================

// This part can be used to configure this timer library
//
// The big an interesting part is: you can choose the time units you
// want to use: milliseconds, microseconds, seconds, days, etc.
// This is done by rewriting the 3 following functions.

// Technical remarks:
//   - repeat = -1 => Loop forever
//   - repeat = 0  => It's a free cell!
//   - repeat = n  => repeat n times before removing!

/*
 * Generic version!
 */
//static
/*int FT_unsigned_compare_to(time_measure_t a, time_measure_t b) {
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        // a == b
        return 0;
    }
}*/

/*
 * Comparison of two integers, assuming they are close
 *
 * a and b MUST be unsigned!!!!
 */

//TODO static

int FT_compare_to(time_measure_t a, time_measure_t b) {
    time_measure_t delta;
    
    //printf("FT_compare_to(%u, %u)\n", a, b);
    
    if (a == b) {
        return 0;
    }
    
    delta = (b - a)&FT_TIME_MEASURE_COMPLETE_MASK;
    
    if (delta <= FT_TIME_MEASURE_HALF_MASK) {
        return -1;
    } else {
        return 1;
    }
}

/*
 * Simple conversion to string of characters, for basic display...
 *
 * If you don't understand how this functions, do not use it!
 */

#define FT_NB_SIMULTANEOUS_ITOA (10)

static const char* FT_time_measure_to_string(time_measure_t n) {
    static char t[FT_NB_SIMULTANEOUS_ITOA][30];
    static int i = 0;
    char* result;
    
    sprintf(t[i], "%u", (unsigned int)n);
    
    result = t[i];
    i = (i + 1)%FT_NB_SIMULTANEOUS_ITOA;
    
    return result;
}

#ifdef _FT_ARDUINO_MS
//
// Returns current time in time units
//
// You can choose the units
//
time_measure_t FT_get_time() {
    return getMillis();
}

//
// Have the processor (if possible) sleep for n units of time
//
void FT_sleep(time_measure_t m) {
    sleepMillis(m);
}
#endif

#ifdef _FT_ARDUINO_uS
//
// Returns current time in time units
//
// You can choose the units
//
time_measure_t FT_get_time() {
    return getMicros();
}

//
// Have the processor (if possible) sleep for n units of time
//
void FT_sleep(time_measure_t m) {
    sleepMillis(m/1000);
    sleepMicros(m%1000);
}
#endif


#ifdef _FT_NORMAL_MS
//
// Returns current time in time units
//
// You can choose the units
//
time_measure_t FT_get_time() {
    struct timeval t;
    
    gettimeofday(&t, NULL);
    
    return ((time_measure_t)t.tv_sec)*1000 + ((time_measure_t)t.tv_usec)/1000;
}

//
// Have the processor (if possible) sleep for n units of time
//
void FT_sleep(time_measure_t m) {
    usleep((time_measure_t)(m*1000));
}

#endif


#ifdef _FT_NORMAL_uS
//
// Returns current time in time units
//
// You can choose the units
//
time_measure_t FT_get_time() {
    struct timeval t;
    
    gettimeofday(&t, NULL);
    
    return ((time_measure_t)t.tv_sec)*1000000 + ((time_measure_t)t.tv_usec);
}

//
// Have the processor (if possible) sleep for n units of time
//
void FT_sleep(time_measure_t m) {
    usleep((time_measure_t)(m));
}

#endif


#ifdef _FT_NORMAL_SECOND
//
// Returns current time in time units
//
// You can choose the units
//
time_measure_t FT_get_time() {
    struct timeval t;
    
    gettimeofday(&t, NULL);
    
    return ((time_measure_t)t.tv_sec);
}

//
// Have the processor (if possible) sleep for n units of time
//
void FT_sleep(time_measure_t m) {
    sleep(m);
}

#endif


#ifdef _FT_EXPERIMENTAL
//
// Returns current time in time units
//
// You can choose the units
//
time_measure_t FT_get_time() {
    struct timeval t;
    
    gettimeofday(&t, NULL);
    
    return (((time_measure_t)t.tv_sec)*1000 + ((time_measure_t)t.tv_usec)/1000)&FT_TIME_MEASURE_COMPLETE_MASK;
}

//
// Have the processor (if possible) sleep for n units of time
//
void FT_sleep(time_measure_t m) {
    assert(m == (m&FT_TIME_MEASURE_COMPLETE_MASK));
    
    int err = usleep((time_measure_t)(m*1000));
    
    if (err != 0) {
        printf("USLEEP ERROR!!!\n");
    }
}

#endif

//=========================== End of configuration section ==================

/*
 * Force time to be limited by mask...
 */
time_measure_t FT_force_get_time() {
    return FT_get_time()&FT_TIME_MEASURE_COMPLETE_MASK;
}

/*
 * Don't do anything if zero, break the sleep in several pieces...
 */
static void FT_sleep_internal(time_measure_t some_time) {
    if (some_time > 0) {
        while (some_time > FT_LONGEST_SLEEP) {
            FT_sleep(FT_LONGEST_SLEEP);
            some_time -= FT_LONGEST_SLEEP;
        }
        
        if (some_time != 0) {
            FT_sleep(some_time);
        }
    }
}


/*
 * Equivalent of FT_sleep_units, but really really wait for the elapsed time to end.
 *
 * There seems to be a bug with Mac OS X: get_time may hang for some time, and then catch up,
 * so you don't want to do more than one sleep... Silly!
 */
void FT_force_sleep(time_measure_t some_time) {
    printf("FT_force_sleep(): started @ %u\n", (unsigned)FT_get_time());
    
    time_measure_t now, theoretical_end;
    
    theoretical_end = (FT_get_time() + some_time)&FT_TIME_MEASURE_COMPLETE_MASK;
    
    printf("FT_sleep: have to wait: %u\n", (unsigned) ((theoretical_end - FT_get_time())&FT_TIME_MEASURE_COMPLETE_MASK));

    printf("FT_sleep: avant: %u\n", (unsigned) (FT_get_time()));
    FT_sleep_internal(some_time);
    printf("FT_sleep: apres: %u\n", (unsigned) (FT_get_time()));

    now = FT_get_time();
    while (FT_compare_to(now, theoretical_end) < 0) {
        printf("FT_sleep: %u\n", (unsigned) ((theoretical_end - now)&FT_TIME_MEASURE_COMPLETE_MASK));
#       ifdef FT_TRUST_SLEEP
        FT_sleep_internal((theoretical_end - now)&FT_TIME_MEASURE_COMPLETE_MASK); // TODO Hesitation??????
#       endif
        now = FT_get_time();
    }
    
    printf("FT_force_sleep(): finished @ %u!\n", (unsigned) FT_get_time());
}

// First cel of timer chained list
static FT_timer_t* first_cell = NULL;

/*
 * Default action: displays
 */
void FT_do_tick(void* not_used_parameter, FT_timer_t* c) {
    printf("Tick %c @ %s\n", c->display, FT_time_measure_to_string(FT_force_get_time()));
}

/*
 * Not really useful... Or is it?
 */
void FT_do_nothing(void* not_used_parameter, FT_timer_t* c) {
    // Nothing!
}

/**
 * Comparison of two timers.
 *
 * Orders timers' ticks chronologically. If they fire at the
 * very same time, the most frequent one comes first.
 */
static int FT_timer_compare_to(FT_timer_t* a, FT_timer_t* b) {
    if (a->next_interrupt < b->next_interrupt) {
        // a will fire before b
        return -1;
    } else if (a->next_interrupt > b->next_interrupt) {
        // a will fire after b
        return 1;
    } else {
        // a should fire at same time than b
        // but most frequent one has priority!
        // Also, RUN_FOREVER WILL come before other values!
        return FT_compare_to(a->delay, b->delay);
    }
}

/************************************************************************/

/* CHAIN OF TIMERS MANAGEMENT */

/* This is a chained list, that is always ordered chronologically... */
 
/*
 * Returns next timer set to fire without removing it from list!
 *
 * If no timers are in the list, returns NULL.
 */
static FT_timer_t* FT_peek_timer() {
    return first_cell;
}

/*
 * Returns next timer set to fire and remove it from list!
 *
 * If no timers are in the list, returns NULL.
 */
static FT_timer_t* FT_pop_timer() {
    FT_timer_t* result;
    
    result = first_cell;
    if (first_cell != NULL) {
        first_cell = first_cell->next;
    }
    
    return result;
}

/*
 * Inserts parameter timer at its place in time!
 *
 * This function assumes the parameter is not NULL!
 */
static void FT_push_timer(FT_timer_t* cel) {
    FT_timer_t** c;
    
    assert(cel != NULL);
    
    c = &first_cell;
    
    while ((*c) != NULL && FT_timer_compare_to(cel, (*c)) > 0) {
        c = &((*c)->next);
    }
    
    cel->next = (*c);
    (*c) = cel;
}

/*
 * Checks if there is at least one timer!
 */
int FT_at_least_one_timer() {
    return first_cell != NULL;
}

/************************************************************************/

/* MEMORY ALLOCATION */

#ifdef _FT_SIMPLE_TIMER_ALLOCATION

// A simple scheme based on an array

static FT_timer_t timers[FT_NB_MAX_TIMERS];
static int current_timer = 0;

// Initializes the timers to an inactive state!
//
//This may not be needed if your computer starts with memory containing zeros...
void FT_init_timer_alloc() {
    int i;

    // Force timers to inactive state!
    // This may not be needed, depending on the platform you're using!!!
    
    for (i = 0; i < FT_NB_MAX_TIMERS; i++) {
        timers[i].delay = 0; // This is how we know a cell is free!
    }
    
    current_timer = 0;
    
    first_cell = NULL;
}

//
//Frees a timer!
//
static void FT_free_timer(FT_timer_t* c) {
    assert(c != NULL);
    
    c->delay = 0; // Mandatory: that's how we know a slot if available!
    
    // Non mandatory: but safer(?)
    c->repeat = 0;
    c->next = NULL;
    c->next_interrupt = 0;
    c->display = '/';
    c->do_it = NULL;
}

//
//Allocates a timer one way or the other...
//
static FT_timer_t* FT_new_timer() {
    int i;
    FT_timer_t* result;
    
    // If limited memory handling: using delay = 0 for free slots!
    i = 0;
    while (i < FT_NB_MAX_TIMERS && timers[(current_timer + i)%FT_NB_MAX_TIMERS].delay != 0) {
        i++;
    }
    
    if (i == FT_NB_MAX_TIMERS) {
        // No slot available!
        return NULL;
    } else {
        // Found a slot, returning it after
        result = &timers[(current_timer + i)%FT_NB_MAX_TIMERS];
        current_timer = (current_timer + i + 1)%FT_NB_MAX_TIMERS;
        return result;
    }
}

#endif

#ifdef _FT_TIMER_MALLOC

// Another scheme based on Unix-style memory allocation

void FT_init_timer_alloc() {
    first_cel = NULL;
}

static void FT_free_timer(FT_timer_t *c) {
    free(c);
}

static FT_timer_t* FT_new_timer() {
    FT_timer_t* result = (FT_timer_t*)malloc(sizeof(FT_timer_t));
    
    return result;
}

#endif

/************************************************************************/

void FT_init_timers() {
    FT_init_timer_alloc();
}

/*
 * Will fire every timer that needs to, in order.
 *
 * There needs to be at least one timer ready.
 */
static void FT_do_interrupt() {
    time_measure_t now;
    FT_timer_t *c;
    
    // There is at least one timer ready to fire!
    assert(FT_at_least_one_timer());
    //printf("FT_do_interrupt()\n");
    
    // Checks the first timer: it will fired, but also every timer
    // which is in time!
    c = FT_peek_timer();
    now = c->next_interrupt;
    
    while (c != NULL
           //&& (FT_compare_to(c->next_interrupt, previous_interrupt) > 0 && FT_compare_to(c->next_interrupt, now) <= 0)) {
           //correct: && c->next_interrupt == now) {
           && FT_compare_to(c->next_interrupt, now) == 0) {

        c = FT_pop_timer();
        
        // Execute the command doing the fire
        if (c->do_it != NULL) {
            c->do_it(c->parameter, c);
        }
        
        if (c->repeat >= 1 || c->repeat == FT_RUN_FOREVER) {
            c->next_interrupt = (c->next_interrupt + c->delay)&FT_TIME_MEASURE_COMPLETE_MASK;
            //correct: c->next_interrupt = c->next_interrupt + c->delay;
            if (c->repeat > 1 || c->repeat == FT_RUN_FOREVER) {
                // If timer needs to fire again, we put it in the list again!
                FT_push_timer(c);
            }
            // One less firing to be done
            if (c->repeat != FT_RUN_FOREVER) {
                c->repeat--;
            }
        } else {
            FT_free_timer(c);
        }
        
        c = FT_peek_timer();
    }
}

/*
 * Checks and fire if some timers needs to fire.
 * Never waits!
 *
 * This is the instruction you want to use everywhere in your code!
 */
void FT_check_and_do() {
    if (FT_at_least_one_timer()) {
        time_measure_t now = FT_force_get_time();
        
        //if (FT_compare_to(first_cel->next_interrupt, previous_interrupt) > 0 && FT_compare_to(first_cel->next_interrupt, now) <= 0) {
        //correct: if (first_cel->next_interrupt <= now) {
        if (FT_compare_to(first_cell->next_interrupt, now) <= 0) {
            FT_do_interrupt();
        }
    }
}

/*
 * Checks and fire if some timers needs to fire.
 * May wait if needed!
 *
 * This is the instruction you want to use in your main loop!
 */
void FT_sleep_and_do() {
    if (FT_at_least_one_timer()) {
        time_measure_t now = FT_force_get_time();
        time_measure_t delay = (first_cell->next_interrupt - now)&FT_TIME_MEASURE_COMPLETE_MASK;
        
        if (delay != 0 && ((delay&FT_TIME_MEASURE_HALF_MASK) == delay)) {
            FT_sleep_internal(delay);
        }
        
        //if (FT_compare_to(first_cel->next_interrupt, previous_interrupt) > 0 && FT_compare_to(first_cel->next_interrupt, now) <= 0) {
        //correct: if (first_cel->next_interrupt <= now) {
        if (FT_compare_to(first_cell->next_interrupt, now) <= 0) {
            FT_do_interrupt();
        }
    }
}

/*
 * Checks and fire if some timers needs to fire.
 * May wait if needed!
 *
 * This is the instruction you want to use in your main loop!
 */
void FT_force_sleep_and_do() {
    if (FT_at_least_one_timer()) {
        time_measure_t delay = first_cell->next_interrupt - FT_get_time();
        
        if (delay > 0 && (delay <= FT_TIME_MEASURE_HALF_MASK)) {
            FT_force_sleep(delay);
        }
        
        FT_do_interrupt();
    }
}

/*
 * Loops until there are no timers in list.
 * Will wait between ticks with wait instructions!
 *
 * This could be a main loop!
 */
void FT_loop_for_interrupts() {
    while (FT_at_least_one_timer()) {
        FT_sleep_and_do();
    }
}

/*
 * Inserts a timer in the list with your parameters...
 */
FT_timer_t* FT_insert_timer(time_measure_t delay, int repeat, void (*do_something)(), void* do_it_parameter) {
    FT_timer_t *c;
    static char display = 'A';
    
    assert(delay > 0 && delay <= FT_TIME_MEASURE_QUARTER_MASK); // You have to set a limit and HALF is risky!
    assert(repeat > 0 || repeat == FT_RUN_FOREVER);
    
    c = FT_new_timer();
    c->delay = delay;
    c->repeat = repeat;
    c->display = display;
    c->parameter = do_it_parameter;
    if (do_something != NULL) {
        c->do_it = do_something;
    } else {
        c->do_it = &FT_do_tick;
    }

    // Debug stuff: every timer is associated to a letter!
    display++;
    if (display > 'Z') display = 'A';
    
    // The following instructions is to be done just before returning!
    // The idea here is to have the timer start as late as we can
    c->next_interrupt = FT_get_time();
    FT_push_timer(c);
    
    return c;
}

/*
 * Recursive part of debug_timers!
 */
static void FT_debug_timer(FT_timer_t* c) {
    if (c == NULL) {
        printf("[NULL]\n");
    } else {
        printf("[%c: delay = %s, repeat = %s, next_interrupt = %s]\n",
               c->display,
               FT_time_measure_to_string(c->delay),
               (c->repeat == FT_RUN_FOREVER)?"RUN_FOREVER":FT_time_measure_to_string(c->repeat),
               FT_time_measure_to_string(c->next_interrupt));
        FT_debug_timer(c->next);
    }
}

/*
 * Simple display of timer list for debug!
 */
void FT_debug_timers() {
    printf("========== Chain of timers is as follows @ %s\n", FT_time_measure_to_string(FT_get_time()));
    FT_debug_timer(first_cell);
    printf("========== Chain of timers - the end\n");
}

/*
static void FT_int_desynchronize(int* *integer, int nb_integers, int delta[]) {
    // Who wants to code it?
    // Or simply use randomize!!!
}

void FT_desynchronize(int* *timer_t, int nb_timers) {
    // Who wants to code it?
    // Or simply use randomize!!!
}
*/

/*
 * Program the timer to start randomly instead of asap.
 *
 * Useful when you want the timer to not fire at the same time!
 *
 * You MUST call this before starting any loop.
 */
void FT_randomize_timer(FT_timer_t* timer) {
    //timer->next_interrupt += random(timer->delay);
    timer->next_interrupt += (((long)rand())*timer->delay)/RAND_MAX;
}

/*
 * Program all timers to start randomly instead of asap.
 *
 * Useful when you want all timers to not fire at the same time!
 *
 * You MUST call this before starting any loop.
 */
void FT_randomize_all_timers() {
    FT_timer_t *current;
    
    current = first_cell;
    while (current != NULL) {
        FT_randomize_timer(current);
        current = current->next;
    }
}
