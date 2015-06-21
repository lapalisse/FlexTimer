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

void display_string(void* param) {
    printf("%s\n", (char*)param);
}

void setup() {
    FT_init_timers();
}

void loop() {
    //FT_check_for_interrupt();
    //FT_sleep_time_units(1);
    
    FT_wait_for_interrupt();
}

int main(int argc, const char * argv[]) {
    time_t t1, t2;
    
    setup();
    
    // insert code here...
    printf("Testing timers...\n");
    
    
    //FT_insert_timer(1000000, 2, event1, NULL);
    //FT_insert_timer(500000, 4, NULL, NULL);
    //FT_insert_timer(250000, 8, NULL, NULL);
    //FT_insert_timer(1000000, FT_RUN_FOREVER, NULL, NULL);
    
    FT_insert_timer(FT_ONE_SECOND, 2, &display_string, "Kaboum!");
    FT_insert_timer(FT_ONE_SECOND/2, 4, &display_string, "Krouzma!");
    FT_insert_timer(FT_ONE_SECOND/4, 8, NULL, NULL);
    FT_insert_timer(FT_ONE_SECOND, FT_RUN_FOREVER, NULL, NULL);
    
    //FT_insert_timer(1, 2, event1, NULL);
    //FT_insert_timer(5, 4, NULL, NULL);
    //FT_insert_timer(25, 8, NULL, NULL);
    //FT_insert_timer(1, FT_RUN_FOREVER, NULL, NULL);
    
    FT_debug_timers();
    
    time(&t1);
    while (FT_at_least_one_timer()) {
        loop();
        
        /*time(&t2);
        if (t2 - t1 > 5) {
            break;
        }*/
    }
    
    printf("Finished!\n");
    
    return 0;
}

