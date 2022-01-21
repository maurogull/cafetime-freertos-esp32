#pragma once


//print error msg to Serial, 1 or 0
#define DEBUG_CONSOLE_ENABLE 0

#define DEBUG_CONSOLE_BAUDATE 112500

//GPIO pin where power relay is connected
#define RELAY_PIN 23 

// GMT -3   : -3 * 60 * 60 : -10800
#define UCT_OFFSET_SECONDS -10800   

// display GPIO pins
#define OLED_SDA 21
#define OLED_SCL 22

// timing
#define MINUTES_HOLD_MAKING_STATE 10
#define MINUTES_HOLD_KEEP_WARM_STATE 5


// wifi credentials -- todo: config via bluetooth maybe?
#define WIFI_SSID "abcdeabcde"
#define WIFI_PASSWD "123456123456"