#include <arduino.h>

int ledState = LOW;

void blink() {
    if (ledState == LOW) {
        ledState = HIGH;
    } else {
        ledState = LOW;
    }
    digitalWrite(13, ledState);
}

void setup() {
    FT_init();

    FT_insert_timer(1000, FT_RUN_FOREVER, &blink, NULL);
}

void loop() {
    FT_wait_and_do();
}
