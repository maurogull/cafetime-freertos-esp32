#include <Arduino.h>
#include "config.h"

void TaskBlink(void * pvParam) {

    const int ledPin = RELAY_PIN; 
    pinMode(ledPin, OUTPUT);
    bool led_status = false;

    while(1) {
        digitalWrite(ledPin, led_status);
        led_status = !led_status;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}