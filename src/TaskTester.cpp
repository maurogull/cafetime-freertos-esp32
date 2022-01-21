#include <Arduino.h>
#include "config.h"
#include "shared.h"

void TaskTester(void * pvParam) {

int32_t sentData;

vTaskDelay(pdMS_TO_TICKS(10e3));

while(1) {

    //schedule cafe to 10:15
    sentData = COMMAND_SCHEDULE;
    xQueueSend( xCommandQueue, (void *) &sentData, 20 );
    sentData = 7;
    xQueueSend( xCommandQueue, (void *) &sentData, 20 );
    sentData = 50;
    xQueueSend( xCommandQueue, (void *) &sentData, 20 );
    Console.print("Sent dummy schedule");
    vTaskDelay(pdMS_TO_TICKS(10000));

    
    //cancel schedule
    sentData = COMMAND_MAKE_IDLE;
    xQueueSend( xCommandQueue, (void *) &sentData, 20 );
    Console.print("Sent dummy make idle");
    vTaskDelay(pdMS_TO_TICKS(5000));


    //force power on
    sentData = COMMAND_FORCE_ON;
    xQueueSend( xCommandQueue, (void *) &sentData, 20 );
    Console.print("Sent dummy force on");
    vTaskDelay(pdMS_TO_TICKS(5000));


    //go idle again
    sentData = COMMAND_MAKE_IDLE;
    xQueueSend( xCommandQueue, (void *) &sentData, 20 );
    Console.print("Sent dummy make idle");
    vTaskDelay(pdMS_TO_TICKS(5000));
    
}

}