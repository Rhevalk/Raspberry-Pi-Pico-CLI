#ifndef CLI_FILEEDITOR_H
#define CLI_FILEEDITOR_H

#include <Arduino.h>


void cmd_fmk(uint8_t argc, char* argv[]) ;
void cmd_fwrt(uint8_t argc, char* argv[]);
void cmd_frd(uint8_t argc, char* argv[]) ;
void cmd_fedt(uint8_t argc, char* argv[]) ;
void cmd_frmv(uint8_t argc, char* argv[]) ;
void cmd_mkdir(uint8_t argc, char* argv[]); 
void cmd_rmdir(uint8_t argc, char* argv[]); 
void cmd_fsz(uint8_t argc, char* argv[]) ;
void cmd_ls(uint8_t argc, char* argv[]) ;
void cmd_frnm(uint8_t argc, char* argv[]) ;
void cmd_cd(uint8_t argc, char* argv[])   ;


#endif
