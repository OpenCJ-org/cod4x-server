#ifndef __OPENCJ_MAIN_H
#define __OPENCJ_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "scr_vm.h"
#include "bg_public.h"

// Enums
typedef enum
{
    OPENCJ_CB_UNKNOWN = 0, // Always 0, so handlers can see if no handler was found
    OPENCJ_CB_PLAYERCOMMAND,
    OPENCJ_CB_RPGFIRED,
    OPENCJ_CB_WEAPONFIRED,
    OPENCJ_CB_SPECTATORCLIENTCHANGED,
    OPENCJ_CB_USERINFO,
    OPENCJ_CB_STARTJUMP,
    OPENCJ_CB_MELEEBUTTONPRESSED,
    OPENCJ_CB_USEBUTTONPRESSED,
    OPENCJ_CB_ATTACKBUTTONPRESSED,
    OPENCJ_CB_MOVEFORWARD,
    OPENCJ_CB_MOVELEFT,
    OPENCJ_CB_MOVEBACKWARD,
    OPENCJ_CB_MOVERIGHT,
    OPENCJ_CB_FPSCHANGE,
    OPENCJ_CB_ONGROUND_CHANGE,
    OPENCJ_CB_PLAYER_BOUNCED,
    OPENCJ_CB_ON_PLAYER_ELE,

    OPENCJ_CB_COUNT, // Always keep this as last entry
} opencj_callback_t;

// Prototypes
void renameClient(gclient_t *, char *);

void opencj_init(void);
void opencj_onFrame(void);
void opencj_onStartJump(struct pmove_t *);
void opencj_onClientThink(gentity_t *);
void opencj_onJumpCheck(struct pmove_t *);
void opencj_onUserInfoChanged(gentity_t *);
void opencj_onClientMoveCommand(client_t *, usercmd_t *);
void opencj_addMethodsAndFunctions(void);
int opencj_getCallback(opencj_callback_t);
void opencj_clearPlayerMovementCheckVars(int);

void Ext_RPGFiredCallback(gentity_t *);
void Ext_WeaponFiredCallback(gentity_t *);
int Ext_IsPlayerAllowedToEle(struct pmove_t *);
void Ext_PlayerTryingToEle(struct pmove_t *);
void Ext_PlayerNotEle(struct pmove_t *);
void Ext_SpectatorClientChanged(gentity_t *, int, int);

#ifdef __cplusplus
}
#endif

#endif // __OPENCJ_MAIN_H