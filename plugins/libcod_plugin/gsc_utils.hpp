#ifndef GSC_UTILS_HPP_
#define GSC_UTILS_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

void Gsc_Utils_GetAscii();
void Gsc_Utils_sprintf();
void Gsc_Utils_AddLanguage();
void Gsc_Utils_LoadLanguages();
void Gsc_Utils_GetLanguageItem();
void Gsc_Utils_FileLink();
void Gsc_Utils_FileUnlink();
void Gsc_Utils_FileExists();
void Gsc_Utils_FS_LoadDir();
void Gsc_Utils_CmdExecuteString();
void Gsc_Utils_Printf();
void Gsc_Utils_Fopen();
void Gsc_Utils_Fread();
void Gsc_Utils_Fwrite();
void Gsc_Utils_Fclose();
void Gsc_G_FindConfigstringIndex();
void Gsc_Utils_SendGameServerCommand();
void gsc_utils_gethex();
void gsc_utils_stringToFloat();
void gsc_utils_renice();
void gsc_misc_forcedlcvars();

int stackPushArray();
int stackPushArrayLast();

#ifdef __cplusplus
}
#endif

#endif // GSC_UTILS_HPP_