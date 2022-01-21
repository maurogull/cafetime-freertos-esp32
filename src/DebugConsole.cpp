#include <Arduino.h>
#include "DebugConsole.h"
#include "config.h"

DebugConsole::DebugConsole() {
    Serial.begin(DEBUG_CONSOLE_BAUDATE);
}

void DebugConsole::print(String msg) {
    if(DEBUG_CONSOLE_ENABLE) Serial.println(String(pcTaskGetTaskName(NULL)) + ": " + msg);
}