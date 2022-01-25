#include <Arduino.h>
#include "config.h"
#include "shared.h"


void TaskStateMachine(void * pvParam) {

    CafeState prevState = CAFE_IDLE;
    vTaskDelay(pdMS_TO_TICKS(9e3));


    TickType_t xTickStartMakingCoffee = 0, xTickStartWarmingUp = 0;

    while(1) {

        /* state transitions because of commands */
        int32_t queueData, queueData2;
        if( xQueueReceive( xCommandQueue, &queueData, 20 ) ) {
        
            if(queueData == COMMAND_FORCE_ON) {
                PowerRelay.on();
                currentState = CAFE_FORCED_ON;
                Console.print("State changed to FORCED_ON");
            }

            if(queueData == COMMAND_MAKE_IDLE) {
                PowerRelay.off();
                currentState = CAFE_IDLE;
                Console.print("State changed to IDLE");
            }

            if(queueData == COMMAND_SCHEDULE) {
                PowerRelay.off();
                xQueueReceive( xCommandQueue, &queueData, 20 );
                xQueueReceive( xCommandQueue, &queueData2, 20 );

                while(! xSemaphoreTake(xScheduleSemaphore, 10) ) vTaskDelay(10);
                
                scheduledHour = queueData;
                scheduledMinute = queueData2;
                currentState = CAFE_SCHEDULED;

                xSemaphoreGive(xScheduleSemaphore);     

                Console.print("State changed to SCHEDULED");
            }
        
        }


        /* state transitions because of time ellapsing */

        while(! xSemaphoreTake(xScheduleSemaphore, 10) ) vTaskDelay(10);
        while(! xSemaphoreTake(xTimeSemaphore, 10) ) vTaskDelay(10);
        if(currentState == CAFE_SCHEDULED && horas == scheduledHour && minutos == scheduledMinute) {  //lets make coffee !!  :D
            xSemaphoreGive(xScheduleSemaphore); 
            xSemaphoreGive(xTimeSemaphore); 

            PowerRelay.on();
            currentState = CAFE_MAKING;
            xTickStartMakingCoffee = xTaskGetTickCount();

            Console.print("State changed to MAKING");
        }
        else {
            xSemaphoreGive(xScheduleSemaphore); 
            xSemaphoreGive(xTimeSemaphore);
        }


        TickType_t ahora = xTaskGetTickCount();
        int timeElapsed = (ahora - xTickStartMakingCoffee) / configTICK_RATE_HZ ; //sec
        if(currentState == CAFE_MAKING && timeElapsed > (MINUTES_HOLD_MAKING_STATE * 60)) {   
            currentState = CAFE_KEEP_WARM;
            xTickStartWarmingUp = xTaskGetTickCount();

            Console.print("State changed to KEEP_WARM");
        }

        timeElapsed = (ahora - xTickStartWarmingUp) / configTICK_RATE_HZ ; //sec
        if(currentState == CAFE_KEEP_WARM && timeElapsed > (MINUTES_HOLD_KEEP_WARM_STATE * 60)) {   
            currentState = CAFE_IDLE;
            PowerRelay.off();

            Console.print("State changed to IDLE");
        }

        
        //notify change (webserver is awaiting for this)
        if(currentState != prevState) { 
            xSemaphoreGive(xStateChangedSemaphore);
        }

        vTaskDelay(pdMS_TO_TICKS(600));
        
    }

}