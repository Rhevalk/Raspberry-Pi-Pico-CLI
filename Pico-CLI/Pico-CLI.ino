#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include <ArduinoJson.h>
#include <LittleFS.h>

#include "cli_commands.h"

void cliTask(void* parameter) {
  static char buffer[64];
  static size_t index = 0;

  while (true) {
  
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\r' || c == '\n') {
        buffer[index] = '\0';
        if (index > 0) {
          handleCLICommand(buffer);
          index = 0;
        }
      } else if (index < sizeof(buffer) - 1) {
        buffer[index++] = c;
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}

void setup() {
  Serial.begin(115200);

  Serial1.setTX(0); // TX GP0
  Serial1.setRX(1); // RX GP1

  Serial1.begin(115200);

  // Memulai LittleFS
  if (!LittleFS.begin()) {
    Serial.println("littleFS fails!");
    return;
  } else { 
    Serial.println("littleFS ready!");
  }
  

  setupCLI();  // CLI 

  // CLI task
  xTaskCreate(
    cliTask,         
    "CLI Task",    
    4096,           
    NULL,           
    1,          
    NULL           
  );

}

void loop() {}