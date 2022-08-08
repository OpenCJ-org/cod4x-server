// OpenCJ main file since plugin API does not allow us to do enough

/**************************************************************************
 * Includes                                                               *
 **************************************************************************/

#include <stdlib.h>
#include <stdbool.h>

#include "opencj_main.hpp"

#include "gsc_custom_player.hpp"
#include "gsc_custom_utils.hpp"
#include "gsc_saveposition.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "scr_vm.h"
#include "scr_vm_functions.h"
#include "xassets/weapondef.h"
#include "bg_public.h"

/**************************************************************************
 * Extern functions without available prototype                           *
 **************************************************************************/

extern int GScr_LoadScriptAndLabel(const char *scriptName, const char *labelName, qboolean mandatory);

/**************************************************************************
 * Defines                                                                *
 **************************************************************************/

#define MAX_NR_FPS  20

/**************************************************************************
 * Types                                                                  *
 **************************************************************************/

typedef struct
{
    char forward;
    char right;
} playermovement_t;

/**************************************************************************
 * Global variables                                                       *
 **************************************************************************/

static bool opencj_playerElevationPermissions[MAX_CLIENTS] = {0};
static bool opencj_playerWASDCallbackEnabled[MAX_CLIENTS] = {0};
static int opencj_callbacks[OPENCJ_CB_COUNT];

static int opencj_clientfps[MAX_CLIENTS][MAX_NR_FPS];
static int opencj_clientfpsindex = 0;

static int opencj_previousButtons[MAX_CLIENTS] = {0};
static playermovement_t opencj_playerMovement[MAX_CLIENTS] = {{0}};

/**************************************************************************
 * Forward declarations for static functions                              *
 **************************************************************************/

static void opencj_onDisconnect(scr_entref_t id);
static void Gsc_GetFollowersAndMe(scr_entref_t id);
static void Gsc_StopFollowingMe(scr_entref_t id);

/**************************************************************************
 * Static functions                                                       *
 **************************************************************************/

static void PlayerCmd_ResetFPS(scr_entref_t arg)
{
    mvabuf;

    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        int entityNum = arg.entnum;
        gentity_t *gentity = &g_entities[entityNum];

        if (!gentity->client)
        {
            Scr_ObjectError(va("Entity: %i is not a player", entityNum));
        }
        else
        {
            const int maxNrFps = (sizeof(opencj_clientfps[0]) / sizeof(opencj_clientfps[0][0]));
            for (int i = 0; i < maxNrFps; i++)
            {
                opencj_clientfps[entityNum][i] = -1;
            }

            client_t *client = svs.clients + entityNum;
            client->clFrames = -1;
        }
    }
}

static void PlayerCmd_GetFPS(scr_entref_t arg)
{
    mvabuf;
    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        int entityNum = arg.entnum;
        gentity_t *gentity = &g_entities[entityNum];

        if (!gentity->client)
        {
            Scr_ObjectError(va("Entity: %i is not a player", entityNum));
        }
        else
        {
            int total = 0;
            const int maxNrFps = (sizeof(opencj_clientfps[0]) / sizeof(opencj_clientfps[0][0]));
            for (int i = 0; i < maxNrFps; i++)
            {
                if (opencj_clientfps[entityNum][i] == -1)
                {
                    Scr_AddInt(-1);
                    return;
                }
                total += opencj_clientfps[entityNum][i];
                Scr_AddInt(total);
            }
        }
    }
}

static void PlayerCmd_FPSNextFrame()
{
    const int maxNrFps = (sizeof(opencj_clientfps[0]) / sizeof(opencj_clientfps[0][0]));
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        gentity_t *gentity = &g_entities[i];

        if (!gentity->client)
        {
            opencj_clientfps[i][opencj_clientfpsindex] = 0;
            continue;
        }

        client_t *client = svs.clients + i;
        opencj_clientfps[i][opencj_clientfpsindex] = client->clFrames;
        client->clFrames = 0;
    }

    if (++opencj_clientfpsindex >= maxNrFps)
    {
        opencj_clientfpsindex = 0;
    }
}

static void PlayerCmd_ClientCommand(scr_entref_t arg)
{
    mvabuf;

    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        int entityNum = arg.entnum;
        gentity_t *gentity = &g_entities[entityNum];

        if (!gentity->client)
        {
            Scr_ObjectError(va("Entity: %i is not a player", entityNum));
        }
        else
        {
            if (Scr_GetNumParam() != 0)
            {
                Scr_Error("Usage: self ClientCommand()\n");
            }
            else
            {
                ClientCommand(entityNum);
            }
        }
    }
}

static void PlayerCmd_FollowPlayer(scr_entref_t arg)
{
    mvabuf;

    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        int entityNum = arg.entnum;
        gentity_t *gentity = &g_entities[entityNum];
        if (!gentity->client)
        {
            Scr_ObjectError(va("Entity: %i is not a player", entityNum));
        }
        else
        {
            gentity = g_entities + entityNum;
            if (Scr_GetNumParam() != 1)
            {
                Scr_Error("Usage: self followplayer(newplayer entid)\n");
            }
            else
            {
                int target = Scr_GetInt(0);
                if (target >= 0)
                {
                    extern qboolean Cmd_FollowClient_f(gentity_t *, int);
                    Cmd_FollowClient_f(gentity, target);
                }
                else
                {
                    StopFollowing(gentity);
                }
            }
        }
    }
}

static void PlayerCmd_RenameClient(scr_entref_t arg)
{
    mvabuf;

    if (arg.classnum)
    {
            Scr_ObjectError("Not an entity");
    }
    else
    {
        int entityNum = arg.entnum;
        gentity_t *gentity = &g_entities[entityNum];
        if (!gentity->client)
        {
            Scr_ObjectError(va("Entity: %i is not a player", entityNum));
        }
        else
        {
            gentity = g_entities + entityNum;
            gclient_t *client = gentity->client;
            if(Scr_GetNumParam() != 1){
                    Scr_Error("Usage: self renameClient(<string>)\n");
            }

            char *s = Scr_GetString(0);
            renameClient(client, s);
        }
    }
}

static void PlayerCmd_StartRecord(scr_entref_t arg)
{
    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        int entityNum = arg.entnum;
        client_t *client = &svs.clients[entityNum];

        if (Scr_GetNumParam() != 1)
        {
            Scr_Error("Usage: self startrecord(<string>)\n");
        }
        else
        {
            char *demoname = Scr_GetString(0);
            if (demoname && (demoname[0] != '\0'))
            {
                SV_RecordClient(client, demoname);
            }
        }
    }
    Scr_AddInt(0);
}

static void PlayerCmd_StopRecord(scr_entref_t arg)
{
    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        int entityNum = arg.entnum;
        client_t *client = &svs.clients[entityNum];
        SV_StopRecord(client);
    }
    Scr_AddInt(0);
}

static void PlayerCmd_allowElevate(scr_entref_t arg)
{
    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        int entityNum = arg.entnum;
        bool canElevate = Scr_GetInt(0);
        opencj_playerElevationPermissions[entityNum] = canElevate;
    }
}

static void PlayerCmd_EnableWASDCallback(scr_entref_t arg)
{
    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        opencj_playerWASDCallbackEnabled[arg.entnum] = true;
    }
}

static void PlayerCmd_DisableWASDCallback(scr_entref_t arg)
{
    if (arg.classnum)
    {
        Scr_ObjectError("Not an entity");
    }
    else
    {
        opencj_playerWASDCallbackEnabled[arg.entnum] = false;
    }
}

/**************************************************************************
 * Global unctions                                                        *
 **************************************************************************/

void renameClient(gclient_t *client, char *s)
{
    if (!client || !s || (s[0] == '\0')) return;

    int i = 0;
    while (s[i] != '\0')
    {
        if ((unsigned char)s[i] < 0xa)
            s[i] = '?';
        i++;
    }
    ClientCleanName(s, client->sess.cs.netname, sizeof(client->sess.cs.netname) , qtrue);
    Q_strncpyz(client->sess.newnetname, client->sess.cs.netname, sizeof( client->sess.newnetname));
}

/**************************************************************************
 * OpenCJ functions                                                       *
 **************************************************************************/

void opencj_init(void)
{
    // Init callbacks
    opencj_callbacks[OPENCJ_CB_PLAYERCOMMAND]           = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerCommand",             qfalse);
    opencj_callbacks[OPENCJ_CB_PLAYERRPG]               = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_RPGFired",                  qfalse);
    opencj_callbacks[OPENCJ_CB_SPECTATORCLIENTCHANGED]  = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_SpectatorClientChanged",    qfalse);
    opencj_callbacks[OPENCJ_CB_WENTFREESPEC]            = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_WentFreeSpec",              qfalse);
    opencj_callbacks[OPENCJ_CB_USERINFO]                = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_UserInfoChanged",           qfalse);
    opencj_callbacks[OPENCJ_CB_STARTJUMP]               = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_StartJump",                 qfalse);
    opencj_callbacks[OPENCJ_CB_MELEEBUTTONPRESSED]      = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MeleeButton",               qfalse);
    opencj_callbacks[OPENCJ_CB_USEBUTTONPRESSED]        = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_UseButton",                 qfalse);
    opencj_callbacks[OPENCJ_CB_ATTACKBUTTONPRESSED]     = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_AttackButton",              qfalse);
    opencj_callbacks[OPENCJ_CB_MOVEFORWARD]             = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MoveForward",               qfalse);
    opencj_callbacks[OPENCJ_CB_MOVELEFT]                = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MoveLeft",                  qfalse);
    opencj_callbacks[OPENCJ_CB_MOVEBACKWARD]            = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MoveBackward",              qfalse);
    opencj_callbacks[OPENCJ_CB_MOVERIGHT]               = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MoveRight",                 qfalse);
}

int opencj_getCallback(opencj_callback_t callbackType)
{
    return opencj_callbacks[callbackType];
}

void opencj_addMethodsAndFunctions(void)
{
    #include "../../server-ext/shared.hpp"
    #include "../../server-ext/gsc_custom_utils.hpp"
    #include "../../server-ext/gsc_custom_player.hpp"
    #include "../../server-ext/gsc_saveposition.hpp"

    // Add functions
    const struct 
    {
        const char      *name;
        xfunction_t     call;
        int             developer;
    } scriptFunctions[] = {
        #include "../../server-ext/functions.hpp"
    };

    for(int i = 0; i < (int)(sizeof(scriptFunctions) / sizeof(scriptFunctions[0])); i++)
    {
        Scr_AddFunction(scriptFunctions[i].name, scriptFunctions[i].call, (scriptFunctions[i].developer == 0) ? qfalse : qtrue);
    }

    // Add methods
    typedef void (*compatibleMethod_t)(int);
    const struct 
    {
        const char      *name;
        compatibleMethod_t       call;
        int             developer;
    } scriptMethods[] = {
        #include "../../server-ext/methods.hpp"
    };

    for(int i = 0; i < (int)(sizeof(scriptMethods) / sizeof(scriptMethods[0])); i++)
    {
        Scr_AddMethod(scriptMethods[i].name, (xmethod_t)scriptMethods[i].call, (scriptMethods[i].developer == 0) ? qfalse : qtrue);
    }

    // FUNCTIONS
    Scr_AddFunction("fpsnextframe",         PlayerCmd_FPSNextFrame,             qfalse);
    // Extra
    Scr_AddFunction("printf",               Gsc_Utils_Printf,                   qfalse);
	Scr_AddFunction("vectorscale",          Gsc_Utils_VectorScale,              qfalse);
    // For GSC compatibility with CoD2
    Scr_AddFunction("getcvar",              GScr_GetCvar,                       qfalse);
    Scr_AddFunction("getcvarint",           GScr_GetCvarInt,                    qfalse);
    Scr_AddFunction("getcvarfloat",         GScr_GetCvarFloat,                  qfalse);
    Scr_AddFunction("setcvar",              GScr_SetCvar,                       qfalse);

    // METHODS
    Scr_AddMethod("clientcommand",          PlayerCmd_ClientCommand,            qfalse); 
    Scr_AddMethod("stoprecord",             PlayerCmd_StopRecord,               qfalse);
    Scr_AddMethod("startrecord",            PlayerCmd_StartRecord,              qfalse);
    Scr_AddMethod("renameclient",           PlayerCmd_RenameClient,             qfalse);
    Scr_AddMethod("stopfollowingme",        Gsc_StopFollowingMe,                qfalse);
    Scr_AddMethod("getspectatorlist",       Gsc_GetFollowersAndMe,              qfalse);
    Scr_AddMethod("followplayer",           PlayerCmd_FollowPlayer,             qfalse);
    Scr_AddMethod("getfps",                 PlayerCmd_GetFPS,                   qfalse);
    Scr_AddMethod("resetfps",               PlayerCmd_ResetFPS,                 qfalse);
    Scr_AddMethod("enablewasdcallback",     PlayerCmd_EnableWASDCallback,       qfalse);
    Scr_AddMethod("disablewasdcallback",    PlayerCmd_DisableWASDCallback,      qfalse);
    Scr_AddMethod("player_ondisconnect",    opencj_onDisconnect,                qfalse);
    // CoD2 methods that are named differently
    Scr_AddMethod("setg_speed",             PlayerCmd_SetMoveSpeed,             qfalse);
    // For GSC compatibility with CoD2
    Scr_AddMethod("setclientcvar",          PlayerCmd_SetClientDvar,            qfalse);
    Scr_AddMethod("setclientcvars",         PlayerCmd_SetClientDvars,           qfalse);
    Scr_AddMethod("leftbuttonpressed",      PlayerCmd_MoveLeftButtonPressed,    qfalse);
    Scr_AddMethod("rightbuttonpressed",     PlayerCmd_MoveRightButtonPressed,   qfalse);
}

static void opencj_onDisconnect(scr_entref_t id)
{
    //gsc_mysqla_ondisconnect(id);

    // Can call other onDisconnect methods from here
}

void opencj_onFrame(void)
{
    //mysql_handle_result_callbacks();
}

void opencj_onStartJump(struct pmove_t *pm)
{
    if(opencj_callbacks[OPENCJ_CB_STARTJUMP])
    {
        int ret = Scr_ExecEntThread(&g_entities[pm->ps->clientNum], opencj_callbacks[OPENCJ_CB_STARTJUMP], 0);
        Scr_FreeThread(ret);
    }
}

void opencj_onUserInfoChanged(gentity_t *ent)
{
    if(opencj_callbacks[OPENCJ_CB_USERINFO])
	{
		int threadId = Scr_ExecEntThread(ent, opencj_callbacks[OPENCJ_CB_USERINFO], 0);
		Scr_FreeThread(threadId);
	}
}

void opencj_onClientMoveCommand(client_t *client, usercmd_t *ucmd)
{
    int clientNum = client - svs.clients;//client->gentity->client->ps.clientNum;
	if(ucmd->buttons & KEY_MASK_MELEE && !(opencj_previousButtons[clientNum] & KEY_MASK_MELEE))
	{
		if(opencj_callbacks[OPENCJ_CB_MELEEBUTTONPRESSED])
		{
			short ret = Scr_ExecEntThread(client->gentity, opencj_callbacks[OPENCJ_CB_MELEEBUTTONPRESSED], 0);
			Scr_FreeThread(ret);
		}
	}

	if(ucmd->buttons & KEY_MASK_USE && !(opencj_previousButtons[clientNum] & KEY_MASK_USE))
	{
		if(opencj_callbacks[OPENCJ_CB_USEBUTTONPRESSED])
		{
			short ret = Scr_ExecEntThread(client->gentity, opencj_callbacks[OPENCJ_CB_USEBUTTONPRESSED], 0);
			Scr_FreeThread(ret);
		}
	}

	if(ucmd->buttons & KEY_MASK_FIRE && !(opencj_previousButtons[clientNum] & KEY_MASK_FIRE))
	{
		if(opencj_callbacks[OPENCJ_CB_ATTACKBUTTONPRESSED])
		{
			short ret = Scr_ExecEntThread(client->gentity, opencj_callbacks[OPENCJ_CB_ATTACKBUTTONPRESSED], 0);
			Scr_FreeThread(ret);
		}
	}

    if(ucmd->rightmove != opencj_playerMovement[clientNum].right)
    {
        if(ucmd->rightmove == 127) // Pressed D
        {
            if(opencj_callbacks[OPENCJ_CB_MOVERIGHT])
            {
                short ret = Scr_ExecEntThread(&g_entities[clientNum], opencj_callbacks[OPENCJ_CB_MOVERIGHT], 0);
                Scr_FreeThread(ret);
            }
        }
        else if(ucmd->rightmove == -127) // Pressed A
        {
            if(opencj_callbacks[OPENCJ_CB_MOVELEFT])
            {
                short ret = Scr_ExecEntThread(&g_entities[clientNum], opencj_callbacks[OPENCJ_CB_MOVELEFT], 0);
                Scr_FreeThread(ret);
            }
        }
        else
        {
            // Released a key, we don't care
        }

        opencj_playerMovement[clientNum].right = ucmd->rightmove;
    }

    if(ucmd->forwardmove != opencj_playerMovement[clientNum].forward)
    {
        if(ucmd->forwardmove == 127) // Pressed W
        {
            if(opencj_callbacks[OPENCJ_CB_MOVEFORWARD])
            {
                short ret = Scr_ExecEntThread(&g_entities[clientNum], opencj_callbacks[OPENCJ_CB_MOVEFORWARD], 0);
                Scr_FreeThread(ret);
            }
        }
        else if(ucmd->forwardmove == -127) // Pressed S
        {
            if(opencj_callbacks[OPENCJ_CB_MOVEBACKWARD])
            {
                short ret = Scr_ExecEntThread(&g_entities[clientNum], opencj_callbacks[OPENCJ_CB_MOVEBACKWARD], 0);
                Scr_FreeThread(ret);
            }
        }
        else
        {
            // Released a key, we don't care
        }

        opencj_playerMovement[clientNum].forward = ucmd->forwardmove;
    }

	opencj_previousButtons[clientNum] = ucmd->buttons;
}

/**************************************************************************
 * GSC commands                                                           *
 **************************************************************************/

static void Gsc_StopFollowingMe(scr_entref_t ref)
{
    if (ref.classnum)
        return;

    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        gentity_t *player = SV_GentityNum(i);
        if(player != NULL && player->client != NULL)
        {
            if(player->client->lastFollowedClient == ref.entnum)
                player->client->lastFollowedClient = -1;
        }
    }
}

static void Gsc_GetFollowersAndMe(scr_entref_t ref)
{
    int alsoMe;
    alsoMe = Scr_GetInt(0);

    Scr_MakeArray();

    if (ref.classnum)
        return;

    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        gentity_t *player = SV_GentityNum(i);

        if(!player)
            continue;

        if(!player->client)
            continue;

        if(player->client->spectatorClient != ref.entnum)
            continue;

        // Now we can treat it as a client
        client_t *client = &svs.clients[i];

        if(!client)
            continue;

        if(client->state != CS_ACTIVE)
            continue;

        // We don't have to check if this is the calling player, because it doesn't have itself as the spectatorClient

        Scr_AddEntity(player);
        Scr_AddArray();
    }

    if(alsoMe)
    {
        gentity_t *me = SV_GentityNum(ref.entnum);
        Scr_AddEntity(me);
        Scr_AddArray();
    }
}

/**************************************************************************
 * Functions that are called from ASM/C (mostly callbacks or checks)        *
 **************************************************************************/

void Ext_RPGFiredCallback(gentity_t *player, gentity_t *rpg)
{
    int callback = opencj_callbacks[OPENCJ_CB_RPGFIRED];
    if (callback != 0)
    {
        Scr_AddString(BG_GetWeaponDef(rpg->s.weapon)->szInternalName);
        Scr_AddEntity(rpg);
        int threadId = Scr_ExecEntThread(player, callback, 2);
        Scr_FreeThread(threadId);
    }
}

int Ext_IsPlayerAllowedToEle(struct pmove_t *pmove)
{
    return (opencj_playerElevationPermissions[pmove->ps->clientNum]);
}

void Ext_SpectatorClientChanged(gentity_t *player, int beingSpectatedClientNum)
{
    int callback = opencj_callbacks[OPENCJ_CB_SPECTATORCLIENTCHANGED];
    if(callback)
    {
        gentity_t *ent = SV_GentityNum(beingSpectatedClientNum);
        Scr_AddEntity(ent);
        int threadId = Scr_ExecEntThread(player, callback, 1);
        Scr_FreeThread(threadId);
    }
}

void Ext_WentFreeSpec(gentity_t *player)
{
    int callback = opencj_callbacks[OPENCJ_CB_WENTFREESPEC];
    if(callback)
    {
        int threadId = Scr_ExecEntThread(player, callback, 0);
        Scr_FreeThread(threadId);
    }
}

#ifdef __cplusplus
}
#endif
