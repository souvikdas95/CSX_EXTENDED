

#include "amxxmodule.h"
#include "rank.h"


static cell AMX_NATIVE_CALL get_user_astats(AMX *amx, cell *params) /* 6 param */
{
	int index = params[1];
	CHECK_PLAYERRANGE(index);
	int attacker = params[2];
	CHECK_PLAYERRANGE(attacker);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if (pPlayer->attackers[attacker].hits){
		cell *cpStats = MF_GetAmxAddr(amx,params[3]);
		cell *cpBodyHits = MF_GetAmxAddr(amx,params[4]);
		CPlayer::PlayerWeapon* stats = &pPlayer->attackers[attacker];
		cpStats[0] = stats->kills;
		cpStats[1] = stats->deaths;
		cpStats[2] = stats->hs;
		cpStats[3] = stats->tks;
		cpStats[4] = stats->shots;
		cpStats[5] = stats->hits;
		cpStats[6] = stats->damage;
		for(int i = 1; i < 8; ++i)
			cpBodyHits[i] = stats->bodyHits[i];
		if (params[6] && attacker && stats->name )
			MF_SetAmxString(amx,params[5],stats->name,params[6]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL get_user_vstats(AMX *amx, cell *params) /* 6 param */
{
	int index = params[1];
	CHECK_PLAYERRANGE(index);
	int victim = params[2];
	CHECK_PLAYERRANGE(victim);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if (pPlayer->victims[victim].hits){
		cell *cpStats = MF_GetAmxAddr(amx,params[3]);
		cell *cpBodyHits = MF_GetAmxAddr(amx,params[4]);
		CPlayer::PlayerWeapon* stats = &pPlayer->victims[victim];
		cpStats[0] = stats->kills;
		cpStats[1] = stats->deaths;
		cpStats[2] = stats->hs;
		cpStats[3] = stats->tks;
		cpStats[4] = stats->shots;
		cpStats[5] = stats->hits;
		cpStats[6] = stats->damage;
		for(int i = 1; i < 8; ++i)
			cpBodyHits[i] = stats->bodyHits[i];
		if (params[6] && victim && stats->name)
			MF_SetAmxString(amx,params[5],stats->name,params[6]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL get_user_wrstats(AMX *amx, cell *params) /* 4 param */ // DEC-Weapon (round) stats (end)
{
	int index = params[1];
	CHECK_PLAYERRANGE(index);
	int weapon = params[2];
	if (weapon<0||weapon>=MAX_WEAPONS+MAX_CWEAPONS){
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id %d", weapon);
		return 0;
	}
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if (pPlayer->weaponsRnd[weapon].shots){
		cell *cpStats = MF_GetAmxAddr(amx,params[3]);
		cell *cpBodyHits = MF_GetAmxAddr(amx,params[4]);
		Stats* stats = &pPlayer->weaponsRnd[weapon];
		cpStats[0] = stats->kills;
		cpStats[1] = stats->deaths;
		cpStats[2] = stats->hs;
		cpStats[3] = stats->tks;
		cpStats[4] = stats->shots;
		cpStats[5] = stats->hits;
		cpStats[6] = stats->damage;
		for(int i = 1; i < 8; ++i)
			cpBodyHits[i] = stats->bodyHits[i];
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL get_user_wstats(AMX *amx, cell *params) /* 4 param */
{
	int index = params[1];
	CHECK_PLAYERRANGE(index);
	int weapon = params[2];
	if (weapon<0||weapon>=MAX_WEAPONS+MAX_CWEAPONS){
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id %d", weapon);
		return 0;
	}
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if (pPlayer->weapons[weapon].shots){
		cell *cpStats = MF_GetAmxAddr(amx,params[3]);
		cell *cpBodyHits = MF_GetAmxAddr(amx,params[4]);
		CPlayer::PlayerWeapon* stats = &pPlayer->weapons[weapon];
		cpStats[0] = stats->kills;
		cpStats[1] = stats->deaths;
		cpStats[2] = stats->hs;
		cpStats[3] = stats->tks;
		cpStats[4] = stats->shots;
		cpStats[5] = stats->hits;
		cpStats[6] = stats->damage;
		for(int i = 1; i < 8; ++i)
			cpBodyHits[i] = stats->bodyHits[i];
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL reset_user_wstats(AMX *amx, cell *params) /* 6 param */
{
	int index = params[1];
	CHECK_PLAYER(index);
	GET_PLAYER_POINTER_I(index)->restartStats();
	return 1;
}

static cell AMX_NATIVE_CALL get_user_rstats(AMX *amx, cell *params) /* 3 param */
{
	int index = params[1];
	CHECK_PLAYERRANGE(index);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if (pPlayer->rank){
		cell *cpStats = MF_GetAmxAddr(amx,params[2]);
		cell *cpBodyHits = MF_GetAmxAddr(amx,params[3]);
		cpStats[0] = pPlayer->life.kills;
		cpStats[1] = pPlayer->life.deaths;
		cpStats[2] = pPlayer->life.hs;
		cpStats[3] = pPlayer->life.tks;
		cpStats[4] = pPlayer->life.shots;
		cpStats[5] = pPlayer->life.hits;
		cpStats[6] = pPlayer->life.damage;
		for(int i = 1; i < 8; ++i)
			cpBodyHits[i] = pPlayer->life.bodyHits[i];
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL get_user_stats(AMX *amx, cell *params) /* 3 param */
{
	int index = params[1];
	CHECK_PLAYERRANGE(index);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if ( pPlayer->rank ){
		cell *cpStats = MF_GetAmxAddr(amx,params[2]);
		cell *cpBodyHits = MF_GetAmxAddr(amx,params[3]);
		cpStats[0] = pPlayer->rank->kills;
		cpStats[1] = pPlayer->rank->deaths;
		cpStats[2] = pPlayer->rank->hs;
		cpStats[3] = pPlayer->rank->tks;
		cpStats[4] = pPlayer->rank->shots;
		cpStats[5] = pPlayer->rank->hits;
		cpStats[6] = pPlayer->rank->damage;

		cpStats[7] = pPlayer->rank->getPosition();

		for(int i = 1; i < 8; ++i)
			cpBodyHits[i] = pPlayer->rank->bodyHits[i];
		return pPlayer->rank->getPosition();
	}
	return 0;

}

static cell AMX_NATIVE_CALL get_user_stats2(AMX *amx, cell *params) /* 3 param */
{
	int index = params[1];
	CHECK_PLAYERRANGE(index);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if ( pPlayer->rank ){
		cell *cpStats = MF_GetAmxAddr(amx,params[2]);

		cpStats[0] = pPlayer->rank->bDefusions;
		cpStats[1] = pPlayer->rank->bDefused;
		cpStats[2] = pPlayer->rank->bPlants;
		cpStats[3] = pPlayer->rank->bExplosions;

		return pPlayer->rank->getPosition();
	}
	return 0;
}

static cell AMX_NATIVE_CALL get_stats(AMX *amx, cell *params) /* 7 param */
{

	int index = params[1] + 1;

	//for(RankSystem::iterator a = g_rank.front(); a ;--a){
	//	if ((*a).getPosition() == index)  {

	RankSystem::iterator a;
	a.getEntryByRank( index );
	if (a) {
			cell *cpStats = MF_GetAmxAddr(amx,params[2]);
			cell *cpBodyHits = MF_GetAmxAddr(amx,params[3]);
			cpStats[0] = (*a).kills;
			cpStats[1] = (*a).deaths;
			cpStats[2] = (*a).hs;
			cpStats[3] = (*a).tks;
			cpStats[4] = (*a).shots;
			cpStats[5] = (*a).hits;
			cpStats[6] = (*a).damage;

			cpStats[7] = (*a).getPosition();

			MF_SetAmxString(amx,params[4],(*a).getName(),params[5]);
			if (params[6] > 0)
				MF_SetAmxString(amx, params[6], (*a).getUnique(), params[7]);
			for(int i = 1; i < 8; ++i)
				cpBodyHits[i] = (*a).bodyHits[i];
			//return --a ? index : 0;
			return index;
	}

	return 0;
}

static cell AMX_NATIVE_CALL get_stats2(AMX *amx, cell *params) /* 4 param */
{

	int index = params[1] + 1;

	//for(RankSystem::iterator a = g_rank.front(); a ;--a){
	//	if ((*a).getPosition() == index)  {

	RankSystem::iterator a;
	a.getEntryByRank( index );
	if (a) {
			cell *cpStats = MF_GetAmxAddr(amx,params[2]);
			if (params[4] > 0)
				MF_SetAmxString(amx, params[3], (*a).getUnique(), params[4]);

			cpStats[0] = (*a).bDefusions;
			cpStats[1] = (*a).bDefused;
			cpStats[2] = (*a).bPlants;
			cpStats[3] = (*a).bExplosions;

			//return --a ? index : 0;
			return index;
	}

	return 0;
}

static cell AMX_NATIVE_CALL get_statsnum(AMX *amx, cell *params)
{
	return g_rank.getRankNum();
}

static cell AMX_NATIVE_CALL register_cwpn(AMX *amx, cell *params){ // name,melee=0,logname
	int i,iLen;
	for ( i=MAX_WEAPONS;i<MAX_WEAPONS+MAX_CWEAPONS;i++){
		if ( !weaponData[i].used ){

			char* szName = MF_GetAmxString(amx, params[1], 0, &iLen);
			char *szLog = MF_GetAmxString(amx, params[3], 0, &iLen);

			strcpy(weaponData[i].name,szName);
			strcpy(weaponData[i].logname,szLog);

			weaponData[i].used = true;
			weaponData[i].melee = params[2] ? true:false;
			return i;
		}
	}

	MF_PrintSrvConsole("No More Custom Weapon Slots!\n");
	return 0;
}

static cell AMX_NATIVE_CALL custom_wpn_dmg(AMX *amx, cell *params){ // wid,att,vic,dmg,hp=0
	int weapon = params[1];
	if (  weapon < MAX_WEAPONS || weapon >= MAX_WEAPONS+MAX_CWEAPONS ||  !weaponData[weapon].used ){ // only for custom weapons
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id %d", weapon);
		return 0;
	}

	int att = params[2];
	CHECK_PLAYERRANGE(att);

	int vic = params[3];
	CHECK_PLAYERRANGE(vic);

	int dmg = params[4];
	if ( dmg<1 ){
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid damage %d", dmg);
		return 0;
	}

	int aim = params[5];
	if ( aim < 0 || aim > 7 ){
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid aim %d", aim);
		return 0;
	}

	CPlayer* pAtt = GET_PLAYER_POINTER_I(att);
	CPlayer* pVic = GET_PLAYER_POINTER_I(vic);

	pVic->pEdict->v.dmg_inflictor = NULL;
	pAtt->saveHit( pVic , weapon , dmg, aim );

	if ( !pAtt ) pAtt = pVic;
	int TA = 0;
	if ( (pVic->teamId == pAtt->teamId) && ( pVic != pAtt) )
		TA = 1;
	MF_ExecuteForward( iFDamage, static_cast<cell>(pAtt->index),
		static_cast<cell>(pVic->index), static_cast<cell>(dmg), static_cast<cell>(weapon),
		static_cast<cell>(aim), static_cast<cell>(TA) );

	if ( pVic->IsAlive() )
		return 1;

	pAtt->saveKill(pVic,weapon,( aim == 1 ) ? 1:0 ,TA);
	MF_ExecuteForward( iFDeath, static_cast<cell>(pAtt->index), static_cast<cell>(pVic->index),
		static_cast<cell>(weapon), static_cast<cell>(aim), static_cast<cell>(TA) );

	return 1;
}

static cell AMX_NATIVE_CALL custom_wpn_shot(AMX *amx, cell *params){ // player,wid
	int index = params[2];
	CHECK_PLAYERRANGE(index);

	int weapon = params[1];
	if (  weapon < MAX_WEAPONS  || weapon >= MAX_WEAPONS+MAX_CWEAPONS || !weaponData[weapon].used ){
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id %d", weapon);
		return 0;
	}

	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	pPlayer->saveShot(weapon);

	return 1;
}

static cell AMX_NATIVE_CALL get_wpnname(AMX *amx, cell *params){
	int id = params[1];
	if (id<1 || id>=MAX_WEAPONS+MAX_CWEAPONS ){
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id %d", id);
		return 0;
	}
	return MF_SetAmxString(amx,params[2],weaponData[id].name,params[3]);
}

static cell AMX_NATIVE_CALL get_wpnlogname(AMX *amx, cell *params){
	int id = params[1];
	if (id<1 || id>=MAX_WEAPONS+MAX_CWEAPONS ){
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id %d", id);
		return 0;
	}
	return MF_SetAmxString(amx,params[2],weaponData[id].logname,params[3]);
}

static cell AMX_NATIVE_CALL is_melee(AMX *amx, cell *params){
	int id = params[1];
	if (id<1 || id>=MAX_WEAPONS+MAX_CWEAPONS ){
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid weapon id %d", id);
		return 0;
	}
	if ( id == 29 )
		return 1;
	return weaponData[id].melee ? 1:0;
}

static cell AMX_NATIVE_CALL get_maxweapons(AMX *amx, cell *params){
	return MAX_WEAPONS+MAX_CWEAPONS;
}

// Send this Stock?
static cell AMX_NATIVE_CALL get_stats_size(AMX *amx, cell *params){
	return 8;
}

enum MapObjective
{
	MapObjective_Bomb	 = (1<<0),
	MapObjective_Hostage = (1<<1),
	MapObjective_Vip	 = (1<<2),
	MapObjective_Escape  = (1<<3),
};

static cell AMX_NATIVE_CALL get_map_objectives(AMX *amx, cell *params)
{
	int flags = 0;

	if (!FNullEnt(FIND_ENTITY_BY_STRING(NULL, "classname", "func_bomb_target")) ||
		!FNullEnt(FIND_ENTITY_BY_STRING(NULL, "classname", "info_bomb_target")))
		flags |= MapObjective_Bomb;

	if (!FNullEnt(FIND_ENTITY_BY_STRING(NULL, "classname", "func_hostage_rescue")) ||
		!FNullEnt(FIND_ENTITY_BY_STRING(NULL, "classname", "info_hostage_rescue")) ||
		!FNullEnt(FIND_ENTITY_BY_STRING(NULL, "classname", "hostage_entity"))) // there are maps with only this and using team spawn as rescue zone.
		flags |= MapObjective_Hostage;

	if (!FNullEnt(FIND_ENTITY_BY_STRING(NULL, "classname", "func_vip_safetyzone")))
		flags |= MapObjective_Vip;

	if (!FNullEnt(FIND_ENTITY_BY_STRING(NULL, "classname", "func_escapezone")))
		flags |= MapObjective_Escape;

	return flags;
}


/* *******************************************
*  ************** NEW NATIVES ****************
*  *******************************************/

static cell AMX_NATIVE_CALL get_user_rank(AMX *amx, cell *params) /* 1 param */
{
	int index = params[1];
	CHECK_PLAYER(index);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if ( pPlayer->rank )
		return pPlayer->rank->getPosition();
	return 0;
}

static cell AMX_NATIVE_CALL get_user_score(AMX *amx, cell *params)  /*3 param */
{
    int index = params[1];
    CHECK_PLAYER(index);
    CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
    cell *pFrags = MF_GetAmxAddr(amx, params[2]);
    cell *pDeads = MF_GetAmxAddr(amx, params[3]);
    *pFrags = (int)pPlayer->pEdict->v.frags;
    *pDeads = *((int *)pPlayer->pEdict->pvPrivateData + OFFSET_CSDEATHS);
    return 1;
}

static cell AMX_NATIVE_CALL set_stats(AMX *amx, cell *params) /* 3 param */
{
	int index = params[1] + 1;
	RankSystem::iterator a;
	a.getEntryByRank( index );
	if (a)
	{
		cell *cpStats = MF_GetAmxAddr(amx,params[2]);
		cell *cpBodyHits = MF_GetAmxAddr(amx,params[3]);
		if (cpStats[0]>=0)
			(*a).kills = cpStats[0];
		if (cpStats[1]>=0)
			(*a).deaths = cpStats[1];
		if (cpStats[2]>=0)
			(*a).hs = cpStats[2];
		if (cpStats[3]>=0)
			(*a).tks = cpStats[3];
		if (cpStats[4]>=0)
			(*a).shots = cpStats[4];
		if (cpStats[5]>=0)
			(*a).hits = cpStats[5];
		if (cpStats[6]>=0)
			(*a).damage = cpStats[6];
		for(int i = 1; i < 8; ++i)
			if (cpBodyHits[i]>=0)
				(*a).bodyHits[i] = cpBodyHits[i];
		(*a).updatePosition(&null);
		return (*a).getPosition();
	}
	return 0;
}

static cell AMX_NATIVE_CALL set_stats2(AMX *amx, cell *params) /* 2 param */
{
	int index = params[1] + 1;
	RankSystem::iterator a;
	a.getEntryByRank( index );
	if (a)
	{
		cell *cpStats = MF_GetAmxAddr(amx,params[2]);
		if (cpStats[0]>=0)
			(*a).bDefusions = cpStats[0];
		if (cpStats[1]>=0)
			(*a).bDefused = cpStats[1];
		if (cpStats[2]>=0)
			(*a).bPlants = cpStats[2];
		if (cpStats[3]>=0)
			(*a).bExplosions = cpStats[3];
		(*a).updatePosition(&null);
		return (*a).getPosition();
	}
	return 0;
}

static cell AMX_NATIVE_CALL set_user_stats(AMX *amx, cell *params) /* 3 param */
{
	int index = params[1];
	CHECK_PLAYER(index);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if ( pPlayer->rank )
	{
		cell *cpStats = MF_GetAmxAddr(amx,params[2]);
		cell *cpBodyHits = MF_GetAmxAddr(amx,params[3]);
		if (cpStats[0]>=0)
			pPlayer->rank->kills = cpStats[0];
		if (cpStats[1]>=0)
			pPlayer->rank->deaths = cpStats[1];
		if (cpStats[2]>=0)
			pPlayer->rank->hs = cpStats[2];
		if (cpStats[3]>=0)
			pPlayer->rank->tks = cpStats[3];
		if (cpStats[4]>=0)
			pPlayer->rank->shots = cpStats[4];
		if (cpStats[5]>=0)
			pPlayer->rank->hits = cpStats[5];
		if (cpStats[6]>=0)
			pPlayer->rank->damage = cpStats[6];
		for(int i = 1; i < 8; ++i)
			if (cpBodyHits[i]>=0)
				pPlayer->rank->bodyHits[i] = cpBodyHits[i];
		pPlayer->rank->updatePosition(&null);
		return pPlayer->rank->getPosition();
	}
	return 0;
}

static cell AMX_NATIVE_CALL set_user_stats2(AMX *amx, cell *params) /* 2 param */
{
	int index = params[1];
	CHECK_PLAYER(index);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	if ( pPlayer->rank )
	{
		cell *cpStats = MF_GetAmxAddr(amx,params[2]);
		if (cpStats[0]>=0)
			pPlayer->rank->bDefusions = cpStats[0];
		if (cpStats[1]>=0)
			pPlayer->rank->bDefused = cpStats[1];
		if (cpStats[2]>=0)
			pPlayer->rank->bPlants = cpStats[2];
		if (cpStats[3]>=0)
			pPlayer->rank->bExplosions = cpStats[3];
		pPlayer->rank->updatePosition(&null);
		return pPlayer->rank->getPosition();
	}
	return 0;
}

static cell AMX_NATIVE_CALL set_user_score(AMX *amx, cell *params)  /*3 param */
{
	int index = params[1];
	CHECK_PLAYER(index);
	CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
	pPlayer->setScore(params[2],params[3]);
	pPlayer->restartStats();
	return 1;
}

static cell AMX_NATIVE_CALL reset_user_stats(AMX *amx, cell *params)  /*1 param */
{
    int index = params[1];
    CHECK_PLAYER(index);
    CPlayer* pPlayer = GET_PLAYER_POINTER_I(index);
    if( pPlayer->rank )
	{
#if defined(_WIN32)
		pPlayer->rank->Stats::Stats();	// not supported in linux
#else
		pPlayer->rank->tks = 0;
		pPlayer->rank->damage = 0;
		pPlayer->rank->deaths = 0;
		pPlayer->rank->kills = 0;
		pPlayer->rank->shots = 0;
		pPlayer->rank->hits = 0;
		pPlayer->rank->hs = 0;
		pPlayer->rank->bDefusions = 0;
		pPlayer->rank->bDefused = 0;
		pPlayer->rank->bPlants = 0;
		pPlayer->rank->bExplosions = 0;
		memset( pPlayer->rank->bodyHits , 0 ,sizeof(pPlayer->rank->bodyHits));
#endif
        pPlayer->rank->updatePosition(&null);
        return pPlayer->rank->getPosition();
    }
    return 0;
}

static cell AMX_NATIVE_CALL reset_stats(AMX *amx, cell *params) /* 1 param */
{
	int index = params[1] + 1;
	RankSystem::iterator a;
	a.getEntryByRank( index );
	if (a)
	{
#if defined(_WIN32)
		(*a).Stats::Stats();	// not supported in linux
#else
		(*a).tks = 0;
		(*a).damage = 0;
		(*a).deaths = 0;
		(*a).kills = 0;
		(*a).shots = 0;
		(*a).hits = 0;
		(*a).hs = 0;
		(*a).bDefusions = 0;
		(*a).bDefused = 0;
		(*a).bPlants = 0;
		(*a).bExplosions = 0;
		memset( (*a).bodyHits , 0 ,sizeof((*a).bodyHits));
#endif
		(*a).updatePosition(&null);
		return (*a).getPosition();
	}
	return 0;
}

static cell AMX_NATIVE_CALL push_stats(AMX *amx, cell *params) /* 5 param */
{
	int iLen;
	char* Unique = MF_GetAmxString(amx, params[1], 0, &iLen);
	char* Name = MF_GetAmxString(amx, params[2], 0, &iLen);
	RankSystem::RankStats* temp = g_rank.findEntryInRank( Unique, Name, (params[5]>0)?true:false );
	if ( temp )
	{
		MF_LogError(amx, AMX_ERR_NATIVE, "Stats already exists");
		return 0;
	}
	temp = g_rank.newEntryInRank( Unique, Name );
	if ( !temp )
		return 0;
	cell* cpStats = MF_GetAmxAddr(amx,params[3]);
	cell* cpBodyHits = MF_GetAmxAddr(amx,params[4]);
	if (cpStats[0]>=0)
		temp->kills = cpStats[0];
	if (cpStats[1]>=0)
		temp->deaths = cpStats[1];
	if (cpStats[2]>=0)
		temp->hs = cpStats[2];
	if (cpStats[3]>=0)
		temp->tks = cpStats[3];
	if (cpStats[4]>=0)
		temp->shots = cpStats[4];
	if (cpStats[5]>=0)
		temp->hits = cpStats[5];
	if (cpStats[6]>=0)
		temp->damage = cpStats[6];
	for(int i = 1; i < 8; ++i)
		if (cpBodyHits[i]>=0)
			temp->bodyHits[i] = cpBodyHits[i];
	temp->updatePosition(&null);
	return temp->getPosition();
}

static cell AMX_NATIVE_CALL remove_stats(AMX *amx, cell *params) /* 1 param */
{
	int index = params[1] + 1;
	RankSystem::iterator a;
	a.getEntryByRank( index );
	if (a)
	{
		for (int i=1;i<gpGlobals->maxClients;i++)
			if ( MF_IsPlayerIngame(i)
			&& players[i].rank
			&& players[i].rank->getPosition() == index )
			{
				// To prevent any possible crash during rank update at Round End.
				MF_LogError(amx, AMX_ERR_NATIVE, "Can't Remove a Connected Player's Stats");
				return 0;
			}
		(*a).deletePosition();
		return 1;
	}
	return -1;
}

static cell AMX_NATIVE_CALL force_load_stats(AMX *amx, cell *params) /* no param */
{
	g_rank.loadRank( MF_BuildPathname( "%s", get_localinfo("csstats") ) );
	for (int i=1;i<gpGlobals->maxClients;i++)
		if ( MF_IsPlayerIngame(i) && !FNullEnt(MF_GetPlayerEdict(i)) )
			players[i].PutInServer();
	return 1;
}

static cell AMX_NATIVE_CALL force_save_stats(AMX *amx, cell *params) /* no param */
{
	g_rank.saveRank( MF_BuildPathname( "%s", get_localinfo("csstats") ) );
	return 1;
}

AMX_NATIVE_INFO stats_Natives[] = {
	{ "get_stats",      get_stats},
	{ "get_stats2",      get_stats2},
	{ "get_statsnum",   get_statsnum},
	{ "get_user_astats",  get_user_astats },
	{ "get_user_rstats",  get_user_rstats },
	{ "get_user_lstats",  get_user_rstats }, // for backward compatibility
	{ "get_user_stats",   get_user_stats },
	{ "get_user_stats2",   get_user_stats2 },
	{ "get_user_vstats",  get_user_vstats },
	{ "get_user_wrstats",  get_user_wrstats},             // DEC-Weapon(Round) Stats
	{ "get_user_wstats",  get_user_wstats},
	{ "reset_user_wstats",  reset_user_wstats },

	// New Natives
	{ "get_user_rank",		get_user_rank },
	{ "get_user_score",		get_user_score },
	{ "set_stats",			set_stats },
	{ "set_stats2",			set_stats2 },
	{ "set_user_stats",		set_user_stats },
	{ "set_user_stats2",	set_user_stats2 },
	{ "set_user_score",		set_user_score },
	{ "reset_user_stats",	reset_user_stats },
	{ "reset_stats",		reset_stats },
	{ "reset_stats",		reset_stats },
	{ "push_stats",			push_stats },
	{ "remove_stats",		remove_stats },
	{ "force_load_stats",	force_load_stats },
	{ "force_save_stats",	force_save_stats },

	// Custom Weapon Support
	{ "custom_weapon_add", register_cwpn },
	{ "custom_weapon_dmg", custom_wpn_dmg },
	{ "custom_weapon_shot", custom_wpn_shot },

	{ "xmod_get_wpnname", get_wpnname },
	{ "xmod_get_wpnlogname", get_wpnlogname },
	{ "xmod_is_melee_wpn", is_melee },
	{ "xmod_get_maxweapons", get_maxweapons },
	{ "xmod_get_stats_size", get_stats_size },

	{ "get_map_objectives", get_map_objectives },

	//***************************************
	//{ "get_weaponname", get_wpnname },

	///*******************
	{ NULL, NULL }
};

