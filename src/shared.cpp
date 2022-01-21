#include <Arduino.h>
#include "config.h"

#include "DebugConsole.h"
#include "Relay.h"
#include "CafeState.h"


DebugConsole Console;



// ---- semaphores (mutexes) and general timing global variables

SemaphoreHandle_t xTimeSemaphore = NULL;
int horas = 0, minutos = 0, segundos = 0;

SemaphoreHandle_t xScheduleSemaphore = NULL;
int scheduledHour = -1, scheduledMinute = -1;

int timeIsReliable = 0;
int internalUpdates = 0;



// ---- states

CafeState currentState = CAFE_IDLE;



// ---- command queue for the state machine

QueueHandle_t xCommandQueue;



// ---- relay object (handles GPIO)

Relay PowerRelay;



// ---- passcode used when accessing via http requests

String HTTPPassCode;

String buildRandomPassCode() {
    char letters[] = { 'A', 'A', 'A', 'A', '\0'};
    int i;


    for(i=0; i<4; i++) {
        if(esp_random() % 2) letters[i] = 48 + (esp_random() % 10);  //a number
        else letters[i] = 97 + (esp_random() % 26);  //a letter
    }

    return String(letters);
}