#include <Arduino.h>
#include "Relay.h"
#include "config.h"

Relay::Relay() {
    pinMode(RELAY_PIN, OUTPUT);
    off();
}

void Relay::off() {
    digitalWrite(RELAY_PIN, 0);
}

void Relay::on() {
    digitalWrite(RELAY_PIN, 1);
}