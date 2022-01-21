/*
 * 
 * CAFETIME
 * A programmable electric outlet to power the coffee machine on in the morning
 * 
 * maurogull
 * 
 */



#include <Arduino.h>

#include "config.h"
#include "shared.h"

#include "DebugConsole.h"
#include "Relay.h"

#include "TaskBlink.h"
#include "TaskTester.h"
#include "TaskNetworkTime.h"
#include "TaskInternalTime.h"
#include "TaskDisplay.h"
#include "TaskStateMachine.h"
#include "TaskWebServer.h"


void setup() {

    xTimeSemaphore = xSemaphoreCreateMutex();
    xScheduleSemaphore = xSemaphoreCreateMutex();

    xCommandQueue = xQueueCreate( 20, sizeof( int32_t ) );


    HTTPPassCode = buildRandomPassCode();


    scheduledHour = 6;
    scheduledMinute = 55;
    currentState = CAFE_IDLE;


    //xTaskCreate(TaskBlink, "TBlink", 1024, NULL, 1, NULL);

    //xTaskCreate(TaskTester, "TTester", 2048, NULL, 1, NULL);

    xTaskCreate(TaskNetworkTime, "TNetTime", 8192, NULL, 1, NULL);

    xTaskCreate(TaskInternalTime, "TInternalTime", 2048, NULL, 1, NULL);

    xTaskCreate(TaskDisplay, "TDisplay", 2048, NULL, 10, NULL);

    xTaskCreate(TaskStateMachine, "TStateMachine", 4096, NULL, 1, NULL);

    xTaskCreate(TaskWebServer, "TWebServer", 8192, NULL, 1, NULL);
}


void loop() { }

