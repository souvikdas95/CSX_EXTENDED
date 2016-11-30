// vim: set ts=4 sw=4 tw=99 noet:
//
// AMX Mod X, based on AMX Mod by Aleksander Naszko ("OLO").
// Copyright (C) The AMX Mod X Development Team.
//
// This software is licensed under the GNU General Public License, version 3 or higher.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     https://alliedmods.net/amxmodx-license

//
// CSX Module
//

#ifndef RANK_H
#define RANK_H

#include "amxxmodule.h"
#include "CMisc.h"
#include "CRank.h"

typedef unsigned char      uint8_t;

#if defined(__linux__) || defined (__APPLE__)
#define EXTRAOFFSET					5
#else
#define EXTRAOFFSET					0
#endif
#if !defined __amd64__
#define OFFSET_TEAM					114 + EXTRAOFFSET
#define OFFSET_CSDEATHS				444 + EXTRAOFFSET
#else
#define OFFSET_TEAM					139 + EXTRAOFFSET
#define OFFSET_CSDEATHS				493 + EXTRAOFFSET
#endif

#if defined(__linux__) || defined (__APPLE__)
#include <sys/mman.h>
#include <malloc.h>

#define VirtFuncSpawn           2

#define PrivateToEdict(pPrivate) (*(entvars_t **)pPrivate)->pContainingEntity
#else
#define VirtFuncSpawn           0

#define PrivateToEdict(pPrivate) (*(entvars_t **)((char*)pPrivate + 4))->pContainingEntity
#endif

#ifndef MAX_PLAYERS
#define MAX_PLAYERS 32
#endif

#define GET_PLAYER_POINTER(e)   (&players[ENTINDEX(e)])
#define GET_PLAYER_POINTER_I(i) (&players[i])

#define CHECK_ENTITY(x) \
	if (x < 0 || x > gpGlobals->maxEntities) { \
		MF_LogError(amx, AMX_ERR_NATIVE, "Entity out of range (%d)", x); \
		return 0; \
	} else { \
		if (x <= gpGlobals->maxClients) { \
			if (!MF_IsPlayerIngame(x)) { \
				MF_LogError(amx, AMX_ERR_NATIVE, "Invalid player %d (not in-game)", x); \
				return 0; \
			} \
		} else { \
			if (x != 0 && FNullEnt(INDEXENT(x))) { \
				MF_LogError(amx, AMX_ERR_NATIVE, "Invalid entity %d", x); \
				return 0; \
			} \
		} \
	}

#define CHECK_PLAYER(x) \
	if (x < 1 || x > gpGlobals->maxClients) { \
		MF_LogError(amx, AMX_ERR_NATIVE, "Player out of range (%d)", x); \
		return 0; \
	} else { \
		if (!MF_IsPlayerIngame(x) || FNullEnt(MF_GetPlayerEdict(x))) { \
			MF_LogError(amx, AMX_ERR_NATIVE, "Invalid player %d", x); \
			return 0; \
		} \
	}

#define CHECK_PLAYERRANGE(x) \
	if (x > gpGlobals->maxClients || x < 0) \
	{ \
		MF_LogError(amx, AMX_ERR_NATIVE, "Player out of range (%d)", x); \
		return 0; \
	}

#define CHECK_NONPLAYER(x) \
	if (x < 1 || x <= gpGlobals->maxClients || x > gpGlobals->maxEntities) { \
		MF_LogError(amx, AMX_ERR_NATIVE, "Non-player entity %d out of range", x); \
		return 0; \
	} else { \
		if (FNullEnt(INDEXENT(x))) { \
			MF_LogError(amx, AMX_ERR_NATIVE, "Invalid non-player entity %d", x); \
			return 0; \
		} \
	}

#define GETEDICT(n) \
	((n >= 1 && n <= gpGlobals->maxClients) ? MF_GetPlayerEdict(n) : INDEXENT(n))

#define BOMB_PLANTING	1
#define BOMB_PLANTED	2
#define BOMB_EXPLODE	3
#define BOMB_DEFUSING	4
#define BOMB_DEFUSED	5

extern void print_srvconsole(const char *fmt, ...);

struct weaponsVault
{
  char name[32];
  char logname[16];
  uint8_t ammoSlot;
  bool used;
  bool melee;
};

typedef void(*funEventCall)(void*);

extern AMX_NATIVE_INFO stats_Natives[];

extern CPlayer players[MAX_PLAYERS + 1];

extern RankSystem g_rank;

extern CPlayer* mPlayer;

extern funEventCall modMsgsEnd[MAX_REG_MSGS];
extern funEventCall modMsgs[MAX_REG_MSGS];

extern void(*function)(void*);
extern void(*endfunction)(void*);

extern uint8_t mPlayerIndex;
extern uint8_t mState;

extern Grenades g_grenades;

extern int iFGrenade;
extern int iFDeath;
extern int iFDamage;
extern int iFBPlanted;
extern int iFBDefused;
extern int iFBPlanting;
extern int iFBDefusing;
extern int iFBExplode;

extern uint8_t g_bombAnnounce;
extern uint8_t g_Planter;
extern uint8_t g_Defuser;

extern cvar_t* csstats_rankbots;
extern cvar_t* csstats_pause;
extern cvar_t* csstats_maxsize;
extern cvar_t* csstats_rank;
extern cvar_t* csstats_reset;
extern bool cvar_rankbots;
extern uint8_t cvar_rank;

extern int gmsgCurWeapon;
extern int gmsgDamageEnd;
extern int gmsgDamage;
extern int gmsgWeaponList;
extern int gmsgAmmoX;
extern int gmsgScoreInfo;
extern int gmsgAmmoPickup;
extern int gmsgSendAudio;
extern int gmsgTextMsg;
extern int gmsgBarTime;
extern int gmsgDeathMsg;

extern void Client_AmmoX(void*);
extern void Client_CurWeapon(void*);
extern void Client_Damage(void*);
extern void Client_WeaponList(void*);
extern void Client_AmmoPickup(void*);
extern void Client_Damage_End(void*);
extern void Client_ScoreInfo(void*);
extern void Client_SendAudio(void*);
extern void Client_TextMsg(void*);
extern void Client_BarTime(void*);
extern void Client_DeathMsg(void*);

extern const bool IsValidAuth(const char*);

const char* get_localinfo(const char* name, const char* def = NULL);

extern weaponsVault weaponData[MAX_WEAPONS + MAX_CWEAPONS];

extern inline const bool IsBot(edict_t *pEnt);
extern inline const bool IsAlive(edict_t *pEnt);

extern inline const bool ignoreBots();

extern inline const bool isModuleActive();

extern void MakeHookSpawn(void);
extern void HookSpawn_Post(int);
#endif // RANK_H



