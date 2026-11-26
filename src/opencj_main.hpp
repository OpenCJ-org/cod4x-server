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

    OPENCJ_CB_FRAGBUTTONPRESSED,
    OPENCJ_CB_MOVEMENTSTATE,
    OPENCJ_CB_COUNT, // Always keep this as last entry
} opencj_callback_t;

// Prototypes
void renameClient(gclient_t *client, char *requestedName);

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
// Connection bans (server-ext/opencj_bans.cpp). Each returns 1 and writes the text
// the refused player sees when the address or player ID is banned, else 0.
struct netadr_s;
int opencj_getAddressBanMessage(struct netadr_s *address, char *message, int messageLength);
int opencj_getClientBanMessage(uint64_t playerID, uint64_t steamID, char *message,
                               int messageLength);

void Ext_RPGFiredCallback(gentity_t *, gentity_t *);
void Ext_WeaponFiredCallback(gentity_t *);
int Ext_IsPlayerAllowedToEle(struct pmove_t *);
void Ext_PlayerTryingToEle(struct pmove_t *);
void Ext_PlayerNotEle(struct pmove_t *);
void Ext_SpectatorClientChanged(gentity_t *, int, int);

#ifdef __cplusplus
}
#endif

#endif // __OPENCJ_MAIN_H