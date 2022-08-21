#include "gsc_player.hpp"
#include "shared_defs.h"

void gsc_client_download(int id)
{
    client_t *cl = Plugin_GetClientForClientNum(id);
    char *dlname = Plugin_Scr_GetString(0);
    if (cl)
    {
        if (dlname)
        {
            strncpy(cl->downloadName, dlname, sizeof(cl->downloadName));
            cl->downloadName[sizeof(cl->downloadName) - 1] = '\0';
        }
    }
}

void gsc_setdemorecording(int id)
{
    client_t *client = Plugin_GetClientForClientNum(id);
    if (client)
    {
        client->demorecording = (qboolean)Plugin_Scr_GetInt(0);
        if (client->demorecording)
        {
            client->demoDeltaFrameCount = 0;
        }
    }
}

void gsc_getQueuedReliableMessages(int id)
{
    client_t *c = Plugin_GetClientForClientNum(id);
    if (c)
    {
        int count = c->reliableSequence - c->reliableAcknowledge;
        Plugin_Scr_AddInt(count);
    }
    else
    {
        Plugin_Scr_AddInt(0);
    }
}

unsigned int jh_fps[64];
void gsc_resetfps(int id)
{
    client_t *c = Plugin_GetClientForClientNum(id);
    if (c)
    {
        jh_fps[id] = c->clFrames;
    }
    else
    {
        jh_fps[id] = 0;
    }
}

void gsc_getfps(int id)
{
    client_t *c = Plugin_GetClientForClientNum(id);
    if (c)
    {
        int fps = 20 * (c->clFrames - jh_fps[id]);
        Plugin_Scr_AddInt(fps);
    }
    else
    {
        Plugin_Scr_AddInt(0);
    }
}

void Gsc_Player_Velocity_Add(int id)
{
    vec3_t velocity;
    Plugin_Scr_GetVector(0, &velocity);

    playerState_t *ps = Plugin_SV_GameClientNum(id);
    if (ps)
    {
        ps->velocity[0] += velocity[0];
        ps->velocity[1] += velocity[1];
        ps->velocity[2] += velocity[2];
        Plugin_Scr_AddInt(1);
    }
    else
    {
        Plugin_Scr_AddInt(0);
    }
}

void Gsc_Player_GetIP(int id)
{
    char address[128] = {0};
    client_t *cl = Plugin_GetClientForClientNum(id);
    if (cl)
    {
        Plugin_NET_AdrToStringMT(&cl->netchan.remoteAddress, address, sizeof(address));
    }

    // Address contains port too, so remove that part. Use strrchr (last index of) so that it will probably work with ipv6 too
    const char *szPortIdx = strrchr(address, ':');
    if (szPortIdx)
    {
        int ipLen = szPortIdx - (char *)address;
        int junkLen = sizeof(address) - ipLen;
        memset(address + ipLen, '\0', junkLen);
    }

    Plugin_Scr_AddString((char *)address);
}

void gsc_player_issprinting(int id)
{
    playerState_t *ps = Plugin_SV_GameClientNum(id);
    if (ps)
    {
        if (ps->sprintState.lastSprintStart <= ps->sprintState.lastSprintEnd)
        {
            Plugin_Scr_AddInt(0);
        }
        else
        {
            Plugin_Scr_AddInt(1);
        }
    }
    else
    {
        Plugin_Scr_AddInt(0);
    }
}
