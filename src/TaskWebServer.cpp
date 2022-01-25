#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "config.h"
#include "shared.h"


/* prototypes */
void handleRoot();
void handleNotFound();
void showMainForm();
void handleSchedule();
String generateRandomString(int size);


//active http server object
WebServer server(80);


// this is used in URLs to difficult brute force attack
String superRandom;


String headerHTML = "<html><head><meta name='viewport' content='width=device-width,initial-scale=1.0'>"
                    "<title>CafeTime</title>"
                    "<link rel='preconnect' href='https://fonts.googleapis.com'>"
                    "<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>"
                    "<link href='https://fonts.googleapis.com/css2?family=Work+Sans:wght@300&display=swap' rel='stylesheet'>"
                    "<style>body,*{font-family:\"Work Sans\",sans-serif;}body{font-size:22px;}"
                    "html{background: cornsilk;}h1{color:#6b2a00;}"
                    "p{padding:7px;background:#f4d2ad;}"
                    "</style>"
                    "</head><body><h1>CafeTime";



void TaskWebServer(void * pvParam) {

    superRandom = generateRandomString(20);

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);
    server.on("/" + HTTPPassCode, showMainForm);
    
    server.on("/idle" + superRandom, []() {   // state change
        int32_t commandData;
        commandData = COMMAND_MAKE_IDLE;
        xQueueSend( xCommandQueue, (void *) &commandData, 20 );

        xSemaphoreTake(xStateChangedSemaphore, pdMS_TO_TICKS(1000));  //wait state change

        server.sendHeader("Location", "/" + HTTPPassCode);
        server.send(302, "redir");
    });


    server.on("/force_on" + superRandom, []() {   // state change
        int32_t commandData;
        commandData = COMMAND_FORCE_ON;
        xQueueSend( xCommandQueue, (void *) &commandData, 20 );

        xSemaphoreTake(xStateChangedSemaphore, pdMS_TO_TICKS(1000));  //wait state change

        server.sendHeader("Location", "/" + HTTPPassCode);
        server.send(302, "redir");
    });

    server.on("/sched" + superRandom, HTTP_POST, handleSchedule);



    vTaskDelay(pdMS_TO_TICKS(10000)); 

    server.begin();
    Console.print("HTTP server started");



    while(1) {
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(300));
    }

}

void handleRoot() {
    server.send(200, "text/html", headerHTML + "<h2>C&oacute;digo de seguridad</h2>"
                                    "<script>function redir() { window.location='/'+ document.getElementById('p').value;}</script>"
                                    "<input type='text' id='p' size='4' style='font-size:1.3em'> "
                                    "<input type='button' value='Enviar' onclick='redir()' style='font-size:1.3em'/>" );
}


void showMainForm() {
    String currentStatus = "";
    
    String bulletColor = "red";

    if(currentState == CAFE_IDLE) { currentStatus = String("Apagado"); bulletColor = "green"; }
    if(currentState == CAFE_MAKING) { currentStatus = String("Haciendo cafe"); }
    if(currentState == CAFE_KEEP_WARM) { currentStatus = String("Cafe listo! Manteniendo caliente"); }
    if(currentState == CAFE_FORCED_ON) { currentStatus = String("Forzado encendido"); }
    if(currentState == CAFE_SCHEDULED) {
        bulletColor = "green";
        while(! xSemaphoreTake(xScheduleSemaphore, 10) ) vTaskDelay(10); 
        currentStatus = String("Agendado para empezar "); 
        currentStatus.concat(scheduledHour); 
        currentStatus.concat(":"); 
        if(scheduledMinute<10) currentStatus.concat("0"); 
        currentStatus.concat(scheduledMinute);
        xSemaphoreGive(xScheduleSemaphore);   
    }

    server.send(200, "text/html", headerHTML + "<h2>Estado actual</h2><p><span style='color:" + bulletColor + "'>&#x25A0;</span> " + currentStatus + "</p>"
                                    "<br/>"
                                    "<h2>Configurar</h2>"
                                    "<script>function redir() { window.location='/'+ document.getElementById('p').value;}</script>"
                                    
                                    "Programar tiempo<br/>"
                                    "<form action='./sched" + superRandom + "' method='post'>"
                                    "<input type='text' size='2' name='h'  style='font-size:1.3em' value='" + String(scheduledHour) + "'>:"
                                    "<input type='text' size='2' name='m'  style='font-size:1.3em' value='" + String(scheduledMinute) + "'> "
                                    "<input type='submit' value='Agendar' style='font-size:1.3em' />"
                                   
                                    "<br/><br/>"
                                    "<h2>Acciones</h2>"

                                    "<input type='button' value='Apagar ahora' style='font-size:1.1em;padding:6px;' onclick='window.location=\"./idle" + superRandom + "\"' />"
                                    "&nbsp;&nbsp;&nbsp;"
                                    "<input type='button' value='Encender ahora' style='font-size:1.1em;padding:6px;' onclick='window.location=\"./force_on" + superRandom + "\"' />"

                                    );

}

void handleNotFound() {
    server.send(404, "text/html", "<html><head><meta name='viewport' content='width=device-width,initial-scale=1.0'></head>"
                                            "<body style='font-family:sans-serif'><h1>Error <a href='/'>Volver</a>");
}


void handleSchedule() {
    int32_t commandData;
    commandData = COMMAND_SCHEDULE;                             // state change
    xQueueSend( xCommandQueue, (void *) &commandData, 20 );

    commandData = server.arg("h").toInt();
    xQueueSend( xCommandQueue, (void *) &commandData, 20 );

    commandData = server.arg("m").toInt();
    xQueueSend( xCommandQueue, (void *) &commandData, 20 );

    xSemaphoreTake(xStateChangedSemaphore, pdMS_TO_TICKS(1000));  //wait state change

    server.sendHeader("Location", "/" + HTTPPassCode);
    server.send(302, "redir");
}


String generateRandomString(int size) {
    String letters = "abcdefghijklmnopqrstuvwxyz0123456789_$*ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    String result = "";

    while(size) {
        result.concat(letters[esp_random() % letters.length()]);
        size--;
    }
    return result;
}