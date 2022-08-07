#include "../pinc.h"

#ifndef INCLUDE_H
#define INCLUDE_H

#define COMPILE_MYSQL 1

typedef int (*Cmd_ExecuteString_t)(int a1, int a2, const char *text);
static Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x8111F32;

typedef int (*FS_LoadDir_t)(char *path, char *dir);
static FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x08188F1A;//0x080A241C; //find address for cod4
/*
typedef int (*Info_SetValueForKey_t)(char *s, const char *key, const char *value);
static Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x081AA2CA;

typedef char* (*Info_ValueForKey_t)(char *s, const char *key);
static Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x081AAB50;
*/
typedef int (*SV_GameSendServerCommand_t)(int clientNum, signed int a2, const char *msg);
static SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x0817CE42;


#endif // INCLUDE_H
