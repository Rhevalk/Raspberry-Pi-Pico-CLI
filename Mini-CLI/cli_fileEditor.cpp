#include "cli_fileEditor.h"

#include <SD.h>
#include <SPI.h> 

/*============MAKE FILE============*/
void cmd_fmk(uint8_t argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("Usage: fmk <type> <filename>");
    return;
  }
  String type = argv[1];
  String name = argv[2];
  String filename = name + "." + type;

  File file = SD.open(filename.c_str(), FILE_WRITE);
  if (file) {
    Serial.println("File created: " + filename);
    file.close();
  } else {
    Serial.println("Failed to create file.");
  }
}

/*============WRITE FILE===========*/
void cmd_fwrt(uint8_t argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("Usage: fwrt <filename> \"content\"");
    return;
  }
  File file = SD.open(argv[1], FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file.");
    return;
  }
  file.truncate(0); // kosongkan dulu
  file.print(argv[2]);
  file.close();
  Serial.println("File written.");
}

/*============READ FILE============*/
void cmd_frd(uint8_t argc, char* argv[]) {
  if (argc < 1) {
    Serial.println("Usage: frd <filename>");
    return;
  }
  File file = SD.open(argv[1]);
  if (!file) {
    Serial.println("Failed to open file.");
    return;
  }
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
  Serial.println();
}

/*============EDIT FILE============*/
void cmd_fedt(uint8_t argc, char* argv[]) {
  if (argc < 3) {
    Serial.println("Usage:");
    Serial.println("  fedt -n <filename> <index> \"text\"");
    Serial.println("  fedt -r <filename> <index>");
    return;
  }

  String flag = argv[1];
  String filename = argv[2];
  int index = atoi(argv[3]);

  if (index < 0 || index > 100) {
    Serial.println("Index out of range (0-100)");
    return;
  }

  // Membaca semua baris ke array
  File file = SD.open(filename.c_str(), FILE_READ);
  if (!file) {
    Serial.println("Failed to open file.");
    return;
  }

  String lines[100]; int count = 0;
  while (file.available() && count < 100) {
    lines[count++] = file.readStringUntil('\n');
  }
  file.close();

  if (flag == "-n") {
    if (argc < 4) {
      Serial.println("Missing text to write.");
      return;
    }
    String newText = argv[4];
    if (index >= count) {
      // Tambahkan baris kosong jika index lebih besar
      for (int i = count; i < index; i++) {
        lines[i] = "";
      }
      count = index + 1;
    }
    lines[index] = newText;
    Serial.println("Line replaced/added at index " + String(index));

  } else if (flag == "-r") {
    if (index >= count) {
      Serial.println("Index out of range.");
      return;
    }
    // Geser baris ke atas untuk menghapus
    for (int i = index; i < count - 1; i++) {
      lines[i] = lines[i + 1];
    }
    count--;
    Serial.println("Line removed at index " + String(index));

  } else {
    Serial.println("Unknown flag.");
    return;
  }

  // Simpan kembali ke file
  File wfile = SD.open(filename.c_str(), FILE_WRITE);
  if (!wfile) {
    Serial.println("Failed to write file.");
    return;
  }
  wfile.truncate(0);
  for (int i = 0; i < count; i++) {
    wfile.println(lines[i]);
  }
  wfile.close();
}

/*===========REMOVE FILE===========*/
void cmd_frmv(uint8_t argc, char* argv[]) {
  if (argc < 1) {
    Serial.println("Usage: frmv <filename>");
    return;
  }
  if (SD.remove(argv[1])) {
    Serial.println("File removed.");
  } else {
    Serial.println("Failed to remove file.");
  }
}
