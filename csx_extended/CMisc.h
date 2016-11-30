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

#ifndef CMISC_H
#define CMISC_H

#include "amxxmodule.h"
#include "CHeader.h"
#include "CRank.h"

typedef unsigned char      uint8_t;

#ifndef MAX_PLAYERS
#define MAX_PLAYERS 32
#endif

#define MAX_CWEAPONS		6

#define CSW_HEGRENADE		4
#define CSW_C4				6
#define CSW_SMOKEGRENADE	9
#define CSW_KNIFE			29
#define CSW_FLASHBANG		25

// *****************************************************
// structure CPlayer
// *****************************************************
struct CPlayer
{
	edict_t* pEdict;

	char* ip;	// Player IP
	char* authid;	// Player AuthID
	uint8_t index;	// Player ID
	uint8_t aiming;	// Body Part aimed at by weapon (Except CSW_KNIFE)
	uint8_t current;	// Current Weapon
	bool bot;	// Player Bot?

	RankSystem::RankStats* rank;	// RANKING IDENTITY of Player

	Stats life;	// CURRENT ROUND STATS of Player

	CPlayer()
	{
		ip = NULL;
		authid = NULL;
		bot = false;
		rank = NULL;
	};

	struct PlayerWeapon : Stats
	{
		const char* name;
		uint8_t		ammo;
		char		clip;	// some weapons do return -1
	};

	PlayerWeapon	weapons[MAX_WEAPONS + MAX_CWEAPONS];
	PlayerWeapon	attackers[MAX_PLAYERS + 1];	// Weapon Info of Attackers
	PlayerWeapon	victims[MAX_PLAYERS + 1];	// Weapon Info of Victims
	Stats			weaponsRnd[MAX_WEAPONS + MAX_CWEAPONS]; // DEC-Weapon (Round) stats

	uint8_t teamId;	// for distinguishing Team Attacks and for Team Stats

	void Init(uint8_t pi, edict_t* pe);
	void Connect(const char* ip);
	void PutInServer();
	void Disconnect();
	void saveKill(CPlayer* pVictim, uint8_t weapon, uint8_t hs, uint8_t tk);
	void saveHit(CPlayer* pVictim, uint8_t weapon, int32_t damage, uint8_t aiming);
	void saveShot(uint8_t weapon);

	void saveBPlant();
	void saveBExplode();
	void saveBDefusing();
	void saveBDefused();

	void restartStats(bool all = true);

	void setScore(short int a = -1, short int b = -1);
};

// *****************************************************
// class Grenades
// *****************************************************
class Grenades
{
	struct Obj
	{
		CPlayer* player;
		edict_t* grenade;
		float time;
		uint8_t type;
		Obj* next;
		Obj* prev;
	} *head;

public:
	Grenades() { head = NULL; }
	~Grenades() { clear(); }
	void put(edict_t* grenade, float time, uint8_t type, CPlayer* player);
	bool find(edict_t* enemy, CPlayer** p, uint8_t* type);
	void clear();
};
#endif // CMISC_H
