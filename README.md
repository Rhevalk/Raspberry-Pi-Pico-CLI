# Raspberry-Pi-Pico-CLI

**Pico CLI (Command Line Interpreter)**  
A lightweight and powerful command-line tool for the Raspberry Pi Pico, designed to simplify testing and interaction with electronic components.

---

## ⚡ Quick Start

1. Download and extract the ZIP archive.
2. Configure GPIO protection settings and the [ESP-AT UART connection](#-wifi-module-with-esp-at).
3. Install the Raspberry Pi Pico board support package  
   👉 [Install Pico board in Arduino IDE](https://docs.arduino.cc/hardware/raspberry-pi-pico/)
4. Flash Pico make sure you use the FS option in the ```tools > file size```, don't choose the one (no fs) and you can adjust it to the flash available on your Pico board.
*tip: use 1MB or more flash and the rest for fs
5. If you include esp-AT, then you can connect to pin rx(1), tx(0) or adjust your tomorrow

---

## 📡 WiFi Module with ESP-AT

Flash the ESP-AT firmware to your module using the official guide:  
👉 [ESP-AT Firmware Documentation](https://docs.espressif.com/projects/esp-at/en/latest/esp32c3/)

Supported modules include:
- ESP8266  
- ESP32  
- ESP32-C2, ESP8684  
- ESP32-C3  
- ESP32-C6  
- ESP32-S2

Once the firmware is flashed, connect the module to the configured UART pins on the Pico.

To send AT commands via Pico's serial interface, use the `@` symbol as a prefix. Note that the ESP-AT module will not respond unless a host microcontroller is sending commands properly.

### 📘 ESP-AT Command Format

```bash
@ <your AT command>
@ AT          # Check connection/status
@ AT+GMR      # Check firmware version
@ AT+CWMODE?  # Query current WiFi mode
```

## ✨ Features

1. Control GPIO: on/off, PWM, analog read, and digital read
2. Create, delete, write, read, and edit any text-based files
3. Create, remove, cd, ls directory
4. ESP-AT support: control Wi-Fi and BLE via ESP modules  
   👉 [See ESP-AT command examples](https://docs.espressif.com/projects/esp-at/en/latest/esp32c3/AT_Command_Examples/index.html)

## How to Read the Command Format
<>   : input string, write without ""
[]   : input int
""   : input string, write with ""
|    : or
#    : int or int list
*    : required
->(*): Complete arguments in the order of command required



## Command List

### 🖥️ System
| Command   | Format                      | Description                                      |
|-----------|-----------------------------|--------------------------------------------------|
| `@`       | `@ <Command ESP-AT>`        | Untuk memerintah ESP-AT                         |
| `help`    | `help`                      | Daftar perintah yang tersedia di Pico CLI       |
| `echo`    | `echo <teks>`               | Menampilkan pesan yang Anda kirim               |

### 🔌 GPIO
| Command   | Format                                                                          | Description                                                          |
|-----------|----------------------------------------------------------------------------------|----------------------------------------------------------------------|
| `gset`    | `gset [pin(#list)] {on | off | [0 - 255] | input | inputpull}`                          | Mengontrol GPIO (digital/analog) secara otomatis                    |
| `grst`    | `grst {[pin(#list)] | rn ->(*)} [from pin]* [to pin]*`                          | Mereset pin yang dipilih dalam format tunggal, list, atau jangkauan |
| `gsts`    | `gsts {[pin(#list)] | rn ->(*)} [from pin]* [to pin]*`                          | Melihat status pin saat ini                                         |

### 💾 System Storage
| Command   | Format                                                 | Description                         |
|-----------|--------------------------------------------------------|-------------------------------------|
| `mkfil`   | `mkfil <type> <filename>`                              | Membuat file baru                   |
| `wrtfil`  | `wrtfil <filename> "content"`                          | Menulis isi ke dalam file           |
| `rdfil`   | `rdfil <filename>`                                     | Membaca isi file                    |
| `edtfil`  | `edtfil <-n | -r ->(*)> <filename> <index> "text"*`    | Mengedit file berdasarkan index     |
| `rmvfil`  | `rmvfil <filename>`                                    | Menghapus file                      |
| `szfil`   | `szfil <filename>`                                     | Menampilkan ukuran file             |
| `rnmfil`  | `rnmfil <old_name> <new_name>`                         | Mengganti nama file                 |
| `mkdir`   | `mkdir <folder_name>`                                  | Membuat folder                      |
| `rmvdir`  | `rmvdir <folder_name>`                                 | Menghapus folder                    |
| `ls`      | `ls`                                                   | Menampilkan daftar file/folder      |
| `cs`      | `cd <directory_name>`                                  | Masuk ke direktori tertentu         |


## 📄 License

This project is licensed under the MIT License - see the [MIT License](https://img.shields.io/badge/license-MIT-green) file for details.

