#include "amxxmodule.h"
#include "CHeader.h"

void *pOrigFuncSpawn = NULL;

void HookSpawn_Post(int iEnt)
{
	CPlayer *pPlayer = GET_PLAYER_POINTER_I(iEnt);

	if (!isModuleActive()	\
		|| pPlayer == NULL	\
		|| pPlayer->rank == NULL)
		return;
	pPlayer->rank->updatePosition(&pPlayer->life);
	pPlayer->restartStats(false);
}

#if defined(__linux__) || defined (__APPLE__)
void HookSpawn(void *pthis)
#else
void __fastcall HookSpawn(void *pthis)
#endif
{
	int iEnt = ENTINDEX(PrivateToEdict(pthis));

	//Pre Hook

#if defined(__linux__) || defined (__APPLE__)
	reinterpret_cast<int(*)(void *)>(pOrigFuncSpawn)(pthis);
#else
	reinterpret_cast<int(__fastcall *)(void *, int)>(pOrigFuncSpawn)(pthis, 0);
#endif

	//Post Hook
	HookSpawn_Post(iEnt);
}

void MakeHookSpawn()
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

	pOrigFuncSpawn = (void *)ivtable[VirtFuncSpawn];

#if defined(__linux__) || defined (__APPLE__)
	mprotect(&ivtable[VirtFuncSpawn], sizeof(int*), PROT_READ | PROT_WRITE);
#else
	DWORD OldFlags;
	VirtualProtect(&ivtable[VirtFuncSpawn], sizeof(int *), PAGE_READWRITE, &OldFlags);
#endif

	ivtable[VirtFuncSpawn] = (int *)HookSpawn;
}
