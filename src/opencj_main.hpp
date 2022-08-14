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
    OPENCJ_CB_PLAYERCOMMAND,
    OPENCJ_CB_RPGFIRED,
    OPENCJ_CB_SPECTATORCLIENTCHANGED,
    OPENCJ_CB_WENTFREESPEC,
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

    OPENCJ_CB_COUNT, // Always keep this as last entry
} opencj_callback_t;

// Prototypes
void renameClient(gclient_t *, char *);

void opencj_init(void);
void opencj_onFrame(void);
void opencj_onStartJump(struct pmove_t *);
void opencj_onUserInfoChanged(gentity_t *);
void opencj_onClientMoveCommand(client_t *, usercmd_t *);
void opencj_addMethodsAndFunctions(void);
int opencj_getCallback(opencj_callback_t);

void Ext_RPGFiredCallback(gentity_t *, gentity_t *);
int Ext_IsPlayerAllowedToEle(struct pmove_t *);
void Ext_SpectatorClientChanged(gentity_t *, int);
void Ext_WentFreeSpec(gentity_t *);

#ifdef __cplusplus
}
#endif

#endif // __OPENCJ_MAIN_H