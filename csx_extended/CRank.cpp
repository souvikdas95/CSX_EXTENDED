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

// *****************************************************
// structure Stats
// *****************************************************

Stats::Stats()
{
	hits = shots = damage = hs = tks = kills = deaths = bDefusions = bDefused = bPlants = bExplosions = 0;
	memset(bodyHits, 0, sizeof(bodyHits));
}

void Stats::commit(Stats* a)
{
	hits += a->hits;
	shots += a->shots;
	damage += a->damage;
	hs += a->hs;
	tks += a->tks;
	kills += a->kills;
	deaths += a->deaths;

	bDefusions += a->bDefusions;
	bDefused += a->bDefused;
	bPlants += a->bPlants;
	bExplosions += a->bExplosions;

	for (uint8_t i = 1; i < 8; ++i)
		bodyHits[i] += a->bodyHits[i];
}


// *****************************************************
// class RankSystem
// *****************************************************

// Constructor/Destructor Routines
RankSystem::RankStats::RankStats(const char* uu, const char* nn, RankSystem* pp)
{
	score = 0;
	namelen = uniquelen = 0;
	next = prev = NULL;
	unique = name = NULL;

	parent = pp;
	id = ++parent->rankNum;
	setUnique(uu);
	setName(nn);
}

RankSystem::RankStats::~RankStats()
{
	delete[] name;
	delete[] unique;
	--parent->rankNum;
}

RankSystem::RankSystem()
{
	rankNum = 0;
	head = NULL;
	tail = NULL;
	calc.code = NULL;
	if (HANDLE_MUTEX == FALSE)
	{
		delete MUTEX;
		MUTEX = new CRITICAL_SECTION;
		HANDLE_MUTEX = InitializeCriticalSectionAndSpinCount(MUTEX, 0x00000400);
		if (HANDLE_MUTEX == FALSE)
		{
			MF_Log("RankSystem: Couldn't create critical section");
			print_srvconsole("[%s] RankSystem: Couldn't create critical section\n", MODULE_LOGTAG);
		}
	}
}

// Identity Initialization Routines
void RankSystem::RankStats::setName(const char* nn)
{
	if (nn == NULL)
		return;
	delete[] name;
	namelen = (int16_t)(strlen(nn) + 1);
	name = new char[namelen];
	if (name != NULL)
		strncpy(name, nn, namelen);
	else
		namelen = 0;
}

void RankSystem::RankStats::setUnique(const char* uu)
{
	if (uu == NULL)
		return;
	delete[] unique;
	uniquelen = (int16_t)(strlen(uu) + 1);
	unique = new char[uniquelen];
	if (unique != NULL)
		strncpy(unique, uu, uniquelen);
	else
		uniquelen = 0;
}

// RankStats Traversal Utility Routines
void RankSystem::put_before(RankStats* a, RankStats* ptr)	// ptr is fixed
{
	if (a == NULL || ptr == NULL)	// Handle Invalid RankStats
		return;

	a->prev = ptr->prev;
	a->next = ptr;
	ptr->prev = a;

	if (a->prev == NULL)
		head = a;
	else
		a->prev->next = a;
}

void  RankSystem::put_after(RankStats* a, RankStats* ptr)	// ptr is fixed
{
	if (a == NULL || ptr == NULL)	// Handle Invalid RankStats
		return;

	a->next = ptr->next;
	a->prev = ptr;
	ptr->next = a;

	if (a->next == NULL)
		tail = a;
	else
		a->next->prev = a;
}

void RankSystem::unlink_ptr(RankStats* ptr)
{
	if (ptr == NULL)	// Handle Invalid RankStats
		return;

	if (ptr->prev != NULL)
		ptr->prev->next = ptr->next;
	else
		head = ptr->next;

	if (ptr->next != NULL)
		ptr->next->prev = ptr->prev;
	else
		tail = ptr->prev;

	ptr->prev = ptr->next = NULL;	// safely unlinked?
}

// *************************************************
// *** THREAD SAFE RankStats Management Routines ***
// *************************************************
BOOL RankSystem::HANDLE_MUTEX = FALSE;
CRITICAL_SECTION* RankSystem::MUTEX = NULL;

void RankSystem::clear()
{
	// Enter Critical Section
	EnterCriticalSection(MUTEX);
	
	while (head != NULL)
	{
		tail = head->next;
		delete head;
		head = tail;
	}

	// Leave Critical Section
	LeaveCriticalSection(MUTEX);
}

// Update Position Routines
DWORD RankSystem::updatePos_thread(LPVOID lpParam)
{
	RankStats *arg = ((RankStats*)lpParam);
	if (arg == NULL || arg->parent == NULL) // Verify Pointer to RankStats
	{
		MF_Log("updatePos_thread: Invalid pointer parameter to RankStats");
		print_srvconsole("[%s] updatePos_thread: Invalid pointer parameter to RankStats\n", MODULE_LOGTAG);
		return 0;
	}
	arg->parent->updatePos_exec(arg);
	return 0;
}

void RankSystem::updatePos_init(RankStats* rr, Stats* s, bool sync)
{
	if (rr == NULL)	// Verify Pointer to RankStats
		return;

	if (s != NULL)	// NULL Stats Update (Only for Synchronization)
		rr->addStats(s);

	if (sync == true)
		updatePos_exec(rr);
	else
	{
		// Prevent Waiting on MAIN Thread
		HANDLE h_temp = CreateThread(NULL, 0, updatePos_thread, rr, CREATE_SUSPENDED, NULL);
		if (h_temp == NULL)
		{
			MF_Log("updatePos_init: Couldn't create thread for updating Ranks");
			print_srvconsole("[%s] updatePos_init: Couldn't create thread for updating Ranks\n", MODULE_LOGTAG);
		}
		else
		{
			SetThreadPriority(h_temp, THREAD_PRIORITY_BELOW_NORMAL);
			ResumeThread(h_temp);
			CloseHandle(h_temp);
		}
	}
}

void RankSystem::updatePos_exec(RankStats* rr)
{
	// Enter Critical Section
	EnterCriticalSection(MUTEX);

	// Verify Pointer to RankStats
	if(rr == NULL)
	{
		MF_Log("updatePos_exec: Pointer to RankStats has Expired!");
		print_srvconsole("[%s] updatePos_exec: Pointer to RankStats has Expired!\n", MODULE_LOGTAG);

		// Leave Critical Section
		LeaveCriticalSection(MUTEX);
		return;
	}

	// Update Calc Buffer
	uint8_t i;
	calc.physAddr1[0] = rr->kills;
	calc.physAddr1[1] = rr->deaths;
	calc.physAddr1[2] = rr->hs;
	calc.physAddr1[3] = rr->tks;
	calc.physAddr1[4] = rr->shots;
	calc.physAddr1[5] = rr->hits;
	calc.physAddr1[6] = rr->damage;
	calc.physAddr1[7] = rr->bDefusions;
	calc.physAddr1[8] = rr->bDefused;
	calc.physAddr1[9] = rr->bPlants;
	calc.physAddr1[10] = rr->bExplosions;
	for (i = 1; i < 8; ++i)
		calc.physAddr2[i] = rr->bodyHits[i];

	// Result Calculation using Calc Buffer and Calc Function "get_score"
	cell result = 0;
	MF_AmxPush(&calc.amx, calc.amxAddr2);
	MF_AmxPush(&calc.amx, calc.amxAddr1);
	if ((i = (uint8_t)MF_AmxExec(&calc.amx, &result, calc.func)) != AMX_ERR_NONE)
	{
		MF_LogError(&calc.amx, AMX_ERR_NATIVE, "findEntryInRank: Error in function \"get_score\"");
		print_srvconsole("[%s] findEntryInRank: Error in function \"get_score\"\n", MODULE_LOGTAG);

		// Leave Critical Section
		LeaveCriticalSection(MUTEX);
		return;
	}

	rr->score = result;
	RankStats* itr = rr->prev;
	while (itr != NULL && rr->score >= itr->score)
	{
		rr->id--;
		itr->id++;
		itr = itr->prev;
	}
	if (itr != rr->prev)
	{
		unlink_ptr(rr);
		if (itr == NULL)
			put_before(rr, head);
		else
			put_after(rr, itr);	// since current "itr" has greater score
	}
	else
	{
		itr = rr->next;
		while (itr != NULL && rr->score < itr->score)
		{
			rr->id++;
			itr->id--;
			itr = itr->next;
		}
		if (itr != rr->next)
		{
			unlink_ptr(rr);
			if (itr == NULL)
				put_after(rr, tail);
			else
				put_before(rr, itr);	// since current "itr" has lower (or equal) score
		}
	}
	
	// Leave Critical Section
	LeaveCriticalSection(MUTEX);
}

// RANK LOAD / SAVE
// Note: Char Arrays / Strings stored in csstats file contain an extra character containing '\0'

// Save Rank Routines
HANDLE RankSystem::h_saveRank = NULL;

DWORD RankSystem::saveRank_thread(LPVOID lpParam)
{
	RankSystem *arg = ((RankSystem*)lpParam);
	if(arg == NULL)
	{
		MF_Log("saveRank_thread: Invalid pointer parameter to RankSystem");
		print_srvconsole("[%s] saveRank_thread: Invalid pointer parameter to RankSystem\n", MODULE_LOGTAG);
		return 0;
	}
	arg->saveRank_exec();
	return 0;
}

void RankSystem::saveRank_init()
{
	if (h_saveRank != NULL)
	{
		WaitForSingleObject(h_saveRank, INFINITE);
		CloseHandle(h_saveRank);
	}
	h_saveRank = CreateThread(NULL, 0, saveRank_thread, this, CREATE_SUSPENDED, NULL);	// Prevent Waiting on MAIN Thread
	if (h_saveRank == NULL)
	{
		MF_Log("saveRank_init: Couldn't create thread for saving Ranks");
		print_srvconsole("[%s] saveRank_init: Couldn't create thread for saving Ranks\n", MODULE_LOGTAG);
	}
	else
	{
		SetThreadPriority(h_saveRank, THREAD_PRIORITY_BELOW_NORMAL);
		ResumeThread(h_saveRank);
	}
}

void RankSystem::saveRank_exec()
{
	// Enter Critical Section
	EnterCriticalSection(MUTEX);
	
	// Open Stats file for Writing
	const char* filename = MF_BuildPathname("%s", get_localinfo("csstats"));	// Path to "csstats.dat"
	FILE* bfp = fopen(filename, "wb");
	if (bfp == NULL)
	{
		MF_Log("saveRank: Could not load csstats file: \"%s\"", filename);
		print_srvconsole("[%s] saveRank: Could not load csstats file: \"%s\"\n", MODULE_LOGTAG, filename);

		// Leave Critical Section
		LeaveCriticalSection(MUTEX);
		return;
	}

	// Write First Byte (RANK_VERSION)
	int16_t* buffer = new int16_t;
	*buffer = RANK_VERSION;
	fwrite(buffer, sizeof(int16_t), 1, bfp);

	// Save
	RankStats *itr = head;
	while (itr != NULL)
	{
		fwrite(&itr->namelen, sizeof(int16_t), 1, bfp);
		fwrite(itr->name, sizeof(char), itr->namelen, bfp);
		fwrite(&itr->uniquelen, sizeof(int16_t), 1, bfp);
		fwrite(itr->unique, sizeof(char), itr->uniquelen, bfp);
		fwrite(&itr->tks, sizeof(int32_t), 1, bfp);
		fwrite(&itr->damage, sizeof(int32_t), 1, bfp);
		fwrite(&itr->deaths, sizeof(int32_t), 1, bfp);
		fwrite(&itr->kills, sizeof(int32_t), 1, bfp);
		fwrite(&itr->shots, sizeof(int32_t), 1, bfp);
		fwrite(&itr->hits, sizeof(int32_t), 1, bfp);
		fwrite(&itr->hs, sizeof(int32_t), 1, bfp);
		fwrite(&itr->bDefusions, sizeof(int32_t), 1, bfp);
		fwrite(&itr->bDefused, sizeof(int32_t), 1, bfp);
		fwrite(&itr->bPlants, sizeof(int32_t), 1, bfp);
		fwrite(&itr->bExplosions, sizeof(int32_t), 1, bfp);
		fwrite(itr->bodyHits, sizeof(itr->bodyHits), 1, bfp);
		itr = itr->next;
	}
	*buffer = 0;
	fwrite(buffer, sizeof(int16_t), 1, bfp);	// NULL Terminator
	fclose(bfp);

	// Leave Critical Section
	LeaveCriticalSection(MUTEX);
}

// Load Rank Routine
void RankSystem::loadRank()
{
	// Wait for saveRank THREAD to Finish (Important)
	if (h_saveRank != NULL)
		WaitForSingleObject(h_saveRank, INFINITE);

	// Open Stats file for Reading
	const char* filename = MF_BuildPathname("%s", get_localinfo("csstats"));	// Path to csstats file
	FILE* bfp = fopen(filename, "rb");
	if (bfp == NULL)
	{
		MF_Log("loadRank: Could not load csstats file: \"%s\"", filename);
		print_srvconsole("[%s] loadRank: Could not load csstats file: \"%s\"\n", MODULE_LOGTAG, filename);
		return;
	}

	// Get RANK_VERSION (1 Byte)
	int16_t* buffer = new int16_t;
	if (fread(buffer, sizeof(int16_t), 1, bfp) != 1 || *buffer != RANK_VERSION)
	{
		MF_Log("loadRank: Invalid RANK_VERSION Found!");
		print_srvconsole("[%s] loadRank: Invalid RANK_VERSION Found!\n", MODULE_LOGTAG);
		fclose(bfp);
		return;
	}

	// Init
	Stats d;
	char *unique = NULL, *name = NULL;

	// Clear Memory before Load
	g_rank.clear();

	// Load
	if (fread(buffer, sizeof(int16_t), 1, bfp) != 1)	// Pre Check
	{
		fclose(bfp);
		return;
	}
	while (buffer != NULL && *buffer && !feof(bfp))
	{
		name = new char[*buffer];
		if (fread(name, sizeof(char), *buffer, bfp) != *buffer)	break;
		if (fread(buffer, sizeof(int16_t), 1, bfp) != 1)	break;	// unique length
		unique = new char[*buffer];
		if (fread(unique, sizeof(char), *buffer, bfp) != *buffer)	break;
		if (fread(&d.tks, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.damage, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.deaths, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.kills, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.shots, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.hits, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.hs, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.bDefusions, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.bDefused, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.bPlants, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(&d.bExplosions, sizeof(int32_t), 1, bfp) != 1)	break;
		if (fread(d.bodyHits, sizeof(d.bodyHits), 1, bfp) != 1)	break;
		if (fread(buffer, sizeof(int16_t), 1, bfp) != 1)	break;	// name length or NULL Terminator

		// Precise Check: Duplicate Entries ( Defalult: Discard Lower Rank Entries )
		RankSystem::RankStats* temp = findEntryInRank(unique);
		if (temp == NULL)
		{
			temp = newEntryInRank(unique, name);
			if (temp != NULL)
				temp->updatePosition(&d, true);
			else
			{
				MF_Log("loadRank: Unable to load any more Stats on Server");
				print_srvconsole("[%s] loadRank: Unable to load any more Stats on Server\n", MODULE_LOGTAG);
				fclose(bfp);
				return;
			}
		}

		delete[] name;
		delete[] unique;
	}
	fclose(bfp);
}

// Delete Position Routine
void RankSystem::deletePos(RankStats* rr)
{
	// Enter Critical Section
	EnterCriticalSection(MUTEX);
	
	RankStats* aa = rr->next;
	while (aa)	// Shift all ranks worse than "rr" 1 step for good
	{
		aa->id--;
		aa = aa->next;
	}

	unlink_ptr(rr);
	delete rr;
	rr = NULL;

	// Leave Critical Section
	LeaveCriticalSection(MUTEX);
}

// Add New RankStats Entry Routine
RankSystem::RankStats* RankSystem::newEntryInRank(const char* unique, const char* name)
{
	// Enter Critical Section
	EnterCriticalSection(MUTEX);
	
	// Create New RankStats Entry
	RankStats* a = new RankStats(unique, name, this);
	if (a == NULL)
	{
		MF_Log("newEntryInRank: Could not allocate new \"RankStats\" entry");
		print_srvconsole("[%s] newEntryInRank: Could not allocate new \"RankStats\" entry\n", MODULE_LOGTAG);

		// Leave Critical Section
		LeaveCriticalSection(MUTEX);

		return NULL;
	}

	if (tail != NULL)
		put_after(a, tail);
	else
		head = tail = a;	// First Entry

	// Leave Critical Section
	LeaveCriticalSection(MUTEX);

	return a;
}

RankSystem::RankStats* RankSystem::findEntryInRank(const char* unique)
{
	// Enter Critical Section
	EnterCriticalSection(MUTEX);
	
	RankStats* itr = head;
	while (itr != NULL)
	{
		if (!strcmp(itr->getUnique(), unique))
			break;
		itr = itr->next;
	}

	// Leave Critical Section
	LeaveCriticalSection(MUTEX);

	return itr;
}

void RankSystem::iterator::getEntryByRank(uint16_t rank)	// Only for Iteration
{
	// Enter Critical Section
	EnterCriticalSection(MUTEX);

	RankSystem::iterator a = g_rank.itr_head();
	while (--rank && &(*a) != NULL)
		++a;
	ptr = a.ptr;

	// Leave Critical Section
	LeaveCriticalSection(MUTEX);
}

// Import Score Calculator Routine
bool RankSystem::loadCalc(const char* filename)
{
	static char error[128];
	if ((MF_LoadAmxScript(&calc.amx, &calc.code, filename, error, 0) != AMX_ERR_NONE)
		|| (MF_AmxAllot(&calc.amx, 11, &calc.amxAddr1, &calc.physAddr1) != AMX_ERR_NONE)
		|| (MF_AmxAllot(&calc.amx, 8, &calc.amxAddr2, &calc.physAddr2) != AMX_ERR_NONE)
		|| (MF_AmxFindPublic(&calc.amx, "get_score", &calc.func) != AMX_ERR_NONE))
	{
		MF_LogError(&calc.amx, AMX_ERR_NATIVE, "loadCalc: Couldn't load function \"get_score\" from \"%s\"", filename);
		print_srvconsole("[%s] loadCalc: Couldn't load function \"get_score\" from \"%s\"\n", MODULE_LOGTAG, filename);
		MF_UnloadAmxScript(&calc.amx, &calc.code);
		return false;
	}
	return true;
}