#include "cli_gpio_tools.h"
#include "cli_gpio.h"

/*===============GPIO SET===============*/
void cmd_Gse(uint8_t argc, char* argv[]) {
    if (argc < 3) {
      Serial1.println("cmd: gset [pin] < [1|0] | in | inpull > ");
      Serial1.println(F("<<END>>"));  // tanda akhir
      return;
    }
    // mode
    int mode = getMode(argv[2]);

    // penyimpan pins
    uint8_t pins[30];
    uint8_t sizeOfPins = 0;
    // split koma menjadi int
    split(argv[1], pins, sizeOfPins, ',');

    if (mode != -1) {

      if (mode == OUTPUT) {

        for (uint8_t i = 0; i < sizeOfPins; i++) {
          
          if (isProtectedPin(pins[i])) {
            Serial1.print(F("Pin "));
            Serial1.print(String(pins[i]));
            Serial1.println(F(" diproteksi dan tidak bisa diubah!"));
            Serial1.println(F("<<END>>"));  // tanda akhir
            return;
          }

          pinMode(pins[i], mode);

          if (isInt(argv[2])) {
            uint8_t value = atoi(argv[2]);
            analogWrite(pins[i], value);
            Serial1.println("Write analog pin: " + String(pins[i]) +" -> "+ String(value));
          } 

          else {
            
            if (strncmp(argv[2], "on", 2) == 0 && strlen(argv[2]) == 2) {
              digitalWrite(pins[i], HIGH);
              Serial1.println("Write digital pin: " + String(pins[i]) +" -> "+ String(argv[2]));
            }
            else if (strncmp(argv[2], "off", 3) == 0 && strlen(argv[2]) == 3) {
              digitalWrite(pins[i], LOW);
              Serial1.println("Write digital pin: " + String(pins[i]) +" -> "+ String(argv[2]));
            }

          }
          
        }
        Serial1.flush();
        Serial1.println(F("<<END>>"));  // tanda akhir
        return;
      }  

      else if (mode == INPUT || mode == INPUT_PULLUP) {

        for (uint8_t i = 0; i < sizeOfPins; i++) {

          if (isProtectedPin(pins[i])) {
            Serial1.print(F("Pin "));
            Serial1.print(String(pins[i]));
            Serial1.println(F(" diproteksi dan tidak bisa diubah!"));
            Serial1.println(F("<<END>>"));  // tanda akhir
            return;
          }

          pinMode(pins[i], mode);

          if (isAnalogPin(pins[i])) {
            int valA = analogRead(pins[i]);
            Serial1.println("Read analog pin: " + String(pins[i]) + " = " + String(valA));
          }

          else {
            int valD = digitalRead(pins[i]);
            Serial1.println("Read pin: " + String(pins[i]) + " = " + String(valD));
          }

        }
        Serial1.println(F("<<END>>"));  // tanda akhir
      }

      else {
        Serial1.println(F("Error value"));
        Serial1.println(F("<<END>>"));  // tanda akhir
        return;
      }

    } 
    
    else {
      Serial1.println(F("Wrong mode"));
      Serial1.println(F("<<END>>"));  // tanda akhir
      return;
    }

}

/*===============GPIO RST===============*/
void cmd_Grs(uint8_t argc, char* argv[]) {

    if (argc < 2) {
      Serial1.println("cmd: grst <[pin] | rn");
      Serial1.println(F("<<END>>"));  // tanda akhir
      return;
    }

    if (strcmp(argv[1], "r") == 0) {
      if (argc < 3) {
        Serial1.println(F("cmd: grst rn <from pin> <to pin>"));
        Serial1.println(F("<<END>>"));  // tanda akhir
        return;
      }
      if ( isInt(argv[2]) && isInt(argv[3]) ) {

        Serial1.print(F("rst pin"));

        for (uint8_t i = atoi(argv[2]); i < atoi(argv[3]) + 1; i++) {
          if (!isProtectedPin(i)) {
            pinMode(i, INPUT);
            digitalWrite(i, LOW);
            Serial1.print(F("."));
          }
        }

        Serial1.println(F("RST Pin done!"));
        Serial1.println(F("<<END>>"));
        return;
      }
    } 

    else if (isValidPinList(argv[1])) {
      // penyimpan pins
      uint8_t pins[30];
      uint8_t sizeOfPins = 0;
      // split koma menjadi int
      split(argv[1], pins, sizeOfPins, ',');

      for (uint8_t i = 0; i < sizeOfPins; i++) {

      if (isProtectedPin(pins[i])) {
        Serial1.print("Pin ");
        Serial1.print(String(pins[i]));
        Serial1.println(" diproteksi dan tidak bisa diubah!");
        Serial1.println(F("<<END>>"));  // tanda akhir
        return;
      }

      pinMode(pins[i], INPUT);
      digitalWrite(pins[i], LOW);
      Serial1.println("RST Pin: " + String(pins[i]));

      }
    }

    else {
      Serial1.println(F("Wrong arguments"));
    }
  Serial1.println(F("<<END>>"));  // tanda akhir
}

/*===============GPIO STS===============*/
void cmd_Gst(uint8_t argc, char* argv[]) {

    if (argc < 2) {
      Serial1.println("cmd: gsts <[pin] | rn>");
      Serial1.println(F("<<END>>"));  // tanda akhir
      return;
    }

    if (strcmp(argv[1], "rn") == 0) {
        if (argc < 3) {
          Serial1.println("cmd: gsts rn <from pin> <to pin>");
          Serial1.println(F("<<END>>"));  // tanda akhir
          return;
        }

      if ( isInt(argv[2]) && isInt(argv[3]) ) {

      for (int pin = atoi(argv[2]); pin < atoi(argv[3]) + 1; pin++) {
        bool isOutput = (sio_hw->gpio_oe & (1u << pin)) != 0;
        bool level = digitalRead(pin);
        Serial1.printf("Pin %02d: %s, Level: %s\n",
                      pin,
                      isOutput ? "OUTPUT" : "INPUT",
                      level ? "HIGH" : "LOW");
        delayMicroseconds(100); 
      }

      Serial1.flush();
      Serial1.println(F("<<END>>"));  // tanda akhir
      return;
      }
      else {
        Serial1.println(F("Arguments not int"));
      }
    } 
  
    else if (isValidPinList(argv[1])) {

      // penyimpan pins
      uint8_t pins[30];
      uint8_t sizeOfPins = 0;
      // split koma menjadi int
      split(argv[1], pins, sizeOfPins, ',');

      for (uint8_t i = 0; i < sizeOfPins; i++) {
        bool isOutput = (sio_hw->gpio_oe & (1u << pins[i])) != 0;
        bool level = digitalRead(pins[i]);
        Serial1.printf("Pin %02d: %s, Level: %s\n",
                      pins[i],
                      isOutput ? "OUTPUT" : "INPUT",
                      level ? "HIGH" : "LOW");
        delayMicroseconds(100); 
      }
    }

    else {
      Serial1.println(F("Wrong arguments"));
    }
    
  Serial1.flush();
  Serial1.println(F("<<END>>"));  // tanda akhir
}