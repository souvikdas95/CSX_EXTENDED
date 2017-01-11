#include "amxxmodule.h"
#include "CHeader.h"

void *pOrigFuncSpawn_Player = NULL;
void *pOrigFuncSpawn_CSBot = NULL;

// Spawn Action
void OnSpawn(int iEnt)
{
	CPlayer *pPlayer = GET_PLAYER_POINTER_I(iEnt);

	if (!isModuleActive()	\
		|| pPlayer == NULL	\
		|| pPlayer->rank == NULL)
		return;

#ifdef DEBUG
	MF_Log("OnSpawn: Updating Stats of Player '%s'", STRING(pPlayer->pEdict->v.netname));
#endif

	pPlayer->rank->updatePosition(&pPlayer->life);
	pPlayer->restartStats(false);
}

// Player Spawn Hook
#if defined(__linux__) || defined (__APPLE__)
void HookSpawn_Player(void *pthis)
#else
void __fastcall HookSpawn_Player(void *pthis)
#endif
{
	int iEnt = ENTINDEX(PrivateToEdict(pthis));

	//Pre Hook

#if defined(__linux__) || defined (__APPLE__)
	reinterpret_cast<int(*)(void *)>(pOrigFuncSpawn_Player)(pthis);
#else
	reinterpret_cast<int(__fastcall *)(void *, int)>(pOrigFuncSpawn_Player)(pthis, 0);
#endif

	//Post Hook
	OnSpawn(iEnt);
}

void MakeHookSpawn_Player()
{
	edict_t *pEdict = CREATE_ENTITY();

	CALL_GAME_ENTITY(PLID, "player", &pEdict->v);

	if (pEdict->pvPrivateData == NULL)
	{
		REMOVE_ENTITY(pEdict);
		return;
	}

#if defined(__linux__) || defined (__APPLE__)
	void **vtable = *((void***)(((char*)pEdict->pvPrivateData) + 0x94));
#else
	void **vtable = *((void***)((char*)pEdict->pvPrivateData));
#endif

	REMOVE_ENTITY(pEdict);

	if (vtable == NULL)
		return;

	int **ivtable = (int **)vtable;

	pOrigFuncSpawn_Player = (void *)ivtable[VirtFuncSpawn];

#if defined(__linux__) || defined (__APPLE__)
	mprotect(&ivtable[VirtFuncSpawn], sizeof(int*), PROT_READ | PROT_WRITE);
#else
	DWORD OldFlags;
	VirtualProtect(&ivtable[VirtFuncSpawn], sizeof(int *), PAGE_READWRITE, &OldFlags);
#endif

	ivtable[VirtFuncSpawn] = (int *)HookSpawn_Player;
}

// CSBot Spawn Hook
#if defined(__linux__) || defined (__APPLE__)
void HookSpawn_CSBot(void *pthis)
#else
void __fastcall HookSpawn_CSBot(void *pthis)
#endif
{
	int iEnt = ENTINDEX(PrivateToEdict(pthis));

	//Pre Hook

#if defined(__linux__) || defined (__APPLE__)
	reinterpret_cast<int(*)(void *)>(pOrigFuncSpawn_CSBot)(pthis);
#else
	reinterpret_cast<int(__fastcall *)(void *, int)>(pOrigFuncSpawn_CSBot)(pthis, 0);
#endif

	//Post Hook
	OnSpawn(iEnt);
}

void MakeHookSpawn_CSBot(edict_t *pEdict)
{
#if defined(__linux__) || defined (__APPLE__)
	void **vtable = *((void***)(((char*)pEdict->pvPrivateData) + 0x94));
#else
	void **vtable = *((void***)((char*)pEdict->pvPrivateData));
#endif

	if (vtable == NULL)
		return;

	int **ivtable = (int **)vtable;

	pOrigFuncSpawn_CSBot = (void *)ivtable[VirtFuncSpawn];

#if defined(__linux__) || defined (__APPLE__)
	mprotect(&ivtable[VirtFuncSpawn], sizeof(int*), PROT_READ | PROT_WRITE);
#else
	DWORD OldFlags;
	VirtualProtect(&ivtable[VirtFuncSpawn], sizeof(int *), PAGE_READWRITE, &OldFlags);
#endif

	ivtable[VirtFuncSpawn] = (int *)HookSpawn_CSBot;
}
