//
//  main.c
//  TimeInterrupt
//
//  Created by Ludovic Bertsch on 18/06/2015.
//  Copyright (c) 2015 Ludovic Bertsch. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "flextimer.h"

void event1(void* param) {
    printf("Kaboum!\n");
}

void setup() {
    init_timers();
}

int main(int argc, const char * argv[]) {
    time_t t1, t2;
    
    setup();
    
    // insert code here...
    printf("Testing timers...\n");
    
    
    insert_timer(1000, 2, event1, NULL);
    insert_timer(500, 4, NULL, NULL);
    insert_timer(250, 8, NULL, NULL);
    insert_timer(1000, RUN_FOREVER, NULL, NULL);
    
    
    debug_timers();
    
    time(&t1);
    while (at_least_one_timer()) {
        check_for_interrupt();
        sleep_time_units(1);
        
        /*time(&t2);
        if (t2 - t1 > 5) {
            break;
        }*/
    }
    
    return 0;
}
