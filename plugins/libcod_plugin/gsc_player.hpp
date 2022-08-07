#ifndef GSC_PLAYER_H
#define GSC_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif	

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void Gsc_Player_Velocity_Add(int id);

void Gsc_Player_PrintF(int id);
void Gsc_Player_GetPlayerJumpheight(int id);
void Gsc_Player_GetPlayerGravity(int id);
void Gsc_Player_GetIP(int id);
void gsc_getQueuedReliableMessages(int id);

void gsc_resetfps(int id);
void gsc_getfps(int id);

void gsc_save_setdistance_travelled(int id);
void gsc_player_issprinting(int id);
void gsc_client_download(int id);


#ifdef __cplusplus
}
#endif

#endif // GSC_PLAYER_H