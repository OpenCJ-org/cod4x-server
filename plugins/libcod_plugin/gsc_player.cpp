#include "gsc_player.hpp"
#include "include.hpp"

int playerStates = 0x084BD120;
int sizeOfPlayer = 0x3184;
int gentities = 0x0841FFE0;
int gentities_size = 628;

int playerinfo_base = 0x090B4F8C;
int playerinfo_size = 0x2958F;

#define PLAYERSTATE(playerid) (playerStates + playerid * sizeOfPlayer)
#define PLAYERBASE(playerid) (playerinfo_base + playerid * playerinfo_size)
#define MAX_SAVES 512
#define MAX_TRIGGER_COUNT 512
#define MAX_CP_COUNT 512
#define MAX_PLAYERS 64
#define CHAR_BIT 8
#define MAX_ENDER_COUNT 16

void gsc_client_download(int id)
{
	client_t* cl = Plugin_GetClientForClientNum(id);
	char* dlname = Plugin_Scr_GetString(0);
	strncpy(cl->downloadName, dlname, 64);
	cl->downloadName[64] = '\0';
}

int jh_cp_ids[MAX_CP_COUNT];
int jh_cp_ids_count;
char jh_triggers[MAX_TRIGGER_COUNT][64];
int jh_triggers_count;
char jh_enders[MAX_ENDER_COUNT][128];
int jh_ender_count;

unsigned int jh_fps[64];

void gsc_save_cps_init()
{
	jh_cp_ids_count = 0;
	jh_triggers_count = 0;
}

struct jh_cp
{
	int origin[3];
	int type; //bonus, funmode, isend, onground, hidden, noprint
	int origin2[3];
	int radius;
	char event[64];
	char trigger[(int)((MAX_TRIGGER_COUNT - 1) / (sizeof(int) * CHAR_BIT)) + 1];
	int id;
	int ender;
	int childs[(int)((MAX_CP_COUNT - 1) / (sizeof(int) * CHAR_BIT)) + 1];
};

jh_cp jh_cps[MAX_CP_COUNT];

struct jh_save
{
	bool completed;
	bool cheated;
	bool jetpack;
	bool funmode;
	bool os_mode;
	bool cansave;
	int fps;
	vec3_t origin;
	vec3_t angles;
	int cps[(int)((MAX_CP_COUNT - 1) / (sizeof(int) * CHAR_BIT)) + 1];
	int passed_checkpoints[(int)((MAX_CP_COUNT - 1) / (sizeof(int) * CHAR_BIT)) + 1];
	int triggers[(int)((MAX_TRIGGER_COUNT - 1) / (sizeof(int) * CHAR_BIT)) + 1];
	char loadout[2][64];
	char entity[64];
	int entnum;
	int bigjump;
	int load_count;
	int save_count;
	int time_played;
	int nadejumps;
	int nadethrows;
	int run_id;
	int jump_count;
	int snum;
	float distance_travelled;
	bool rpg_out;
};

jh_save jh_saves[MAX_PLAYERS][MAX_SAVES];
int savenum[MAX_PLAYERS];

void gsc_save_initplayer(int id) //call on reset_stats and connect (thus in the init() of saveload and in the !loadpos)
{
	int snum = 0;
	if(Plugin_Scr_GetNumParam() == 1)
		snum = (((int)(Plugin_Scr_GetInt(0) / MAX_SAVES)) + 1) * MAX_SAVES;
	for(int i = 0; i < MAX_SAVES; i++)
		jh_saves[id][i].completed = false;
	savenum[id] = snum;
}

void gsc_save_copysave(int id)
{
	int srcplayer = Plugin_Scr_GetInt(0);
	if(!savenum[srcplayer])
		Plugin_Scr_AddInt(-1);
	else
	{
		memcpy(&jh_saves[id][(savenum[id] % MAX_SAVES)], &jh_saves[srcplayer][(savenum[srcplayer] - 1) % MAX_SAVES], sizeof(jh_save));
		jh_saves[id][(savenum[id] % MAX_SAVES)].completed = false;
		jh_saves[id][(savenum[id] % MAX_SAVES)].snum = savenum[id];
		savenum[id]++;
		Plugin_Scr_AddInt((savenum[id] - 1) % MAX_SAVES);
	}
}

void gsc_save_setos_mode(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].os_mode = (bool) Plugin_Scr_GetInt(1);
}

void gsc_save_setfunmode(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].funmode = (bool) Plugin_Scr_GetInt(1);
}

void gsc_save_settime_played(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].time_played = Plugin_Scr_GetInt(1);
}

void gsc_save_setfps(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].fps = Plugin_Scr_GetInt(1);
}

void gsc_save_setload_count(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].load_count = Plugin_Scr_GetInt(1);
}

void gsc_save_setsave_count(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].save_count = Plugin_Scr_GetInt(1);
}

void gsc_save_setjump_count(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].jump_count = Plugin_Scr_GetInt(1);
}

void gsc_save_setnadejumps(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].nadejumps = Plugin_Scr_GetInt(1);
}

void gsc_save_setnadethrows(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].nadethrows = Plugin_Scr_GetInt(1);
}


void gsc_save_setrun_id(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].run_id = Plugin_Scr_GetInt(1);
}

void gsc_save_setrpg_out(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].rpg_out = (bool)Plugin_Scr_GetInt(1);
}

void gsc_save_setdistance_travelled(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].distance_travelled = Plugin_Scr_GetFloat(1);
}

void gsc_save_createsave(int id)
{
	jh_saves[id][(savenum[id] % MAX_SAVES)].completed = false;
	for(int i = 0; i < 8; i++)
	{
		jh_saves[id][(savenum[id] % MAX_SAVES)].cps[i] = 0;
		jh_saves[id][(savenum[id] % MAX_SAVES)].passed_checkpoints[i] = 0;
		jh_saves[id][(savenum[id] % MAX_SAVES)].triggers[i] = 0;
	}
	jh_saves[id][(savenum[id] % MAX_SAVES)].snum = savenum[id];
	savenum[id]++;
	Plugin_Scr_AddInt(((savenum[id] - 1) % MAX_SAVES));
}

int mysql_async_query_initializer(char*, bool);

void gsc_save_setcompleted(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	bool domysql = (bool)Plugin_Scr_GetInt(1);
	jh_saves[id][snum].completed = true;
	//printf("domysql: %d, cheated: %d\n", domysql, jh_saves[id][snum].cheated);
	if(domysql && !jh_saves[id][snum].cheated)
	{
		//insert into database
		int entry = 0;
		int shift = 0;
		int cpcount = 0;
		for(int i = 0; i < jh_cp_ids_count; i++)
		{
			if(jh_saves[id][snum].passed_checkpoints[entry] & (1<<shift))
				cpcount++;
			shift++;
			if(shift == (sizeof(int) * CHAR_BIT))
			{
				entry++;
				shift = 0;
			}
		}
		char q[1024];
		sprintf(q, "INSERT INTO player_saves (run_id, save_id, coord_x, coord_y, coord_z, ang_x, ang_y, ang_z, save_count, load_count, time_played, nadejumps, nadethrows, jump_count, distance_travelled, funmode, os_mode, fps, loadout, checkpoint_count) VALUES (%d, %d, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %d, %d, %d, %d, %d, %d, %.1f, %d, %d, %d, '%s:%s', %d)", jh_saves[id][snum].run_id, jh_saves[id][snum].snum, jh_saves[id][snum].origin[0], jh_saves[id][snum].origin[1], jh_saves[id][snum].origin[2] + 1, jh_saves[id][snum].angles[0], jh_saves[id][snum].angles[1], jh_saves[id][snum].angles[2], jh_saves[id][snum].save_count, jh_saves[id][snum].load_count, jh_saves[id][snum].time_played, jh_saves[id][snum].nadejumps, jh_saves[id][snum].nadethrows, jh_saves[id][snum].jump_count, jh_saves[id][snum].distance_travelled, jh_saves[id][snum].funmode, jh_saves[id][snum].os_mode, jh_saves[id][snum].fps,jh_saves[id][snum].loadout[0], jh_saves[id][snum].loadout[1], cpcount);
		//add q to mysql waiting list
		mysql_async_query_initializer(q, false);
		entry = 0;
		shift = 0;
		bool added = false;
		sprintf(q, "%s", "INSERT INTO player_saves_passed_checkpoints (run_id, save_id, cp_id) VALUES ");
		int qlen = strlen(q);
		for(int i = 0; i < jh_cp_ids_count; i++)
		{
			if(jh_saves[id][snum].passed_checkpoints[entry] & (1<<shift))
			{
				char sq[64];
				if(added)
					sprintf(sq, ", (%d, %d, %d)", jh_saves[id][snum].run_id, jh_saves[id][snum].snum, jh_cp_ids[i]);
				else
					sprintf(sq, "(%d, %d, %d)", jh_saves[id][snum].run_id, jh_saves[id][snum].snum, jh_cp_ids[i]);
				qlen += sprintf(q + qlen, "%s", sq);
				added = true;
				if(qlen >= sizeof(q) - sizeof(sq) - 1)
				{
					mysql_async_query_initializer(q, false);
					sprintf(q, "%s", "INSERT INTO player_saves_passed_checkpoints (run_id, save_id, cp_id) VALUES ");
					qlen = strlen(q);
					added = false;
				}
			}
			shift++;
			if(shift == (sizeof(int) * CHAR_BIT))
			{
				entry++;
				shift = 0;
			}
		}
		if(added)
			mysql_async_query_initializer(q, false);
		entry = 0;
		shift = 0;
		added = false;
		sprintf(q, "%s", "INSERT INTO player_saves_cps (run_id, save_id, cp_id) VALUES ");
		qlen = strlen(q);
		for(int i = 0; i < jh_cp_ids_count; i++)
		{
			if(jh_saves[id][snum].cps[entry] & (1<<shift))
			{
				char sq[64];
				if(added)
					sprintf(sq, ", (%d, %d, %d)", jh_saves[id][snum].run_id, jh_saves[id][snum].snum, jh_cp_ids[i]);
				else
					sprintf(sq, "(%d, %d, %d)", jh_saves[id][snum].run_id, jh_saves[id][snum].snum, jh_cp_ids[i]);
				qlen += sprintf(q + qlen, "%s", sq);
				added = true;
				if(qlen >= sizeof(q) - sizeof(sq) - 1)
				{
					mysql_async_query_initializer(q, false);
					sprintf(q, "%s", "INSERT INTO player_saves_cps (run_id, save_id, cp_id) VALUES ");
					qlen = strlen(q);
					added = false;
				}
			}
			shift++;
			if(shift == (sizeof(int) * CHAR_BIT))
			{
				entry++;
				shift = 0;
			}
		}
		if(added)
			mysql_async_query_initializer(q, false);
		entry = 0;
		shift = 0;
		added = false;
		sprintf(q, "%s", "INSERT INTO player_saves_triggers (run_id, save_id, trigger) VALUES ");
		qlen = strlen(q);
		for(int i = 0; i < jh_triggers_count; i++)
		{
			if(jh_saves[id][snum].triggers[entry] & (1<<shift))
			{
				char sq[128];
				if(added)
					sprintf(sq, ", (%d, %d, '%s')", jh_saves[id][snum].run_id, jh_saves[id][snum].snum, jh_triggers[i]);
				else
					sprintf(sq, "(%d, %d, '%s')", jh_saves[id][snum].run_id, jh_saves[id][snum].snum, jh_triggers[i]);
				qlen += sprintf(q + qlen, "%s", sq);
				added = true;
				if(qlen >= sizeof(q) - sizeof(sq) - 1)
				{
					mysql_async_query_initializer(q, false);
					sprintf(q, "%s", "INSERT INTO player_saves_triggers (run_id, save_id, trigger) VALUES ");
					qlen = strlen(q);
					added = false;
				}
			}
			shift++;
			if(shift == (sizeof(int) * CHAR_BIT))
			{
				entry++;
				shift = 0;
			}
		}
		if(added)
			mysql_async_query_initializer(q, false);
	}
}

void gsc_save_loadpos(int id)
{
	int backcount = Plugin_Scr_GetInt(0);
	int snum = ((savenum[id] - 1) % MAX_SAVES) - backcount;
	if(snum < 0)
		snum += MAX_SAVES;
	if(snum < 0 || backcount >= MAX_SAVES || !jh_saves[id][snum].completed)
	{
		//too far back or non-existent
		Plugin_Scr_AddInt(-1);
	}
	else
		Plugin_Scr_AddInt(snum);
}

void gsc_save_setentity(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	char *ent = Plugin_Scr_GetString(1);
	int entnum = Plugin_Scr_GetInt(2);
	if(ent[0] == '\0')
		jh_saves[id][snum].entity[0] = '\0';
	else
	{
		strncpy(jh_saves[id][snum].entity, ent, sizeof(jh_saves[0][0].entity) - 1);
		jh_saves[id][snum].entity[sizeof(jh_saves[0][0].entity) - 1] = '\0';
		jh_saves[id][snum].entnum = entnum;
	}
}

void gsc_save_getentity(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_MakeArray();
	Plugin_Scr_AddString(jh_saves[id][snum].entity);
	Plugin_Scr_AddArray();
	Plugin_Scr_AddInt(jh_saves[id][snum].entnum);
	Plugin_Scr_AddArray();
}

void gsc_save_getentitynum(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddInt(jh_saves[id][snum].entnum);
}

void gsc_save_setcheated(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].cheated = (bool) Plugin_Scr_GetInt(1);
}

void gsc_save_getcheated(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddInt(jh_saves[id][snum].cheated);
}

void gsc_save_getfps(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddInt(jh_saves[id][snum].fps);
}

void gsc_save_getnadejumps(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddInt(jh_saves[id][snum].nadejumps);
}

void gsc_save_getrpg_out(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddInt(jh_saves[id][snum].rpg_out);
}

void gsc_save_setcansave(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].cansave = (bool) Plugin_Scr_GetInt(1);
}

void gsc_save_getcansave(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddInt(jh_saves[id][snum].cansave);
}

void gsc_save_setbigjump(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].bigjump = Plugin_Scr_GetInt(1);
}

void gsc_save_getbigjump(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddInt(jh_saves[id][snum].bigjump);
}

void gsc_save_setjetpack(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	jh_saves[id][snum].jetpack = (bool) Plugin_Scr_GetInt(1);
}

void gsc_save_getjetpack(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddInt(jh_saves[id][snum].jetpack);
}

void gsc_save_setloadout(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	int slotnum = Plugin_Scr_GetInt(1);
	char *loadout = "";
	if(Plugin_Scr_GetType(2) == 2)
		loadout = Plugin_Scr_GetString(2);
	strncpy(jh_saves[id][snum].loadout[slotnum], loadout, sizeof(jh_saves[0][0].loadout[0]) - 1);
	jh_saves[id][snum].loadout[slotnum][sizeof(jh_saves[0][0].loadout[0]) - 1] = '\0';
}

void gsc_save_getloadout(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_MakeArray();
	Plugin_Scr_AddString(jh_saves[id][snum].loadout[0]);
	Plugin_Scr_AddArray();
	Plugin_Scr_AddString(jh_saves[id][snum].loadout[1]);
	Plugin_Scr_AddArray();
}

void gsc_save_setorigin(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	vec3_t org;
	Plugin_Scr_GetVector(1, org);
	memcpy(jh_saves[id][snum].origin, org, sizeof(vec3_t));
}

void gsc_save_getorigin(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddVector(jh_saves[id][snum].origin);
}

void gsc_save_setangles(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	vec3_t ang;
	Plugin_Scr_GetVector(1, ang);
	memcpy(jh_saves[id][snum].angles, ang, sizeof(vec3_t));
}

void gsc_save_getangles(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_AddVector(jh_saves[id][snum].angles);
}

void gsc_save_addtrigger(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	char * trigger = Plugin_Scr_GetString(1);
	int cnum = -1;
	for(int i = 0; i < jh_triggers_count; i++)
	{
		if(!strcmp(jh_triggers[i], trigger))
		{
			cnum = i;
			break;
		}
	}
	if(cnum == -1)
	{
		//register the checkpoint
		
		if(jh_triggers_count >= MAX_TRIGGER_COUNT)
		{
			Plugin_Scr_AddInt(-1);
			return;
		}
		else
		{
			cnum = jh_triggers_count;
			strncpy(jh_triggers[jh_triggers_count], trigger, sizeof(jh_triggers[0]) - 1);
			jh_triggers[jh_triggers_count][sizeof(jh_triggers[0]) - 1] = '\0';
			jh_triggers_count++;
		}
	}
	int entry = (int) cnum / (sizeof(int) * CHAR_BIT);
	int shift = cnum % (sizeof(int) * CHAR_BIT);
	int mask = 1<<shift;
	jh_saves[id][snum].triggers[entry] |= mask;
	Plugin_Scr_AddInt(0);
}

void gsc_save_gettriggers(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_MakeArray();
	int entry = 0;
	int shift = 0;
	for(int i = 0; i < jh_triggers_count; i++)
	{
		if(jh_saves[id][snum].triggers[entry] & (1<<shift))
		{
			Plugin_Scr_AddString(jh_triggers[i]);
			Plugin_Scr_AddArray();
		}
		shift++;
		if(shift == (sizeof(int) * CHAR_BIT))
		{
			entry++;
			shift = 0;
		}
	}
}

void gsc_save_addcp(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	int cp_id = Plugin_Scr_GetInt(1);
	int cnum = -1;
	for(int i = 0; i < jh_cp_ids_count; i++)
	{
		if(jh_cp_ids[i] == cp_id)
		{
			cnum = i;
			break;
		}
	}
	if(cnum == -1)
	{
		//register the checkpoint
		if(jh_cp_ids_count >= MAX_CP_COUNT)
		{
			Plugin_Scr_AddInt(-1);
			return;
		}
		else
		{
			cnum = jh_cp_ids_count;
			jh_cp_ids[jh_cp_ids_count] = cp_id;
			jh_cp_ids_count++;
		}
	}
	int entry = (int) cnum / (sizeof(int) * CHAR_BIT);
	int shift = cnum % (sizeof(int) * CHAR_BIT);
	int mask = 1<<shift;
	jh_saves[id][snum].cps[entry] |= mask;
	Plugin_Scr_AddInt(0);
}

void gsc_save_getcps(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_MakeArray();
	int entry = 0;
	int shift = 0;
	for(int i = 0; i < jh_cp_ids_count; i++)
	{
		if(jh_saves[id][snum].cps[entry] & (1<<shift))
		{
			Plugin_Scr_AddInt(jh_cp_ids[i]);
			Plugin_Scr_AddArray();
		}
		shift++;
		if(shift == (sizeof(int) * CHAR_BIT))
		{
			entry++;
			shift = 0;
		}
	}
}

void gsc_save_addpassed_checkpoint(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	int cp_id = Plugin_Scr_GetInt(1);
	int cnum = -1;
	for(int i = 0; i < jh_cp_ids_count; i++)
	{
		if(jh_cp_ids[i] == cp_id)
		{
			cnum = i;
			break;
		}
	}
	if(cnum == -1)
	{
		//register the checkpoint
		if(jh_cp_ids_count >= MAX_CP_COUNT)
		{
			Plugin_Scr_AddInt(-1);
			return;
		}
		else
		{
			cnum = jh_cp_ids_count;
			jh_cp_ids[jh_cp_ids_count] = cp_id;
			jh_cp_ids_count++;
		}
	}
	int entry = (int) cnum / (sizeof(int) * CHAR_BIT);
	int shift = cnum % (sizeof(int) * CHAR_BIT);
	int mask = 1<<shift;
	jh_saves[id][snum].passed_checkpoints[entry] |= mask;
	Plugin_Scr_AddInt(0);
}

void gsc_save_getpassed_checkpoints(int id)
{
	int snum = Plugin_Scr_GetInt(0);
	Plugin_Scr_MakeArray();
	int entry = 0;
	int shift = 0;
	for(int i = 0; i < jh_cp_ids_count; i++)
	{
		if(jh_saves[id][snum].passed_checkpoints[entry] & (1<<shift))
		{
			Plugin_Scr_AddInt(jh_cp_ids[i]);
			Plugin_Scr_AddArray();
		}
		shift++;
		if(shift == (sizeof(int) * CHAR_BIT))
		{
			entry++;
			shift = 0;
		}
	}
}

void gsc_setdemorecording(int id)
{
	client_t* c = Plugin_GetClientForClientNum(id);
	c->demorecording = (qboolean)Plugin_Scr_GetInt(0);
	if(c->demorecording)
		c->demoDeltaFrameCount = 0;
}


void gsc_getQueuedReliableMessages(int id)
{
	client_t* c = Plugin_GetClientForClientNum(id);
	int count = c->reliableSequence - c->reliableAcknowledge;
	Plugin_Scr_AddInt(count);
}

void gsc_resetfps(int id)
{
	client_t* c = Plugin_GetClientForClientNum(id);
	jh_fps[id] = c->clFrames;
}

void gsc_getfps(int id)
{
	client_t* c = Plugin_GetClientForClientNum(id);
	int fps = 20 * (c->clFrames - jh_fps[id]);
	Plugin_Scr_AddInt(fps);
}

void gsc_printcommandtime(int id)
{
	//playerState_t* ps = Plugin_SV_GameClientNum(id);
	//Plugin_Printf("command time: %d\n", ps->commandTime);
	client_t* c = Plugin_GetClientForClientNum(id);
	if(c->clFPS == 0)
		c->clFPS = Plugin_Scr_GetInt(0);
	else
	{
		c->clFrameCalcTime += Plugin_Scr_GetInt(0) - c->clFPS;
		c->clFPS = 0;
	}
}

void Gsc_Player_Velocity_Set(int id) {
    vec3_t velocity;
    playerState_t* ps;

    Plugin_Scr_GetVector(0, velocity);

    ps = Plugin_SV_GameClientNum(id);

    ps->velocity[0] = velocity[0];
    ps->velocity[1] = velocity[1];
    ps->velocity[2] = velocity[2];

    Plugin_Scr_AddInt(1);
}

void Gsc_Player_Velocity_Add(int id) {
    vec3_t velocity;
    playerState_t* ps;

    Plugin_Scr_GetVector(0, velocity);

    ps = Plugin_SV_GameClientNum(id);

    ps->velocity[0] += velocity[0];
    ps->velocity[1] += velocity[1];
    ps->velocity[2] += velocity[2];

    Plugin_Scr_AddInt(1);
}

void Gsc_Player_ButtonAds(int id) {
    int currentPlayer = playerStates + id * sizeOfPlayer;
    unsigned char *aim_address = (unsigned char *)(currentPlayer + 0x26CD);
    int aimButtonPressed = *aim_address & 0xF0; // just the first 4 bits tell the state
    Plugin_Scr_AddInt(aimButtonPressed);
}

void Gsc_Player_ButtonLeft(int id) {

    unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA7);

    int leftButtonPressed = (*aim_address & 0x81)==0x81;
    Plugin_Scr_AddInt(leftButtonPressed);
}

void Gsc_Player_ButtonRight(int id) {

    unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA7);

    int rightButtonPressed = (*aim_address & 0x7F)==0x7F;
    Plugin_Scr_AddInt(rightButtonPressed);
}

void Gsc_Player_ButtonForward(int id) {

    unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA6);

    int forwardButtonPressed = (*aim_address & 0x7F)==0x7F;
    Plugin_Scr_AddInt(forwardButtonPressed);
}

void Gsc_Player_ButtonBack(int id) {

    unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FA6);

    int backButtonPressed = (*aim_address & 0x81)==0x81;
    Plugin_Scr_AddInt(backButtonPressed);
}

void Gsc_Player_ButtonLeanLeft(int id) {

    unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB4);

    int leanleftButtonPressed = (*aim_address & 0x40)==0x40;
    Plugin_Scr_AddInt(leanleftButtonPressed);
}

void Gsc_Player_ButtonLeanRight(int id) {

    unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB4);
    
    int leanrightButtonPressed = (*aim_address & 0x80)==0x80;
    Plugin_Scr_AddInt(leanrightButtonPressed);
}

void Gsc_Player_ButtonJump(int id) {

    unsigned char *aim_address = (unsigned char *)(PLAYERSTATE(id) + 0x2FB5);
    
    int jumpButtonPressed = (*aim_address & 0x04)==0x04;
    Plugin_Scr_AddInt(jumpButtonPressed);
}

void Gsc_Player_SpectatorClientGet(int id) {
    gentity_t* gentity = Plugin_GetGentityForEntityNum(id);

    if(gentity->client->sess.spectatorClient == -1)
        Plugin_Scr_AddEntity(Plugin_GetGentityForEntityNum(id));
    else
        Plugin_Scr_AddEntity(Plugin_GetGentityForEntityNum(gentity->client->sess.spectatorClient));
}

void Gsc_Player_GetIP(int id){
    char* ip;
    client_t* cl;

    cl = Plugin_GetClientForClientNum(id);

    ip = (char*)Plugin_NET_AdrToString( &cl->netchan.remoteAddress );

    Plugin_Scr_AddString(ip);
}

void gsc_player_issprinting(int id)
{
	playerState_t* ps = Plugin_SV_GameClientNum(id);
	if(ps->sprintState.lastSprintStart <= ps->sprintState.lastSprintEnd)
		Plugin_Scr_AddInt(0);
	else
		Plugin_Scr_AddInt(1);
}
