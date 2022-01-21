#include <Arduino.h>
#include "config.h"
#include "shared.h"

#include <WiFi.h>
#include "NTPClient.h"
#include <WiFiUdp.h>

void TaskNetworkTime(void * pvParam) {

    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "pool.ntp.org", UCT_OFFSET_SECONDS);

    int waitingPeriods = 0;

    while(1) {
        if(WiFi.status() != WL_CONNECTED) {
            Console.print("Wifi is down");
            waitingPeriods = 0;
            
            WiFi.mode(WIFI_STA);
            vTaskDelay(pdMS_TO_TICKS(500));
            WiFi.begin(WIFI_SSID, WIFI_PASSWD);
            
            while (WiFi.status() != WL_CONNECTED)  { 
                Console.print("Wifi waiting..."); 

                waitingPeriods++;

                if(waitingPeriods >= 10) {
                    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
                    waitingPeriods=0;
                }
                
                vTaskDelay(pdMS_TO_TICKS(1000));  
            }
            
            Console.print("Wifi ok");
            Console.print(WiFi.localIP().toString());
        }

        timeClient.begin();

        while(!timeClient.update()) { vTaskDelay(pdMS_TO_TICKS(2000)); } 

        if( xSemaphoreTake(xTimeSemaphore, 10) == pdTRUE ) {
            horas = timeClient.getHours();
            minutos = timeClient.getMinutes();
            segundos = timeClient.getSeconds();
            timeIsReliable = 1;
            internalUpdates = 0;

            xSemaphoreGive(xTimeSemaphore);
            Console.print("Updated to " + String(horas) + ":" + String(minutos) + ":" + String(segundos));
        } 
        else {
            Console.print("TimeSemaphore not ready");
        }

        vTaskDelay(pdMS_TO_TICKS(900000)); //15 minutes = 900000 msec
    }
    
} 