#include <Arduino.h>
#include "config.h"
#include "shared.h"

#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>


Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

void TaskDisplay(void * pvParam) {

    display.begin(SH1106_SWITCHCAPVCC, 0x3c);  //address
    vTaskDelay(pdMS_TO_TICKS(100));

    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);          
    display.setTextColor(WHITE);
    display.display();
    vTaskDelay(pdMS_TO_TICKS(100));

    display.print("Hola :)");
    display.setCursor(0,30);
    display.setTextSize(1);
    display.print("CafeTime ");
    display.write(3);
    display.print(" maurogull");
    display.display();

    vTaskDelay(pdMS_TO_TICKS(2000));

    int lastMinuteShown = -1;

    while(1) {

        while(! xSemaphoreTake(xTimeSemaphore, 10) ) vTaskDelay(10); 

        if(!timeIsReliable) {
            xSemaphoreGive(xTimeSemaphore);

            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.print("Tiempo no disponible aun, espera...");
        }
        else {

            // ---- big hour : minute


            if(lastMinuteShown != minutos) {
                display.clearDisplay();
                display.setCursor(18,0); 
                display.setTextSize(3);
                display.setTextColor(WHITE);
                if(horas<10) display.print("0");
                display.print(horas);
                display.print(":");
                if(minutos<10) display.print("0");
                display.print(minutos);
                
                lastMinuteShown = minutos;
            }
            
            xSemaphoreGive(xTimeSemaphore);
        }

        

        // ---- wifi status
        /*
        display.setCursor(85,30);
        display.setTextSize(1);
        if(WiFi.status() == WL_CONNECTED) { display.setTextColor(WHITE); display.print("Wifi Ok"); }
        else { display.setTextColor(BLACK, WHITE); display.print("Wifi NO"); }
        */


        // ---- current state

        if(lastMinuteShown != -1) {

            display.fillRect(0, 35, 125, 10, BLACK);  //x, y, w, h, color

            display.setCursor(0,35);
            display.setTextColor(WHITE);
            display.setTextSize(1);
            
            if(currentState == CAFE_IDLE) { display.print("Apagado"); }
            
            if(currentState == CAFE_SCHEDULED) {
                while(! xSemaphoreTake(xScheduleSemaphore, 10) ) vTaskDelay(10); 
                display.print("Agendado para "); 
                display.print(scheduledHour); 
                display.print(":"); 
                if(scheduledMinute<10) display.print("0"); 
                display.print(scheduledMinute);
                xSemaphoreGive(xScheduleSemaphore);   
            }
            
            if(currentState == CAFE_MAKING) { display.print("Haciendo cafe"); }

            if(currentState == CAFE_KEEP_WARM) { display.print("Cafe listo!"); }
            
            if(currentState == CAFE_FORCED_ON) { display.print("Forzado encendido"); }

        
        }
        


        // ---- ip and passcode

        display.fillRect(0, 50, 125, 10, BLACK); 
        display.setCursor(0, 50);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        if(WiFi.status() == WL_CONNECTED) { 
            display.print(WiFi.localIP().toString());
            display.print("  ");
            display.print(HTTPPassCode);
        }




        display.display();

        vTaskDelay(pdMS_TO_TICKS(1300));
    }

}
