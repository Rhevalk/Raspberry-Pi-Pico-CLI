#include "cli_commands.h"
#include "cli_gpio.h"
#include "cli_fileEditor.h"

#include <ArduinoJson.h>
#define ARDUINOJSON_ENABLE_DEPRECATED 0
// Alias agar sesuai anjuran ArduinoJson versi terbaru
using JsonDocument = StaticJsonDocument<512>;

#include <SD.h>
#include <SPI.h> 

#define MAX_COMMANDS 20

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

  Serial1.print("Commands not found: ");
  Serial1.println(argv[0]);
  Serial1.println("Send h to help");
  Serial1.println("<<END>>");  // tanda akhir
}

void executeCMDFromString(const char* input) {
  handleCLICommand(input); // langsung pakai fungsi parsing yang sudah ada
}

// === Command ===

void cmd_Help(uint8_t argc, char* argv[]) {
  Serial1.println("=== Daftar Perintah CLI ===");
  for (int i = 0; i < commandCount; i++) {
    Serial1.print("- ");
    Serial1.print(commands[i].name);
    if (commands[i].description) {
      Serial1.print(": ");
      Serial1.println(commands[i].description);
    } else {
      Serial1.println("");
    }
    
    if (i % 1 == 0) {
      Serial1.flush(); // flush setiap 5 baris
    }
  }
  Serial1.println("<<END>>");
  Serial1.flush(); // pastikan semua terkirim
}

void cmd_Print(uint8_t argc, char* argv[]) {
  String text = "";
  for (uint8_t i = 1; i < argc; i++) {
    text += String(argv[i]);
    if (i < argc - 1) text += " ";
  }
  Serial1.println(text);
  Serial1.println("<<END>>");  // tanda akhir
}

void cmd_AT(uint8_t argc, char* argv[]) {
  // Gabungkan semua argumen jadi satu baris AT command menggunakan buffer
  char atCommand[256];  // Buffer untuk AT command
  int cmdPos = 0;

  // Gabungkan semua argumen menjadi satu perintah AT
  for (uint8_t i = 1; i < argc; i++) {
    int len = strlen(argv[i]);
    memcpy(atCommand + cmdPos, argv[i], len);  // salin ke buffer
    cmdPos += len;
    
    if (i < argc - 1) {
      atCommand[cmdPos++] = ' '; // tambahkan spasi antara argumen
    }
  }
  atCommand[cmdPos] = '\0';  // Akhiri string dengan null terminator

  // Kirimkan perintah ke Serial2 dengan format \r\n
  Serial2.print(atCommand);
  Serial2.print("\r\n");

  // Buffer untuk menampung jawaban
  char response[512]; // Buffer untuk menampung respon
  memset(response, 0, sizeof(response)); // Reset buffer
  unsigned long timeout = millis() + 3000; // timeout 3 detik
  bool finished = false;
  int resPos = 0;

  // Baca data dari Serial2 dan kirimkan ke Serial1
  while (millis() < timeout && !finished) {
    while (Serial2.available()) {
      char c = Serial2.read();
      if (resPos < sizeof(response) - 1) {  // Pastikan tidak overflow
        response[resPos++] = c;
      }

      // Kirimkan data langsung ke Serial1
      Serial1.write(c);  // Kirimkan data ke Serial1
      Serial1.flush();

      Serial.write(c);  // Kirimkan data ke Serial1
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

  // Kirimkan tanda <<END>> untuk menunjukkan akhir
  Serial1.println("<<END>>"); 
  Serial1.flush();
}

void cmd_cld(uint8_t argc, char **argv) {
  if (argc < 2) {
    Serial.println("Usage: cld <key>");
    return;
  }

  const char* key = argv[1];
  File file = LittleFS.open("/gpio.json", "r");

  if (!file) {
    Serial.println("Failed to open gpio.json");
    return;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, file);
  file.close();

  if (err) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(err.c_str());
    return;
  }

  if (!doc[key]) {
    Serial.print("Key not found: ");
    Serial.println(key);
    return;
  }

  // Tampilkan dan jalankan
  Serial.print("Loaded command from key ");
  Serial.print(key);
  Serial.print(": ");
  serializeJsonPretty(doc[key], Serial);
  Serial.println();

  // Eksekusi perintah jika string
  if (doc[key].is<const char*>()) {
    String commandString = doc[key].as<const char*>();
    executeCMDFromString(commandString);
  } else {
    Serial.println("Value is not a command string.");
  }
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
  addCmd("fmk",  cmd_fmk,  "Create file");
  addCmd("fwrt", cmd_fwrt, "Write file");
  addCmd("frd",  cmd_frd,  "Read file");
  addCmd("fedt", cmd_fedt, "Edit file");
  addCmd("frmv", cmd_frmv, "Remove file");

}

  // tes bikin gpio read analog suhu, terus buat kode dari terminal buat dibaca dalam bantuk suhu

  // bikin file atau terminal khusu buat bikibn lingkuan program sendiri otomatis

  // cd
  /*
    🔧 Modularitas & Manajemen Perangkat
    Device Manager: Otomatis mendeteksi dan mengelola device (sensor, GPIO, I2C, UART, dll).

    Drivers ready-to-use: Banyak peripheral umum (seperti GPIO, RTC, EEPROM, Display) udah punya driver siap pakai.

    💻 Command Line Interface (CLI)
    Built-in CLI kayak terminal Linux

    Bisa ngetik perintah langsung dari UART/USB atau serial monitor

    Mendukung fitur seperti:

    ls, cd, cat, echo, gpio, i2c, dll

    Autocomplete dan history (tergantung build)

    📁 Filesystem
    Support LittleFS: Bisa simpan konfigurasi, script, data, dll ke flash.

    Bisa mount, read, write, mkdir, rm file langsung dari CLI.

    Bisa nyimpan konfigurasi device di filesystem dan autoload saat boot.

    📜 Script System
    Bisa bikin script CLI (.cli) untuk automasi tugas saat boot.

    Bisa dijalankan otomatis (autorun.cli) atau manual.

    Mirip shell script sederhana buat ngontrol semua device dari CLI.

    🧠 Konfigurasi Persisten
    Bisa simpan konfigurasi GPIO, device, dan lainnya ke file, lalu autoload saat startup.

    🔌 I2C / SPI / UART tools
    Bisa langsung scan I2C (i2c scan)

    Baca/tulis data ke device

    Debug komunikasi I2C langsung dari CLI

    ⚙️ Pengelolaan GPIO & Peripheral
    Control pin GPIO secara live (gpio set, gpio read)

    Timer, PWM, analog input juga bisa diatur

    🔁 Support multitasking (via FreeRTOS)
    Bisa jalanin banyak task paralel jika dibuild pakai FreeRTOS.

    📦 Dukungan Platform
    RP2040

    STM32

    ESP32 (dalam pengembangan/eksperimental)

    🧰 Cocok Buat:
    Proyek embedded kompleks yang butuh CLI

    Membuat sistem seperti Flipper Zero atau mini komputer GPIO

    Prototyping dengan banyak komponen I2C/SPI/UART

    Sistem scripting ringan di atas mikrokontroler

    1. Fitur CLI Script kamu sekarang
Berbasis GPIOScriptInterpreter, berikut semua yang bisa dilakukan:

✅ Struktur Program Scripting
label:loop → Buat label

goto loop → Lompat ke label

state = 1, setstate 2, goto state → Sistem state machine

✅ Kontrol GPIO
gpio set <pin> <0|1> → Set output

gpio rst <pin|all> → Reset pin

gpio status → Status semua pin

✅ Logika & Matematika
a = 3 + 2 * 4 → Variabel dan ekspresi

if a == 10 goto end → Percabangan logika

Mendukung: == != < > <= >= && ||

✅ Loop
while a < 5 ... end

for i = 0 to 3 ... end

✅ Delay
delay 1000 → Delay 1000 ms

✅ Komentar
# ini komentar

5. Next Step (Opsional)
Kalau kamu mau lanjut lebih pro:

 Load script dari file LittleFS (loadscript config.cli)

 Tambahkan print, input, break, continue

 Nested function support (function blink() ... end)

 Evaluator infix math expression yang proper (pakai stack)
  */