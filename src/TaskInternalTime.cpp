
#include <Arduino.h>
#include "config.h"
#include "shared.h"


void TaskInternalTime(void * pvParam) {

    while(1) {
        
        while(!timeIsReliable) {
            Console.print("Time is not reliable"); 
            vTaskDelay(pdMS_TO_TICKS(2000));  
        }

        TickType_t xLastUpdate;
        xLastUpdate = xTaskGetTickCount();

        if( xSemaphoreTake(xTimeSemaphore, 10) == pdTRUE ) { 
            segundos += 10;
            
            if(segundos >= 60) { segundos -= 60; minutos++; }
            if(minutos >= 60) { minutos = 0; horas++; }
            if(horas >= 24) { horas=0; }

            internalUpdates++;
                
            Console.print("Updated to " + String(horas) + ":" + String(minutos) + ":" + String(segundos) + " internal " + String(internalUpdates));

            if(internalUpdates > 1080) timeIsReliable = 0;  //10800 sec = 3 hr      ... and internalUpdates counts 10 sec periods
            
            xSemaphoreGive(xTimeSemaphore);
        }
        else {
            Console.print("TimeSemaphore not ready");
        }
        
        vTaskDelayUntil( &xLastUpdate, pdMS_TO_TICKS(10000) );
        
    }
}
