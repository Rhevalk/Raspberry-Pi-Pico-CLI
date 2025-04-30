#include "cli_commands.h"
#include "cli_gpio.h"
#include "cli_fileEditor.h"

#include <LittleFS.h>
#include <Arduino_JSON.h>

#define MAX_COMMANDS 30

static CLICommand commands[MAX_COMMANDS];
static int commandCount = 0;

void addCmd(const char* name, CommandFunction func, const char* desc) {
  if (commandCount < MAX_COMMANDS) {
    commands[commandCount++] = { name, func, desc };
  }
}

void handleCLICommand(const char* input) {

  char inputCopy[64];
  strncpy(inputCopy, input, sizeof(inputCopy));
  inputCopy[sizeof(inputCopy) - 1] = '\0';

  char* argv[10];
  int argc = 0;

  char* token = strtok(inputCopy, " ");
  while (token && argc < 10) {
    argv[argc++] = token;
    token = strtok(NULL, " ");
  }

  if (argc == 0) return;

  for (int i = 0; i < commandCount; i++) {
    if (strcmp(argv[0], commands[i].name) == 0) {
      commands[i].func(argc, argv);
      return;
    }
  }

  Serial.print("Commands not found: ");
  Serial.println(argv[0]);
  Serial.println("Send h to help");

}

void executeCMDFromString(const char* input) {
  handleCLICommand(input);
}

// === Command ===

void cmd_Help(uint8_t argc, char* argv[]) {
  Serial.println("=== CLI Command List ===");
  for (int i = 0; i < commandCount; i++) {
    Serial.print("- ");
    Serial.print(commands[i].name);
    if (commands[i].description) {
      Serial.print(": ");
      Serial.println(commands[i].description);
    } else {
      Serial.println("");
    }
    Serial.flush();
  }
  Serial.flush();
}

void cmd_Print(uint8_t argc, char* argv[]) {
  String text = "";
  for (uint8_t i = 1; i < argc; i++) {
    text += String(argv[i]);
    if (i < argc - 1) text += " ";
  }
  Serial.println(text);

}

void cmd_AT(uint8_t argc, char* argv[]) {

  char atCommand[256];  // Buffer AT command
  int cmdPos = 0;

  for (uint8_t i = 1; i < argc; i++) {
    int len = strlen(argv[i]);
    memcpy(atCommand + cmdPos, argv[i], len);  // copy to buffer
    cmdPos += len;
    
    if (i < argc - 1) {
      atCommand[cmdPos++] = ' '; 
    }
  }
  atCommand[cmdPos] = '\0';  

  Serial1.print(atCommand);
  Serial1.print("\r\n");

  char response[512];
  memset(response, 0, sizeof(response));
  unsigned long timeout = millis() + 3000;
  bool finished = false;
  int resPos = 0;


  while (millis() < timeout && !finished) {
    while (Serial1.available()) {
      char c = Serial1.read();
      if (resPos < sizeof(response) - 1) {  
        response[resPos++] = c;
      }


      Serial.write(c);  
      Serial.flush();

      // Cek apakah sudah ada jawaban selesai
      if (strstr(response, "OK\r\n") != NULL || 
          strstr(response, "ERROR\r\n") != NULL || 
          strstr(response, "FAIL\r\n") != NULL) {
        finished = true;
        break;
      }
    }
  }

  Serial.flush();
}

void cmd_cld(uint8_t argc, char **argv) {
  if (argc < 3) {
    Serial.println("cmd: cld <filename> <key>");
    return;
  }

  const char* filename = argv[1];
  const char* key = argv[2];

  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.print("Failed to open ");
    Serial.println(filename);
  
    return;
  }

  String fileContent = file.readString();
  file.close();

  JSONVar doc = JSON.parse(fileContent);
  if (JSON.typeof(doc) == "undefined") {
    Serial.println("Failed to parse JSON.");
  
    return;
  }

  if (!doc.hasOwnProperty(key)) {
    Serial.print("Key not found: ");
    Serial.println(key);
  
    return;
  }

  Serial.print("Loaded command from key ");
  Serial.print(key);
  Serial.print(" in file ");
  Serial.print(filename);
  Serial.print(": ");
  Serial.println(doc[key]);

  if (JSON.typeof(doc[key]) == "string") {
    const char* commandString = (const char*)doc[key];
    if (commandString != nullptr && strlen(commandString) > 0) {
      executeCMDFromString(commandString);
    } else {
      Serial.println("Invalid command string.");
    }
  } else {
    Serial.println("Value is not a command string.");
  }

  Serial.flush();


}


// === Setup CLI ===

void setupCLI() {
  /*================SYSTEM================*/  
  addCmd("@", cmd_AT, "Set user to ESP-AT");
  addCmd("help", cmd_Help, "Show all commands");
  addCmd("echo", cmd_Print, "Teks print to display");
  addCmd("cld", cmd_cld, "Teks print to display");

  /*=================GPIO=================*/
  addCmd("gset", cmd_Gse, "Gpio set value");
  addCmd("grst", cmd_Grs, "Gpio reset");
  addCmd("gsts", cmd_Gst, "Gpio show status");

  /*=================FILE=================*/
  addCmd("mkfil",  cmd_fmk,  "Make file");
  addCmd("wrtfil", cmd_fwrt, "Write file");
  addCmd("rdfil",  cmd_frd,  "Read file");
  addCmd("edtfil", cmd_fedt, "Edit file");
  addCmd("rmvfil", cmd_frmv, "Remove file");
  addCmd("szfil", cmd_fsz, "Info file size");
  addCmd("rnmfil", cmd_frnm, "Rename file");

  addCmd("mkdir", cmd_mkdir, "Make directory");
  addCmd("rmdir", cmd_rmdir, "Remove directory");

  addCmd("ls", cmd_ls, "Table of Contents");
  addCmd("cd", cmd_cd, "Go to directory");

}