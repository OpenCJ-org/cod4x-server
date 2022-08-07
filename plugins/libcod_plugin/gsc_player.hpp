#ifndef GSC_PLAYER_H
#define GSC_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif	

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void Gsc_Player_Velocity_Set(int id);
void Gsc_Player_Velocity_Add(int id);

void Gsc_Player_ButtonAds(int id);
void Gsc_Player_ButtonLeft(int id);
void Gsc_Player_ButtonRight(int id);
void Gsc_Player_ButtonForward(int id);
void Gsc_Player_ButtonBack(int id);
void Gsc_Player_ButtonLeanLeft(int id);
void Gsc_Player_ButtonLeanRight(int id);
void Gsc_Player_ButtonJump(int id);
void Gsc_Player_PrintF(int id);
void Gsc_Player_SpectatorClientGet(int id);
void Gsc_Player_GetPlayerJumpheight(int id);
void Gsc_Player_GetPlayerGravity(int id);
void Gsc_Player_GetIP(int id);
void gsc_getQueuedReliableMessages(int id);
void gsc_save_initplayer(int id);
void gsc_save_createsave(int id);
void gsc_save_setcompleted(int id);
void gsc_save_loadpos(int id);
void gsc_save_setentity(int id);
void gsc_save_getentity(int id);
void gsc_save_getentitynum(int id);
void gsc_save_setcheated(int id);
void gsc_save_getcheated(int id);
void gsc_save_setbigjump(int id);
void gsc_save_getbigjump(int id);
void gsc_save_setjetpack(int id);
void gsc_save_getjetpack(int id);
void gsc_save_setloadout(int id);
void gsc_save_getloadout(int id);
void gsc_save_setorigin(int id);
void gsc_save_getorigin(int id);
void gsc_save_setangles(int id);
void gsc_save_getangles(int id);
void gsc_save_addcp(int id);
void gsc_save_getcps(int id);
void gsc_save_addpassed_checkpoint(int id);
void gsc_save_getpassed_checkpoints(int id);
void gsc_save_copysave(int id);
void gsc_save_addtrigger(int id);
void gsc_save_gettriggers(int id);
void gsc_save_setcansave(int id);
void gsc_save_getcansave(int id);
void gsc_save_setfps(int id);
void gsc_save_cps_init();
void gsc_setdemorecording(int id);
void gsc_printcommandtime(int id);
void gsc_save_getfps(int id);
void gsc_save_setrpg_out(int id);
void gsc_save_getrpg_out(int id);
void gsc_save_getnadejumps(int id);

void gsc_resetfps(int id);
void gsc_getfps(int id);

void gsc_save_setos_mode(int id);
void gsc_save_setfunmode(int id);
void gsc_save_settime_played(int id);
void gsc_save_setload_count(int id);
void gsc_save_setsave_count(int id);
void gsc_save_setjump_count(int id);
void gsc_save_setnadejumps(int id);
void gsc_save_setnadethrows(int id);
void gsc_save_setrun_id(int id);
void gsc_save_setdistance_travelled(int id);
void gsc_player_issprinting(int id);
void gsc_client_download(int id);


#ifdef __cplusplus
}
#endif

#endif // GSC_PLAYER_H