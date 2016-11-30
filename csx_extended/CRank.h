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

#ifndef CRANK_H
#define CRANK_H

#define RANK_VERSION 11

#include "amxxmodule.h"
#include "CHeader.h"

typedef unsigned char      uint8_t;

// *****************************************************
// class Stats
// *****************************************************

struct Stats
{
	int32_t hits;
	int32_t shots;
	int32_t damage;
	int32_t hs;
	int32_t tks;
	int32_t kills;
	int32_t deaths;
	int32_t bodyHits[9];

	int32_t bPlants;
	int32_t bExplosions;
	int32_t bDefusions;
	int32_t bDefused;

	Stats();
	void commit(Stats* a);
};

// *****************************************************
// class RankSystem
// *****************************************************

class RankSystem
{
	public:
		// Class Declarations
		class RankStats;
		friend class RankStats;
		class iterator;

		// Class Definition: Rank Stats
		class RankStats : public Stats
		{
			private:
				friend class RankSystem;
				RankSystem*	parent;
				RankStats*	next;
				RankStats*	prev;
				char*		unique;
				int16_t		uniquelen;	// only for write cache
				char*		name;
				int16_t		namelen;	// only for write cache
				int32_t		score;
				uint16_t	id;
				RankStats(const char* uu, const char* nn, RankSystem* pp);
				~RankStats();
				inline void addStats(Stats* a) { commit(a); }
			public:
				void setUnique(const char* uu);
				void setName(const char* nn);
				inline const char* getName() const { return (name != NULL) ? name : ""; }
				inline const char* getUnique() const { return (unique != NULL) ? unique : ""; }
				inline uint16_t getPosition() const { return id; }
				inline void updatePosition(Stats* points = NULL, bool sync = false) { parent->updatePos_init(this, points, sync); }
				inline void deletePosition() { parent->deletePos(this); }
		};

	private:
		RankStats* head;
		RankStats* tail;
		uint16_t rankNum;

		struct scoreCalc
		{
			AMX amx;
			void* code;
			int func;
			cell amxAddr1;
			cell amxAddr2;
			cell* physAddr1;
			cell* physAddr2;
		} calc;

		void put_before(RankStats* a, RankStats* ptr);
		void put_after(RankStats* a, RankStats* ptr);
		void unlink_ptr(RankStats* ptr);
		void deletePos(RankStats* r);

		static CRITICAL_SECTION* MUTEX;	// For Inter-Thread Synchronization
		static BOOL HANDLE_MUTEX;

		static DWORD WINAPI updatePos_thread(LPVOID lpParam);
		void updatePos_exec(RankStats* rr);
		void updatePos_init(RankStats* r, Stats* s = NULL, bool sync = false);

		static DWORD WINAPI saveRank_thread(LPVOID lpParam);
		static HANDLE h_saveRank;	// To make sure Load Rank doesn't occur while Save Rank Thread is Active
		void saveRank_exec();

	public:
		RankSystem();
		~RankSystem() { clear(); }

		void saveRank_init();
		void loadRank();
		RankStats* findEntryInRank(const char* unique);
		RankStats* newEntryInRank(const char* unique, const char* name);
		bool loadCalc(const char* filename);
		inline uint16_t getRankNum() const { return rankNum; }
		void clear();
		inline void unloadCalc() { MF_UnloadAmxScript(&calc.amx, &calc.code); }

		// Class Definition: Iterator (Read Only Safe Pointers to RankStats)
		class iterator
		{
			private:
				RankStats* ptr;
			public:
				iterator() { ptr = NULL; }
				iterator(RankStats* a) : ptr(a) {}
				inline iterator& operator--() { ptr = ptr->prev; return *this; }
				inline iterator& operator++() { ptr = ptr->next; return *this; }
				inline RankStats& operator*() { return *ptr; }
				operator bool() { return (ptr != NULL); }
				void getEntryByRank(uint16_t rank);	// Only for Iteration
		};
		inline iterator itr_head() { return iterator(head); }
		inline iterator itr_tail() { return iterator(tail); }
};
#endif // CRANK_H
