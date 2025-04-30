#ifndef CLI_COMMANDS_H
#define CLI_COMMANDS_H

#include <Arduino.h>
#include <FreeRTOS.h>


typedef void (*CommandFunction)(uint8_t argc, char* argv[]);

struct CLICommand {
  const char* name;
  CommandFunction func;
  const char* description;
};

void setupCLI();
void handleCLICommand(const char* input);

#endif
