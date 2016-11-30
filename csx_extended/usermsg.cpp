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

#include "amxxmodule.h"
#include "CHeader.h"

weaponsVault weaponData[MAX_WEAPONS + MAX_CWEAPONS];

int32_t damage = 0;
uint8_t weapon = 0;
uint8_t aim = 0;
bool ignorehit = false;
CPlayer *pAttacker;

void Client_DeathMsg(void *mValue)
{
	static CPlayer* victim;
	static CPlayer* killer;
	static uint8_t victim_id;
	static uint8_t killer_id;
	static uint8_t killer_hs;
	static uint8_t killer_weapon;
	static uint8_t killer_tk;
	static bool is_headshot;
	static const char* tw_name;

	switch (mState++)
	{
		case 0:
		{
			killer_id = *(uint8_t *)mValue;
			break;
		}
		case 1:
		{
			victim_id = *(uint8_t *)mValue;	// same as mPlayer->index
			break;
		}
		case 2:
		{
			is_headshot = *(uint8_t *)mValue ? true : false;
			break;
		}
		case 3:
		{
			// DeathMsg occurs before Round End LogEvent and Damage Event
			// So, it can detect deaths faster and accurately
			if (!victim_id)
				break;
			victim = GET_PLAYER_POINTER_I(victim_id);
			if (killer_id && killer_id != victim_id)
			{
				killer = GET_PLAYER_POINTER_I(killer_id);

				tw_name = (const char *)mValue;
				if (!strcmp(tw_name, "knife"))
					killer->aiming = (uint8_t)is_headshot ? 1 : 0;

				killer_weapon = (uint8_t)killer->current;
				killer_hs = (uint8_t)(killer->aiming == 1) ? 1 : 0;
				killer_tk = (uint8_t)(killer->teamId == victim->teamId) ? 1 : 0;

				if (killer_weapon != CSW_C4	|| !killer_tk)
					killer->saveKill(victim, killer_weapon, killer_hs, killer_tk);
			}
			else
				victim->saveKill(victim, 0, 0, 0);
			break;
		}
	}
}

void Client_WeaponList(void* mValue)
{
	static size_t wpnList;
	static uint8_t iSlot;
	static const char* wpnName;

	switch (mState++)
	{
		case 0:
		{
			wpnName = (const char*)mValue;
			break;
		}
		case 1:
		{
			iSlot = *(uint8_t*)mValue;
			break;
		}
		case 7:
		{
			uint8_t iId = *(uint8_t*)mValue;
			if (!iId || iId >= MAX_WEAPONS || (wpnList & (1 << iId)))
				break;
			
			wpnList |= (1 << iId);
			weaponData[iId].ammoSlot = iSlot;

			if (strstr(wpnName, "weapon_"))
			{
				if (strcmp(wpnName + 7, "hegrenade") == 0)
					strcpy(weaponData[iId].name, wpnName + 9);
				else
					strcpy(weaponData[iId].name, wpnName + 7);
				strcpy(weaponData[iId].logname, weaponData[iId].name);
			}
		}
	}
}

void Client_Damage(void* mValue)
{
	static uint8_t bits;
	switch (mState++)
	{
		case 1:
		{
			ignorehit = false;
			damage = *(int32_t*)mValue;
			break;
		}
		case 2:
		{
			bits = *(uint8_t*)mValue;
			break;
		}
		case 3:
		{
			if (mPlayer == NULL || damage < 1 || bits)
			{
				ignorehit = true;
				break;
			}

			edict_t *enemy;
			enemy = mPlayer->pEdict->v.dmg_inflictor;

			if (FNullEnt(enemy))
			{
				ignorehit = true;
				break;
			}

			aim = 0;
			weapon = 0;
			pAttacker = NULL;

			if (enemy->v.flags & (FL_CLIENT | FL_FAKECLIENT))
			{
				pAttacker = GET_PLAYER_POINTER(enemy);
				aim = pAttacker->aiming;
				weapon = pAttacker->current;
				pAttacker->saveHit(mPlayer, weapon, damage, aim);
				break;
			}
			if (g_grenades.find(enemy, &pAttacker, &weapon))
				pAttacker->saveHit(mPlayer, weapon, damage, aim);
			else if (strcmp("grenade", STRING(enemy->v.classname)) == 0) // ? more checks ?
				weapon = CSW_C4;
		}
	}
}

void Client_Damage_End(void* mValue)
{
	if (ignorehit)
		return;

	uint8_t TA = 0; // why make it global?

	if (pAttacker == NULL)
		pAttacker = mPlayer;

	if ((mPlayer->teamId == pAttacker->teamId) && (mPlayer != pAttacker))
		TA = 1;

	MF_ExecuteForward(iFDamage,	\
		static_cast<cell>(pAttacker->index), \
		static_cast<cell>(mPlayer->index), \
		static_cast<cell>(damage), \
		static_cast<cell>(weapon), \
		static_cast<cell>(aim), \
		static_cast<cell>(TA));

	if (!IsAlive(mPlayer->pEdict))
	{
		MF_ExecuteForward(iFDeath, \
			static_cast<cell>(pAttacker->index), \
			static_cast<cell>(mPlayer->index), \
			static_cast<cell>(weapon), \
			static_cast<cell>(aim), \
			static_cast<cell>(TA));
	}
}

void Client_CurWeapon(void* mValue)
{
	static uint8_t iState;
	static uint8_t iId;
	switch (mState++)
	{
		case 0:
		{
			iState = *(uint8_t*)mValue;
			break;
		}
		case 1:
		{
			if (!iState)
				break;
			iId = *(uint8_t*)mValue;
			break;
		}
		case 2:
		{
			if (mPlayer == NULL || !iState)
				break;
			char iClip = *(char*)mValue;
			if (iClip != -1 && iClip < mPlayer->weapons[iId].clip)
				mPlayer->saveShot(iId);
			mPlayer->weapons[iId].clip = iClip;
			mPlayer->current = iId;
		}
	}
}

void Client_AmmoX(void* mValue)
{
	static uint8_t iAmmo;
	switch (mState++)
	{
		case 0:
		{
			iAmmo = *(uint8_t*)mValue;
			break;
		}
		case 1:
		{
			if (mPlayer == NULL)
				break;
			for (uint8_t i = 1; i < MAX_WEAPONS; ++i)
				if (iAmmo == weaponData[i].ammoSlot)
					mPlayer->weapons[i].ammo = *(uint8_t*)mValue;
		}
	}
}

void Client_AmmoPickup(void* mValue)
{
	static uint8_t iSlot;
	switch (mState++)
	{
		case 0:
		{
			iSlot = *(uint8_t*)mValue;
			break;
		}
		case 1:
		{
			if (mPlayer == NULL)
				break;
			for (int i = 1; i < MAX_WEAPONS; ++i)
				if (weaponData[i].ammoSlot == iSlot)
					mPlayer->weapons[i].ammo += *(uint8_t*)mValue;
		}
	}
}

void Client_ScoreInfo(void* mValue)
{
	static uint8_t index;
	switch (mState++)
	{
		case 0:
		{
			index = *(uint8_t*)mValue;
			break;
		}
		case 4:
		{
			if (index > 0 && index <= gpGlobals->maxClients)
				GET_PLAYER_POINTER_I(index)->teamId = *(uint8_t*)mValue;
		}
	}
}

void Client_SendAudio(void* mValue)
{
	static const char* szText;
	if (mState == 1)
	{
		szText = (const char*)mValue;
		
		if (mPlayer == NULL && szText[7] == 'B')
		{
			if (szText[11] == 'P' && g_Planter)
			{
				GET_PLAYER_POINTER_I(g_Planter)->saveBPlant();
				g_bombAnnounce = BOMB_PLANTED;
			}
			else if (szText[11] == 'D' && g_Defuser)
			{
				GET_PLAYER_POINTER_I(g_Defuser)->saveBDefused();
				g_bombAnnounce = BOMB_DEFUSED;
			}
		}
	}
	mState++;
}

void Client_TextMsg(void* mValue)
{
	static const char* szText;
	if (mPlayer == NULL && mState == 1)
	{
		szText = (const char*)mValue;
		if (szText[1] == 'T' && szText[8] == 'B' && g_Planter)
		{
			GET_PLAYER_POINTER_I(g_Planter)->saveBExplode();
			g_bombAnnounce = BOMB_EXPLODE;
		}
	}
	mState++;
}

void Client_BarTime(void* mValue)
{
	uint16_t iTime = *(uint16_t*)mValue;
	if (!iTime || !IsAlive(mPlayer->pEdict))
		return;
	if (iTime == 3)
	{
		g_Planter = mPlayerIndex;
		g_bombAnnounce = BOMB_PLANTING;
		g_Defuser = 0;
	}
	else
	{
		mPlayer->saveBDefusing();
		g_Defuser = mPlayerIndex;
		g_bombAnnounce = BOMB_DEFUSING;
	}
}
