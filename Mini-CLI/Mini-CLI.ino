#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include <ArduinoJson.h>

#include <SD.h>
#include <SPI.h> 

#include "cli_commands.h"

void cliTask(void* parameter) {
  static char buffer[64];
  static size_t index = 0;

  while (true) {
  
    while (Serial1.available()) {
      char c = Serial1.read();
      if (c == '\r' || c == '\n') {
        buffer[index] = '\0';
        if (index > 0) {
          handleCLICommand(buffer);  // custom parser
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

  Serial1.setTX(0); // TX di GP0
  Serial1.setRX(1); // RX di GP1

  Serial1.begin(115200);

  Serial2.setTX(4); // TX di GP0
  Serial2.setRX(5); // RX di GP1

  Serial2.begin(115200);


  setupCLI();  // Daftarkan perintah ke CLI

  // Buat task CLI
  xTaskCreate(
    cliTask,         // Fungsi task
    "CLI Task",      // Nama task
    4096,            // Ukuran stack
    NULL,            // Parameter
    1,               // Prioritas
    NULL             // Handle task (optional)
  );

}

void loop() {}