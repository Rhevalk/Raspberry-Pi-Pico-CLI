#include "cli_fileEditor.h"
#include <LittleFS.h>

String currentDir = "/";  // Direktori path

/*============MAKE FILE===========*/
void cmd_fmk(uint8_t argc, char* argv[]) {
  if (argc < 2) {
    Serial.println(F("cmd: fmk <type> <filename>"));
    
    return;
  }

  String type = argv[1];
  String name = argv[2];
  String filename = currentDir + "/" + name + "." + type; 

  File file = LittleFS.open(filename.c_str(), "w");
  if (file) {
    Serial.println("File created: " + filename);
    file.close();
  } else {
    Serial.println(F("Failed to create file."));
  }
  Serial.flush();
  
}

/*============WRITE FILE===========*/
void cmd_fwrt(uint8_t argc, char* argv[]) {
  if (argc < 2) {
    Serial.println(F("cmd: fwrt <filename> \"content\""));
    
    return;
  }

  String filename = currentDir + "/" + argv[1]; 

  if (!LittleFS.exists(filename.c_str())) {
    Serial.println(F("File tidak ditemukan."));
    
    return;
  }

  File file = LittleFS.open(filename.c_str(), "w");
  if (!file) {
    Serial.println(F("Failed to open file."));
    
    return;
  }

  file.truncate(0); 
  file.print(argv[2]);
  file.close();
  Serial.println(F("File written."));

  Serial.flush();
  
}

/*============READ FILE===========*/
void cmd_frd(uint8_t argc, char* argv[]) {
  if (argc < 1) {
    Serial.println(F("cmd: frd <filename>"));
    
    return;
  }

  String filename = currentDir + "/" + argv[1];  

  if (!LittleFS.exists(filename.c_str())) {
    Serial.println(F("File tidak ditemukan."));
    
    return;
  }

  File file = LittleFS.open(filename.c_str(), "r");
  if (!file) {
    Serial.println(F("Failed to open file."));
    
    return;
  }

  while (file.available()) {
    Serial.write(file.read());
  }

  file.close();
  Serial.flush();
  
}

/*============EDIT FILE (LittleFS dengan cd)===========*/
void cmd_fedt(uint8_t argc, char* argv[]) {
  if (argc < 3) {
    Serial.println(F("cmd: fedt <-n | -r ->(*)> <filename> <index> \"text\"*"));
    
    return;
  }

  String flag = argv[1];
  String filename = argv[2];
  int index = atoi(argv[3]);

  filename = currentDir + "/" + filename;  

  if (index < 0 || index > 100) {
    Serial.println("Index out of range (0-100)");
    
    return;
  }

  File file = LittleFS.open(filename.c_str(), "r");
  if (!file) {
    Serial.println("Failed to open file.");
    
    return;
  }

  String lines[100]; 
  int count = 0;
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

      for (int i = count; i < index; i++) {
        lines[i] = "";
      }
      count = index + 1;
    }
    lines[index] = newText;
    Serial.println("Line replaced/added at index " + String(index));

    Serial.flush();
    

  } else if (flag == "-r") {
    if (index >= count) {
      Serial.println("Index out of range.");
      
      return;
    }

    for (int i = index; i < count - 1; i++) {
      lines[i] = lines[i + 1];
    }
    count--;
    Serial.println("Line removed at index " + String(index));
    Serial.flush();
    

  } else {
    Serial.println("Unknown flag.");
    
    return;
  }


  File wfile = LittleFS.open(filename.c_str(), "w");
  if (!wfile) {
    Serial.println("Failed to write file.");
    
    return;
  }

  wfile.truncate(0); 
  for (int i = 0; i < count; i++) {
    wfile.println(lines[i]);
  }
  wfile.close();

  Serial.flush();
}

/*===========REMOVE FILE===========*/
void cmd_frmv(uint8_t argc, char* argv[]) {
  if (argc < 1) {
    Serial.println("cmd: frmv <filename>");
    
    return;
  }

  String filename = currentDir + "/" + argv[1];  
  if (LittleFS.remove(filename.c_str())) {
    Serial.println("File removed.");
  } else {
    Serial.println("Failed to remove file.");
  }
  Serial.flush();
  
}

/*============CREATE FOLDER (LittleFS)===========*/
void cmd_mkdir(uint8_t argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("cmd: mkdir <folder_name>");
    
    return;
  }

  String folderName = argv[1];
  if (LittleFS.mkdir(folderName.c_str())) {
    Serial.println("Folder created: " + folderName);
  } else {
    Serial.println("Failed to create folder.");
  }
  Serial.flush();
  
}

/*============CHECK IF FOLDER IS EMPTY===========*/
bool isFolderEmpty(String folderPath) {
  fs::Dir dir = LittleFS.openDir(folderPath.c_str());
  return !dir.next();  
}

/*============REMOVE FOLDER (LittleFS)===========*/
void cmd_rmdir(uint8_t argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("cmd: rmdir <folder_name>");
    
    return;
  }

  String folderName = currentDir + "/" + String(argv[1]); 
  if (!LittleFS.exists(folderName.c_str())) {
    Serial.println("Directory not found: " + folderName);
    
    return;
  }


  if (isFolderEmpty(folderName)) {
    Serial.println("Folder is now empty: " + folderName);
    
    return;
  } 

  // Buka direktori, langsung iterasi
  fs::Dir dir = LittleFS.openDir(folderName.c_str());
  while (dir.next()) {
    String child = folderName + "/" + dir.fileName();
    LittleFS.remove(child.c_str());  

    Serial.println("Folder contents removed: " + folderName);
    
    Serial.println("Folder is now empty and considered removed: " + folderName);
  }


  Serial.flush();
  
}


/*============GET FILE SIZE (LittleFS)===========*/
void cmd_fsz(uint8_t argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("cmd: fsize <filename>");
    
    return;
  }

  String filename = argv[1];
  File file = LittleFS.open(filename.c_str(), "r");
  if (!file) {
    Serial.println("Failed to open file.");
    
    return;
  }

  uint32_t fileSize = file.size();
  file.close();

  Serial.println("File size: " + String(fileSize) + " bytes");
  Serial.flush();
  
}


/*============LIST FILES (LittleFS)===========*/
void cmd_ls(uint8_t argc, char* argv[]) {
  String path = (argc > 1) ? currentDir + "/" + String(argv[1]) : currentDir;

  if (!LittleFS.exists(path.c_str())) {
    Serial.println("Directory not found: " + path);
    Serial.flush();
    
    return;
  }

  fs::Dir dir = LittleFS.openDir(path.c_str());
  Serial.println("Listing files in: " + path);
  while (dir.next()) {
    String fileName = dir.fileName();
    String fullPath = path + "/" + fileName;

    if (dir.isDirectory()) {
      Serial.print("[DIR] ");
    } else {
      Serial.print("[FILE] ");
    }

    Serial.println(fileName);
  }
  Serial.flush();
  
}




/*============RENAME FILE (LittleFS)===========*/
void cmd_frnm(uint8_t argc, char* argv[]) {
  if (argc < 3) {
    Serial.println("cmd: rename <old_name> <new_name>");
    
    return;
  }

  String oldName = argv[1];
  String newName = argv[2];

  if (LittleFS.rename(oldName.c_str(), newName.c_str())) {
    Serial.println("File renamed.");
  } else {
    Serial.println("Failed to rename file.");
  }
  Serial.flush();
  
}

/*============CHANGE DIRECTORY===========*/
void cmd_cd(uint8_t argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("cmd: cd <folder_name>");
    
    return;
  }

  String newDir = String(argv[1]);
  String fullPath = (newDir == "/") ? "/" : currentDir + "/" + newDir;

  if (!LittleFS.exists(fullPath.c_str())) {
    Serial.println("Path tidak ditemukan: " + fullPath);
    
    return;
  }

  fs::Dir d = LittleFS.openDir(fullPath.c_str());
  if (!d.next()) {
    Serial.println("Bukan direktori: " + fullPath);
    
    return;
  }

  currentDir = fullPath;
  Serial.println("Changed directory to: " + currentDir);

  Serial.flush();
  
}


