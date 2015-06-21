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

//typedef uint32 time_t;

/*
 * Returns current time in time units
 *
 * You can choose the units
 */
static uint32 FT_get_time_units() {
    struct timeval t;
    
    gettimeofday(&t, NULL);
    
    return ((uint32)t.tv_sec)*1000 + ((uint32)t.tv_usec)/1000;
}

/*
 * Have the processor (if possible) sleep for n units of time
 */
void FT_sleep_time_units(uint32 m) {
    usleep((uint32)(m*1000));
}

/*
 * Simple conversion to string of characters, for basic display...
 */
static const char* FT_uint32_to_string(uint32 n) {
    static char result[30];
    
    sprintf(result, "%ld", n);
    
    return result;
}

//=========================== End of configuration section ==================

#include "time_millis.c"

// First cel of timer chained list
static FT_timer_t* first_cel = NULL;

/*
 * Default action: displays
 */
void FT_tick(void* not_used_parameter, FT_timer_t* c) {
    printf("%c @ %s\n", c->display, FT_uint32_to_string(FT_get_time_units()));
}

/**
 * Comparison of two integers
 *
 * This needs to be improved so it can handle overflow values!
 * Should favour shortest distance between numbers!
 */
static int FT_compare_to(int32 a, int32 b) {
    //TODO
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        // a == b
        return 0;
    }
}

/*
 * Comparison of two integers, assuming they are close
 *
 * a and b MUST be unsigned!!!!
 */
static int FT_proxy_compare_to(uint32 a, uint32 b) {
    if (a == b) {
        return 0;
    } else if (a < b) {
        if ((b - a) <= INT_MAX) {
            return 1;
        } else {
            return -1;
        }
    } else {
        // a > b
        if ((a - b) <= INT_MAX) {
            return -1;
        } else {
            return 1;
        }
    }
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
        // but most frequent one has priority
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
    return first_cel;
}

/*
 * Returns next timer set to fire and remove it from list!
 *
 * If no timers are in the list, returns NULL.
 */
static FT_timer_t* FT_pop_timer() {
    FT_timer_t* result;
    
    result = first_cel;
    if (first_cel != NULL) {
        first_cel = first_cel->next;
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
    
    c = &first_cel;
    
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
    return first_cel != NULL;
}

/************************************************************************/

/* MEMORY ALLOCATION */


// A simple scheme based on an array

static FT_timer_t timers[FT_NB_MAX_TIMERS];
static int current_timer = 0;

// Initializes the timers to an inactive state!
//
//This may not be needed if your computer starts with memory containing zeros...
void FT_init_timers() {
    int i;

    // Force timers to inactive state!
    // This may not be needed, depending on the platform you're using!!!
    
    for (i = 0; i < FT_NB_MAX_TIMERS; i++) {
        timers[i].delay = 0; // This is how we know a cell is free!
    }
    
    current_timer = 0;
    
    first_cel = NULL;
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


 /*
// Another scheme based on Unix-style memory allocation

void init_timers() {
    first_cel = NULL;
}

static void free_timer(timer_t *c) {
    free(c);
}

static timer_t* new_timer() {
    return (timer_t*)malloc(sizeof(timer_t));
}
*/

/************************************************************************/

/*
 * Will fire every timer that needs to, in order.
 *
 * There needs to be at least one timer ready.
 */
static void FT_do_interrupt() {
    uint32 now;
    FT_timer_t *c;
    
    // There is at least one timer ready to fire!
    assert(FT_at_least_one_timer());
    
    // Checks the first timer: it will fired, but also every timer
    // which is in time!
    c = FT_peek_timer();
    now = c->next_interrupt;
    
    while (c != NULL && c->next_interrupt <= now) {
        c = FT_pop_timer();
        
        // Execute the command doing the fire
        if (c->do_it != NULL) {
            c->do_it(c->parameter, c);
        } else {
            // Default fire function
            FT_tick(c->parameter, c);
        }
        
        if (c->repeat >= 1 || c->repeat == FT_RUN_FOREVER) {
            c->next_interrupt = c->next_interrupt + c->delay;
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
void FT_check_for_interrupt() {
    if (FT_at_least_one_timer()) {
        time_t now = FT_get_time_units();
        
        if (first_cel->next_interrupt < now) {
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
void FT_wait_for_interrupt() {
    if (FT_at_least_one_timer()) {
        uint32 delay = first_cel->next_interrupt - FT_get_time_units();
        
        if (delay > 0) {
            FT_sleep_time_units(delay);
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
        FT_wait_for_interrupt();
    }
}

/*
 * Inserts a timer in the list with your parameters...
 */
FT_timer_t* FT_insert_timer(uint32 delay, uint32 repeat, void (*do_something)(), void* do_it_parameter) {
    FT_timer_t *c;
    static char display = 'A';
    
    assert(delay > 0);
    assert(delay < DELAY_MAX_VALUE);
    assert(repeat > 0 || repeat == FT_RUN_FOREVER);
    
    c = FT_new_timer();
    c->delay = delay;
    c->repeat = repeat;
    c->display = display;
    c->do_it = do_something;
    c->parameter = do_it_parameter;

    // Debug
    display++;
    if (display > 'Z') display = 'A';
    
    // The following instructions is to be done just before returning!
    // The idea here is to have the timer start as late as we can
    c->next_interrupt = FT_get_time_units();
    FT_push_timer(c);
    
    return c;
}

/*
 * Recursive part of debug_timers!
 */
static void FT_debug_timer(FT_timer_t* c) {
    if (c == NULL) {
        printf("NULL\n");
    } else {
        printf("[%c: delay = %s, repeat = %s, next_interrupt = %s]\n", c->display, FT_uint32_to_string(c->delay), FT_uint32_to_string(c->repeat), FT_uint32_to_string(c->next_interrupt));
        FT_debug_timer(c->next);
    }
}

/*
 * Simple display of timer list for debug!
 */
void FT_debug_timers() {
    printf("========== Chain of timers is as follows @ %s\n", FT_uint32_to_string(FT_get_time_units()));
    FT_debug_timer(first_cel);
}

