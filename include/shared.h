#pragma once

#include <Arduino.h>
#include "CafeState.h"
#include "Relay.h"
#include "DebugConsole.h"


extern DebugConsole Console;


// ---- semaphores and general timing global variables

extern SemaphoreHandle_t xTimeSemaphore;
extern int horas, minutos, segundos;

extern SemaphoreHandle_t xScheduleSemaphore;
extern int scheduledHour, scheduledMinute;

extern int timeIsReliable;
extern int internalUpdates;


extern SemaphoreHandle_t xStateChangedSemaphore;


// ---- states

extern CafeState currentState;


// ---- command queue for the state machine

#define COMMAND_FORCE_ON 1
#define COMMAND_MAKE_IDLE 2
#define COMMAND_SCHEDULE 3
extern QueueHandle_t xCommandQueue;


// ---- relay object (handles GPIO)

extern Relay PowerRelay;


// ---- passcode used when accessing via http requests

extern String HTTPPassCode;

String buildRandomPassCode();