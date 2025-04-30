#include "cli_gpio_tools.h"

// Pin Protection
const uint8_t protectedPins[] = {0, 1, 2, 3, 4, 5, 6, 7}; 
const size_t protectedPinCount = sizeof(protectedPins) / sizeof(protectedPins[0]);

const uint8_t analogPin[] = {26, 27, 28, 29};
const size_t analogPinCount = sizeof(analogPin) / sizeof(analogPin[0]);

///////////////////////////////////////////////////////

bool isProtectedPin(uint8_t pin) {
  for (size_t i = 0; i < protectedPinCount; i++) {
    if (protectedPins[i] == pin) return true;
  }
  return false;
}

bool isAnalogPin(uint8_t pin) {
  for (uint8_t i = 0; i < analogPinCount; i++) {
    if (pin == analogPin[i]) return true;
  } return false;
}

bool isInt(const char* str) {
  if (str == NULL) return false;

  if (*str == '-' || *str == '+') str++;
  if (*str == '\0') return false;

  while (*str) {
    if (!isdigit(*str)) return false;
    str++;
  }

  return true;
}

int getMode(const char* mode) {
  if (mode == NULL) return -1;

  if (isInt(mode) || (strncmp(mode, "on", 2) == 0 && strlen(mode) == 2) || (strncmp(mode, "off", 3) == 0 && strlen(mode) == 3) ) return OUTPUT;
  else if (strcmp(mode, "in") == 0) return INPUT;
  else if (strcmp(mode, "inpull") == 0) return INPUT_PULLUP;
  else return -1;
}

bool isValidPinList(char* str) {
  while (*str) {
    if (!isdigit(*str) && *str != ',') {
      return false;
    }
    str++;
  }
  return true;
}

void split(char* input, uint8_t* output, uint8_t& size, char splitter) {
  char* token;
  int index = 0;


  if (strchr(input, splitter) == NULL) {
    output[0] = atoi(input);  
    size = 1;
    return;
  }

  token = strtok(input, &splitter);

  while (token != NULL) {
    output[index++] = atoi(token);
    token = strtok(NULL, &splitter);
  }

  size = index;
}
