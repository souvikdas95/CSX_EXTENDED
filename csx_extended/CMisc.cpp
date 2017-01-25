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

#include "CHeader.h"

// *****************************************************
// class Grenades
// *****************************************************
void Grenades::put(edict_t* grenade, float time, uint8_t type, CPlayer* player)
{
	Obj* a = new Obj;
	if(a == nullptr)
		return;
	a->player = player;
	a->grenade = grenade;
	a->time = gpGlobals->time + time;
	a->type = type;
	a->prev = 0;
	a->next = head;
	if(head != nullptr)
		head->prev = a;
	head = a;
}

bool Grenades::find(edict_t* enemy, CPlayer** p, uint8_t* type)
{
	bool found = false;
	Obj* a = head;
	while(a != nullptr)
	{
		if(a->time > gpGlobals->time && !found)
		{
			if(a->grenade == enemy)
			{
				found = true;
				*p = a->player;
				*type = a->type;
			}
		}
		else
		{
			Obj* next = a->next;
			if(a->prev != nullptr)
				a->prev->next = next;
			else
				head = next;
			if(next != nullptr)
				next->prev = a->prev;
			delete a;
			a = next;
			continue;
		}
		a = a->next;
	}

	return found;
}

void Grenades::clear()
{
	while(head)
	{
		Obj* a = head->next;
		delete head;
		head = a;
	}
}

// *****************************************************
// structure CPlayer
// *****************************************************
void CPlayer::Disconnect()
{
	// Set In-Game False
	ingame = false;
	
	// ignore if he is bot and bots rank is disabled or module is paused
	if((canIgnoreBots() && bot) || !isModuleActive())
		return;

	// Just a sanity check
	if(rank != nullptr)
	{
		rank->updatePosition(&life);
		rank = nullptr;
	}
}

void CPlayer::PutInServer()
{
	// In-Game?
	ingame = true;
	
	// Ignore Bots?
	if(canIgnoreBots() && bot)
		return;

	// Clear Player Stats
	restartStats();

	// Nominate Unique Identification for Player
	const char* name = STRING(pEdict->v.netname);
	const char* unique = nullptr;
	switch((uint8_t)(csstats_rank->value))
	{
		case 0:
		{
			unique = name;
		} break;
		case 1:
		{
			if(authid == nullptr || !IsValidAuth(authid))
				unique = name;
			else
				unique = authid;
		} break;
		case 2:
		{
			if(ip == nullptr)
				unique = name;
			else
				unique = ip;
		} break;
		default:
		{
			unique = name;
		}
	}

	// Allocate Player Stats
	rank = g_rank.findEntryInRank(unique);
	if(rank == nullptr)
	{
		rank = g_rank.newEntryInRank(unique, name);
		if(rank != nullptr)
			rank->updatePosition();
		else
		{
			MF_SyncLog("PutInServer: Unable to load Stats of player \"%s\" on Server", name);
			return;
		}
	}
	else if(unique != name && strcmp(rank->getName(), name) != 0)
		rank->setName(name);
}

void CPlayer::Connect(const char* address)
{
	// Just a sanity check
	if(rank != nullptr)
	{
		rank->updatePosition(&life);
		rank = nullptr;
	}

	// Cache BOT parameter
	bot = IsBot(pEdict);

	// Cache AuthID
	authid = strdup(GETPLAYERAUTHID(pEdict));

	// Cache IP
	ip = strdup(address);
	uint8_t len = (uint8_t)strlen(ip);
	while(--len)
		if(ip[len] == ':')
		{
			ip[len] = '\0';
			break;
		}
}

void CPlayer::restartStats(bool all)
{
	if(all)
		memset(weapons, 0, sizeof(weapons));
	memset(weaponsRnd, 0, sizeof(weaponsRnd));   //DEC-Weapon (Round) stats
	memset(attackers, 0, sizeof(attackers));
	memset(victims, 0, sizeof(victims));
	memset(&life, 0, sizeof(life));
}

void CPlayer::Init(uint8_t pi, edict_t* pe)
{
	pEdict = pe;
	index = pi;
	current = 0;
	rank = nullptr;
}

void CPlayer::saveKill(CPlayer* pVictim, uint8_t wweapon, uint8_t hhs, uint8_t ttk)
{
	if(!isModuleActive() || (canIgnoreBots() && (bot || IsBot(pVictim->pEdict))))
		return;

	if(pVictim->index == index) // killed self
	{
		pVictim->weapons[0].deaths++;
		pVictim->life.deaths++;
		pVictim->weaponsRnd[0].deaths++;       // DEC-Weapon (round) stats
		return;
	}

	pVictim->attackers[index].name = weaponData[wweapon].name;
	pVictim->attackers[index].kills++;
	pVictim->attackers[index].hs += hhs;
	pVictim->attackers[index].tks += ttk;
	pVictim->attackers[0].kills++;
	pVictim->attackers[0].hs += hhs;
	pVictim->attackers[0].tks += ttk;
	pVictim->weapons[pVictim->current].deaths++;
	pVictim->weapons[0].deaths++;
	pVictim->life.deaths++;

	pVictim->weaponsRnd[pVictim->current].deaths++; // DEC-Weapon (round) stats
	pVictim->weaponsRnd[0].deaths++;                   // DEC-Weapon (round) stats

	uint8_t vi = pVictim->index;
	victims[vi].name = weaponData[wweapon].name;
	victims[vi].deaths++;
	victims[vi].hs += hhs;
	victims[vi].tks += ttk;
	victims[0].deaths++;
	victims[0].hs += hhs;
	victims[0].tks += ttk;

	weaponsRnd[wweapon].kills++;                // DEC-Weapon (round) stats
	weaponsRnd[wweapon].hs += hhs;         // DEC-Weapon (round) stats
	weaponsRnd[wweapon].tks += ttk;     // DEC-Weapon (round) stats
	weaponsRnd[0].kills++;                     // DEC-Weapon (round) stats
	weaponsRnd[0].hs += hhs;              // DEC-Weapon (round) stats
	weaponsRnd[0].tks += ttk;          // DEC-Weapon (round) stats

	weapons[wweapon].kills++;
	weapons[wweapon].hs += hhs;
	weapons[wweapon].tks += ttk;
	weapons[0].kills++;
	weapons[0].hs += hhs;
	weapons[0].tks += ttk;
	life.kills++;
	life.hs += hhs;
	life.tks += ttk;
}

void CPlayer::saveHit(CPlayer* pVictim, uint8_t wweapon, int32_t ddamage, uint8_t bbody)
{
	if(!isModuleActive() || (canIgnoreBots() && (bot || IsBot(pVictim->pEdict))))
		return;

	if(index == pVictim->index)
		return;

	pVictim->attackers[index].hits++;
	pVictim->attackers[index].damage += ddamage;
	pVictim->attackers[index].bodyHits[bbody]++;
	pVictim->attackers[0].hits++;
	pVictim->attackers[0].damage += ddamage;
	pVictim->attackers[0].bodyHits[bbody]++;

	uint8_t vi = pVictim->index;
	victims[vi].hits++;
	victims[vi].damage += ddamage;
	victims[vi].bodyHits[bbody]++;
	victims[0].hits++;
	victims[0].damage += ddamage;
	victims[0].bodyHits[bbody]++;

	weaponsRnd[wweapon].hits++;              // DEC-Weapon (round) stats
	weaponsRnd[wweapon].damage += ddamage;    // DEC-Weapon (round) stats
	weaponsRnd[wweapon].bodyHits[bbody]++;   // DEC-Weapon (round) stats
	weaponsRnd[0].hits++;                   // DEC-Weapon (round) stats
	weaponsRnd[0].damage += ddamage;         // DEC-Weapon (round) stats
	weaponsRnd[0].bodyHits[bbody]++;        // DEC-Weapon (round) stats

	weapons[wweapon].hits++;
	weapons[wweapon].damage += ddamage;
	weapons[wweapon].bodyHits[bbody]++;
	weapons[0].hits++;
	weapons[0].damage += ddamage;
	weapons[0].bodyHits[bbody]++;

	life.hits++;
	life.damage += ddamage;
	life.bodyHits[bbody]++;
}


void CPlayer::saveShot(uint8_t weapon)
{
	if(!isModuleActive() || (canIgnoreBots() && bot))
		return;

	victims[0].shots++;
	weapons[weapon].shots++;
	weapons[0].shots++;
	life.shots++;
	weaponsRnd[weapon].shots++;       // DEC-Weapon (round) stats
	weaponsRnd[0].shots++;            // DEC-Weapon (round) stats
}

void CPlayer::saveBPlant()
{
	if(!isModuleActive())
		return;
	life.bPlants++;
}

void CPlayer::saveBExplode()
{
	if(!isModuleActive())
		return;
	life.bExplosions++;
}

void CPlayer::saveBDefusing()
{
	if(!isModuleActive())
		return;
	life.bDefusions++;
}

void CPlayer::saveBDefused()
{
	if(!isModuleActive())
		return;
	life.bDefused++;
}

void CPlayer::setScore(short int a, short int b) // This will update the Score Properly
{
	if(a >= 0)
		pEdict->v.frags = a;
	if(b >= 0)
		*((int *)pEdict->pvPrivateData + OFFSET_CSDEATHS) = b;
	MESSAGE_BEGIN(MSG_BROADCAST, GET_USER_MSG_ID(PLID, "ScoreInfo", nullptr));
	WRITE_BYTE(index);
	WRITE_SHORT((short int)pEdict->v.frags);
	WRITE_SHORT(*((int *)pEdict->pvPrivateData + OFFSET_CSDEATHS));
	WRITE_SHORT(0);
	WRITE_SHORT(*((int *)pEdict->pvPrivateData + OFFSET_TEAM));
	MESSAGE_END();
}

// **********************************
// ****** NON MEMBER FUNCTIONS ******
// **********************************
inline const bool IsBot(edict_t *pEnt)
{
	const char* auth = GETPLAYERAUTHID(pEnt);
	return ((auth != nullptr && !strcmp(auth, "BOT")) || (pEnt->v.flags & FL_FAKECLIENT));
}

inline const bool IsAlive(edict_t *pEnt)
{
	return ((pEnt->v.deadflag == DEAD_NO) && (pEnt->v.health > 0));
}

inline const bool canIgnoreBots()
{
	return ((uint8_t)csstats_rankbots->value) ? false : true;
}

inline const bool isModuleActive()
{
	return ((uint8_t)csstats_pause->value) ? false : true;
}
