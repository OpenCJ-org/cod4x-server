// OpenCJ main file since plugin API does not allow us to do enough

/**************************************************************************
 * Includes                                                               *
 **************************************************************************/

#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

#include "opencj_main.hpp"

#include "../server-ext/includes.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "scr_vm.h"
#include "scr_vm_functions.h"
#include "xassets/weapondef.h"
#include "bg_public.h"
#include "cscr_stringlist.h"
#include "server_public.h"

/**************************************************************************
 * Extern functions without available prototype                           *
 **************************************************************************/

extern int GScr_LoadScriptAndLabel(const char *scriptName, const char *labelName, qboolean mandatory);

/**************************************************************************
 * Defines                                                                *
 **************************************************************************/



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

// Callbacks for... everything we need to inform the GSC of
static int opencj_callbacks[OPENCJ_CB_COUNT];

// Per player elevator
static bool opencj_playerElevationPermissions[MAX_CLIENTS] = {0};
static bool opencj_isPlayerElevating[MAX_CLIENTS] = {0};

// Allow WASD
static bool opencj_playerWASDCallbackEnabled[MAX_CLIENTS] = {0};

// For button monitoring
static int opencj_previousButtons[MAX_CLIENTS] = {0};
static playermovement_t opencj_playerMovement[MAX_CLIENTS] = {{0}};

// Ground monitoring
bool opencj_clientOnGround[MAX_CLIENTS];
// Bounce monitoring
bool opencj_clientCanBounce[MAX_CLIENTS];
float opencj_clientBouncePrevVelocity[MAX_CLIENTS];

// Player objectives
objective_t opencj_playerObjectives[MAX_CLIENTS][16];

/**************************************************************************
 * Forward declarations for static functions                              *
 **************************************************************************/

static void opencj_onConnect(scr_entref_t id);
static void Gsc_GetFollowersAndMe(scr_entref_t id);
static void Gsc_StopFollowingMe(scr_entref_t id);
static void Gsc_ClientUserInfoChanged(scr_entref_t id);
static void Gsc_ScaleOverTime(scr_entref_t hudelem);
static void Gsc_GetConfigStringByIndex();

/**************************************************************************
 * Static functions                                                       *
 **************************************************************************/

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
            if (Scr_GetNumParam() != 1)
            {
                Scr_Error("Usage: (cleanedName =) self renameClient(<string>)\n");
            }

            char *s = Scr_GetString(0);
            renameClient(client, s);
            Scr_AddString(client->sess.newnetname);
            return;
        }
    }

    Scr_AddUndefined();
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
        if (opencj_playerElevationPermissions[entityNum] != canElevate)
        {
            opencj_playerElevationPermissions[entityNum] = canElevate;
            opencj_isPlayerElevating[entityNum] = false;
        }
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

extern void SetObjectiveIcon(objective_t *, unsigned int);
void PlayerCmd_Objective_Add(scr_entref_t id)
{
	if (id.classnum)
	{
		Scr_Error(va("gsc_player_objective_player_add() entity %i is not a player", id.entnum));
		Scr_AddUndefined();
		return;
	}

	int args = Scr_GetNumParam();
	if (args < 2)
	{
		Scr_Error("objective_add needs at least the first two parameters out of its parameter list of: index state [string] [position]\n");
	}

	int objective_number = Scr_GetInt(0);
	if ((objective_number < 0) || (objective_number >= (int)sizeof(opencj_playerObjectives[0])))
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objective_number, sizeof(opencj_playerObjectives[0]) - 1));
	}

	objective_t *obj = &opencj_playerObjectives[id.entnum][objective_number];
	if (obj->entNum != 1023)
	{
		if (g_entities[obj->entNum].r.inuse != 0)
		{
			g_entities[obj->entNum].r.svFlags = g_entities[obj->entNum].r.svFlags & 0xef;
		}
		obj->entNum = 1023;
	}

	ushort index = Scr_GetConstString(1);
    objectiveState_t state;
	if (index == scr_const.empty)
	{
		state = OBJST_EMPTY;
	}
	else if (index == scr_const.invisible)
	{
		state = OBJST_INVISIBLE;
	}
    else if (index == scr_const.active)
    {
        state = OBJST_ACTIVE;
    }
	else 
	{
		if (index != scr_const.current)
		{
			state = OBJST_EMPTY;
			Scr_ParamError(1, va("Illegal objective state \"%s\". Valid states are \"empty\", \"invisible\", \"current\"\n", SL_ConvertToString((unsigned int)index)));
		}
		state = OBJST_CURRENT;
	}
	obj->state = state;

	if (args > 2)
	{
		Scr_GetVector(2, obj->origin);
		obj->origin[0] = (float)(int)obj->origin[0];
		obj->origin[1] = (float)(int)obj->origin[1];
		obj->origin[2] = (float)(int)obj->origin[2];
		obj->entNum = 0x3ff;
		if (args > 3)
		{
			//SetObjectiveIcon(obj, 3);
            const char *str = Scr_GetString(3);
            extern int G_MaterialIndex(const char *);
            int idx = G_MaterialIndex(str);
            obj->icon = idx;
		}
	}
	obj->teamNum = 0;
}

void PlayerCmd_Objective_Delete(scr_entref_t id)
{
	if (id.classnum)
	{
		Scr_Error(va("gsc_player_objective_player_delete() entity %i is not a player", id));
		Scr_AddUndefined();
		return;
	}

	int objective_number = Scr_GetInt(0);
	if ((objective_number < 0) || (objective_number >= (int)sizeof(opencj_playerObjectives[0])))
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objective_number, 15));
	}
	objective_t *obj = &opencj_playerObjectives[id.entnum][objective_number];

	if (obj->entNum != 0x3ff)
	{
		if (g_entities[obj->entNum].r.inuse != 0)
		{
			g_entities[obj->entNum].r.svFlags = g_entities[obj->entNum].r.svFlags & 0xef;
		}
		obj->entNum = 0x3ff;
	}

	obj->state = OBJST_EMPTY;
	obj->origin[0] = 0.0;
	obj->origin[1] = 0.0;
	obj->origin[2] = 0.0;
	obj->entNum = 0x3ff;
	obj->teamNum = 0;
	obj->icon = 0;
}

void PlayerCmd_SetMoveSpeed_Wrap(scr_entref_t id)
{
    if (id.classnum)
    {
        Scr_ObjectError("Not an entity");
        return;
    }

    if (Scr_GetNumParam() != 1)
    {
        Scr_Error("Usage: self setmovespeed( <integer> )\n");
        return;
    }

    int entityNum = id.entnum;
    gentity_t *gentity = &g_entities[entityNum];
    if (!gentity->client)
    {
        Scr_ObjectError(va("Entity: %i is not a player", entityNum));
        return;
    }

    int speed = Scr_GetInt(0);

    if ((speed <= 0) || (speed > 1000))
    {
        Scr_Error("setmovespeed range is between 0 and 1000\n");
        return;
    }

    gentity->client->sess.moveSpeedScaleMultiplier = (float)speed / g_speed->integer;
}

/**************************************************************************
 * Global unctions                                                        *
 **************************************************************************/

void renameClient(gclient_t *client, char *s)
{
    if (!client || !s || (s[0] == '\0')) return;

    int i = 0;
    // Strip leading spaces
    while (s[i] == ' ')
    {
        s++; // Skip over the space
        i++;
    }

    i = 0;
    while (s[i] != '\0')
    {
        if (((unsigned char)s[i] < 0x1F) || ((unsigned char)s[i] == 0x7F))
        {
            s[i] = '?';
        }
        i++;
    }
    ClientCleanName(s, client->sess.cs.netname, sizeof(client->sess.cs.netname), qtrue);
    CS_SetPlayerName(&client->sess.cs, client->sess.cs.netname);
    Q_strncpyz(client->sess.newnetname, client->sess.cs.netname, sizeof(client->sess.newnetname));
}

/**************************************************************************
 * OpenCJ functions                                                       *
 **************************************************************************/

void opencj_init(void)
{
    // Init callbacks
    opencj_callbacks[OPENCJ_CB_PLAYERCOMMAND]           = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerCommand",             qfalse);
    opencj_callbacks[OPENCJ_CB_RPGFIRED]                = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_RPGFired",                  qfalse);
    opencj_callbacks[OPENCJ_CB_SPECTATORCLIENTCHANGED]  = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_SpectatorClientChanged",    qfalse);
    opencj_callbacks[OPENCJ_CB_USERINFO]                = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_UserInfoChanged",           qfalse);
    opencj_callbacks[OPENCJ_CB_STARTJUMP]               = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_StartJump",                 qfalse);
    opencj_callbacks[OPENCJ_CB_MELEEBUTTONPRESSED]      = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MeleeButton",               qfalse);
    opencj_callbacks[OPENCJ_CB_USEBUTTONPRESSED]        = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_UseButton",                 qfalse);
    opencj_callbacks[OPENCJ_CB_ATTACKBUTTONPRESSED]     = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_AttackButton",              qfalse);
    opencj_callbacks[OPENCJ_CB_MOVEFORWARD]             = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MoveForward",               qfalse);
    opencj_callbacks[OPENCJ_CB_MOVELEFT]                = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MoveLeft",                  qfalse);
    opencj_callbacks[OPENCJ_CB_MOVEBACKWARD]            = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MoveBackward",              qfalse);
    opencj_callbacks[OPENCJ_CB_MOVERIGHT]               = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_MoveRight",                 qfalse);
    opencj_callbacks[OPENCJ_CB_FPSCHANGE] 				= GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_FPSChange", 				qfalse);
    opencj_callbacks[OPENCJ_CB_ONGROUND_CHANGE]         = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnGroundChange",            qfalse);
    opencj_callbacks[OPENCJ_CB_PLAYER_BOUNCED]          = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerBounced",             qfalse);
    opencj_callbacks[OPENCJ_CB_ON_PLAYER_ELE]           = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_OnElevate",                 qfalse);
}

int opencj_getCallback(opencj_callback_t callbackType)
{
    if ((callbackType <= OPENCJ_CB_UNKNOWN) || (callbackType >= OPENCJ_CB_COUNT))
    {
        return 0;
    }

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
    // Extra
    Scr_AddFunction("printf",                   Gsc_Utils_Printf,                   qfalse);
    Scr_AddFunction("vectorscale",              Gsc_Utils_VectorScale,              qfalse);
    Scr_AddFunction("GetConfigStringByIndex",   Gsc_GetConfigStringByIndex,         qfalse);
    // For GSC compatibility with CoD2
    Scr_AddFunction("getcvar",                  GScr_GetCvar,                       qfalse);
    Scr_AddFunction("getcvarint",               GScr_GetCvarInt,                    qfalse);
    Scr_AddFunction("getcvarfloat",             GScr_GetCvarFloat,                  qfalse);
    Scr_AddFunction("setcvar",                  GScr_SetCvar,                       qfalse);

    // METHODS
    Scr_AddMethod("clientcommand",          PlayerCmd_ClientCommand,            qfalse); 
    Scr_AddMethod("stoprecord",             PlayerCmd_StopRecord,               qfalse);
    Scr_AddMethod("startrecord",            PlayerCmd_StartRecord,              qfalse);
    Scr_AddMethod("renameclient",           PlayerCmd_RenameClient,             qfalse);
    Scr_AddMethod("stopfollowingme",        Gsc_StopFollowingMe,                qfalse);
    Scr_AddMethod("clientuserinfochanged",  Gsc_ClientUserInfoChanged,          qfalse);
    Scr_AddMethod("getspectatorlist",       Gsc_GetFollowersAndMe,              qfalse);
    Scr_AddMethod("followplayer",           PlayerCmd_FollowPlayer,             qfalse);
    Scr_AddMethod("allowelevate",           PlayerCmd_allowElevate,             qfalse);
    Scr_AddMethod("enablewasdcallback",     PlayerCmd_EnableWASDCallback,       qfalse);
    Scr_AddMethod("disablewasdcallback",    PlayerCmd_DisableWASDCallback,      qfalse);
    Scr_AddMethod("player_onconnect",       opencj_onConnect,                   qfalse);
    Scr_AddMethod("objective_player_add",   PlayerCmd_Objective_Add,            qfalse);
	Scr_AddMethod("objective_player_delete",PlayerCmd_Objective_Delete,         qfalse);
    Scr_AddMethod("scaleovertime",          Gsc_ScaleOverTime,                  qfalse);
    // CoD2 methods that are named differently
    Scr_AddMethod("setg_speed",             PlayerCmd_SetMoveSpeed_Wrap,        qfalse);
    // For GSC compatibility with CoD2
    Scr_AddMethod("setclientcvar",          PlayerCmd_SetClientDvar,            qfalse);
    Scr_AddMethod("setclientcvars",         PlayerCmd_SetClientDvars,           qfalse);
    Scr_AddMethod("leftbuttonpressed",      PlayerCmd_MoveLeftButtonPressed,    qfalse);
    Scr_AddMethod("rightbuttonpressed",     PlayerCmd_MoveRightButtonPressed,   qfalse);
}

static void opencj_onConnect(scr_entref_t id)
{
    if (id.classnum != 0)
    {
        // Not an entity
        return;
    }

    gentity_t *gent = &g_entities[id.entnum];
    if (!gent || !gent->client)
    {
        // Not a player
        return;
    }

    int clientNum = gent->client->ps.clientNum;
    opencj_clearPlayerFPS(clientNum);

    opencj_previousButtons[id.entnum] = 0;
    memset(&opencj_playerMovement[id.entnum], 0, sizeof(opencj_playerMovement[id.entnum]));
    opencj_clientOnGround[id.entnum] = false;
    opencj_clientCanBounce[id.entnum] = false;
    opencj_clientBouncePrevVelocity[id.entnum] = 0.0f;
}

void opencj_onFrame(void)
{
    //mysql_handle_result_callbacks();
}

void opencj_onClientThink(gentity_t *ent)
{

}

void opencj_onJumpCheck(struct pmove_t *pm)
{

}

void opencj_onStartJump(struct pmove_t *pm)
{
    if(opencj_callbacks[OPENCJ_CB_STARTJUMP])
    {
        Scr_AddInt(pm->cmd.serverTime);
        short ret = Scr_ExecEntThread(&g_entities[pm->ps->clientNum], opencj_callbacks[OPENCJ_CB_STARTJUMP], 1);
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
    if (!client || !ucmd || !client->gentity || !client->gentity->client) return;

    gclient_t *gclient = client->gentity->client;

    // ---- 1. Client FPS determination
    int clientNum = client - svs.clients;
	int time = ucmd->serverTime;
    int avgFrameTime = 0;
    if (opencj_updatePlayerFPS(clientNum, time, &avgFrameTime))
    {
        // Client FPS changed, report this to GSC via callback
		if (opencj_callbacks[OPENCJ_CB_FPSCHANGE])
		{
			Scr_AddInt(avgFrameTime);
			short ret = Scr_ExecEntThread(client->gentity, opencj_callbacks[OPENCJ_CB_FPSCHANGE], 1);
			Scr_FreeThread(ret);
		}
	}

    // 2. Buttons pressed reporting
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


    // When spectating, client->gentity is the person you're spectating. We don't want reporting for them!
    if (gclient->sess.sessionState == SESS_STATE_PLAYING)
    {
        // 3. onGround reporting
        bool isOnGround = (gclient->ps.groundEntityNum != 1023);
        if (isOnGround != opencj_clientOnGround[clientNum])
        {
            opencj_clientOnGround[clientNum] = isOnGround;

            // This callback can spam! Filtering on GSC side required.
            if (opencj_callbacks[OPENCJ_CB_ONGROUND_CHANGE])
            {
                Scr_AddVector(gclient->ps.origin);
                Scr_AddInt(ucmd->serverTime);
                Scr_AddBool(isOnGround);
                short ret = Scr_ExecEntThread(client->gentity, opencj_callbacks[OPENCJ_CB_ONGROUND_CHANGE], 3);
                Scr_FreeThread(ret);
            }
        }


        // 4. Report bounce occurring (https://xoxor4d.github.io/research/cod4-doublebounce/)
        bool canBounce = ((gclient->ps.pm_flags & 0x4000) != 0);
        if (canBounce != opencj_clientCanBounce[clientNum])
        {
            // If the player can no longer bounce, it means they just bounced!
            if (!canBounce)
            {
                // If the Z velocity went up, it means they bounced. Unless new velocity is 0, then they loaded or whatever
                if ((gclient->ps.velocity[2] > opencj_clientBouncePrevVelocity[clientNum]) && (fabs(gclient->ps.velocity[2] - 0.0f) >= FLT_EPSILON))
                {
                    if (opencj_callbacks[OPENCJ_CB_PLAYER_BOUNCED])
                    {
                        Scr_AddInt(gclient->sess.cmd.serverTime);
                        short ret = Scr_ExecEntThread(client->gentity, opencj_callbacks[OPENCJ_CB_PLAYER_BOUNCED], 1);
                        Scr_FreeThread(ret);
                    }
                }
                else
                {
                    // Z velocity didn't go up, so they just slid off something, or spawn/setorigin/setvelocity whatever.
                }
            }
            else
            {
                // Player can bounce again after not being able to bounce anymore
            }
            opencj_clientCanBounce[clientNum] = canBounce;
        }

        // Always update the previous velocity
        opencj_clientBouncePrevVelocity[clientNum] = gclient->ps.velocity[2];
    }
}

/**************************************************************************
 * GSC commands                                                           *
 **************************************************************************/

static void Gsc_ClientUserInfoChanged(scr_entref_t ref)
{
    if (ref.classnum) return;

    extern void ClientUserinfoChanged(int clientNum);

	if (ref.entnum >= MAX_CLIENTS)
	{
		Scr_Error("gsc_player_clientuserinfochanged() entity is not a player");
		Scr_AddUndefined();
		return;
	}

	ClientUserinfoChanged(ref.entnum);
	Scr_AddBool(qtrue);
}

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

static void Gsc_ScaleOverTime(scr_entref_t hudelem)
{
    game_hudelem_s *hudelem_t = NULL;
    if (hudelem.classnum == 1)
    {
        hudelem_t = &g_hudelems[hudelem.entnum];
    }
    else
    {
        Scr_ObjectError("not a hud element");
        hudelem_t = 0;
    }

    if (Scr_GetNumParam() != 3)
    {
        Scr_Error("hudelem scaleOverTime(time_in_seconds, new_width, new_height)");
    }

    float time = Scr_GetFloat(0);
    if (time <= 0.0)
    {
        Scr_ParamError(0, va("scale time %g <= 0", time));
    }
    else if (time > 60.0)
    {
        Scr_ParamError(0, va("scale time %g > 60", time));
    }

    int newWidth = Scr_GetInt(1u);
    int newHeight = Scr_GetInt(2u);

    extern level_locals_t level;
    hudelem_t->elem.scaleStartTime = level.time;
    int roundedTimeMs = floorf((float)(time * 1000.0) + 0.5);
    hudelem_t->elem.scaleTime = roundedTimeMs;
    hudelem_t->elem.fromWidth = hudelem_t->elem.width;
    hudelem_t->elem.fromHeight = hudelem_t->elem.height;
    hudelem_t->elem.width = newWidth;
    hudelem_t->elem.height = newHeight;

    // Bug fix: it doesn't contain the correct value of the hudelem so needs to be overwritten
    // otherwise.. you get a 'jumpy' scaleovertime
    hudelem_t->elem.fromAlignOrg = hudelem_t->elem.alignOrg;
    hudelem_t->elem.fromAlignScreen = hudelem_t->elem.alignScreen;
}

static void Gsc_GetConfigStringByIndex()
{
    char buf[MAX_STRING_CHARS] = {0};
    if (Scr_GetNumParam() != 1)
    {
        Scr_ObjectError("GetConfigStringByIndex expected 1 arg: index");
        return;   
    }

    int index = Scr_GetInt(0);
    SV_GetConfigstring(index, buf, sizeof(buf));

    if (buf[0] == '\0')
    {
        Scr_AddUndefined();
    }
    else
    {
        Scr_AddString(buf);
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
        Scr_AddInt(player->client->lastServerTime);
        Scr_AddString(BG_GetWeaponDef(rpg->s.weapon)->szInternalName);
        Scr_AddEntity(rpg);
        int threadId = Scr_ExecEntThread(player, callback, 3);
        Scr_FreeThread(threadId);
    }
}

// Called by ASM when a player is trying to start an elevate (spammed while this occurs)
int Ext_IsPlayerAllowedToEle(struct pmove_t *pmove)
{
    Ext_PlayerTryingToEle(pmove);
    return (opencj_playerElevationPermissions[pmove->ps->clientNum]);
}

// Called by Ext_IsPlayerAllowedToEle because then we know a player is trying to elevate (this can get spammed)
void Ext_PlayerTryingToEle(struct pmove_t *pmove)
{
    int clientNum = pmove->ps->clientNum;
    // Only do the callback if the player wasn't already trying to elevate
    if (!opencj_isPlayerElevating[clientNum])
    {
        opencj_isPlayerElevating[clientNum] = true;
        // Let GSC know that a player is trying to elevate, and whether or not they are allowed to
        int callback = opencj_callbacks[OPENCJ_CB_ON_PLAYER_ELE];
        if(callback)
        {
            gentity_t *ent = SV_GentityNum(clientNum);
            Scr_AddBool(opencj_playerElevationPermissions[clientNum]);
            int threadId = Scr_ExecEntThread(ent, callback, 1);
            Scr_FreeThread(threadId);
        }
    }
}

// Called by ASM when we know a player is not trying to elevate at this point of time (needed to reset callback for player trying to elevate)
void Ext_PlayerNotEle(struct pmove_t *pmove)
{
    if (opencj_isPlayerElevating[pmove->ps->clientNum])
    {
        opencj_isPlayerElevating[pmove->ps->clientNum] = false;
    }
}

void Ext_SpectatorClientChanged(gentity_t *player, int beingSpectatedClientNum)
{
    int callback = opencj_callbacks[OPENCJ_CB_SPECTATORCLIENTCHANGED];
    if(callback)
    {
        if (beingSpectatedClientNum != -1)
        {
            gentity_t *ent = SV_GentityNum(beingSpectatedClientNum);
            Scr_AddEntity(ent);
        }
        else
        {
            Scr_AddUndefined();
        }
        int threadId = Scr_ExecEntThread(player, callback, 1);
        Scr_FreeThread(threadId);
    }
}

#ifdef __cplusplus
}
#endif
