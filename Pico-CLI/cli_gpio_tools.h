#ifndef CLI_GPIO_TOOLS_H
#define CLI_GPIO_TOOLS_H

#include <Arduino.h>

bool isProtectedPin(uint8_t pin);
bool isAnalogPin(uint8_t pin);
bool isInt(const char* str);
int getMode(const char* mode);
bool isValidPinList(char* str);
void split(char* input, uint8_t* output, uint8_t& size, char splitter);

#endif
