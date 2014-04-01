======================
CSX_AMXX_UPDATING
======================

Update for AmxModX CSX Module for Counter-Strike 1.6 and Condition Zero

-----------------------------
Modifications:
----------------------------- 

1. Added: following Natives:

get_user_rank
get_user_score
set_stats
set_stats2
set_user_stats
set_user_stats2
set_user_score
reset_user_stats
reset_stats
reset_stats
push_stats
remove_stats
force_load_stats
force_save_stats

2. Changes in cVar "csstats_maxsize"

Changed: default value changed from 3500 to 9000
Removed: Maximum Value Cap ( clamp removed with 0 min and 3500 max )
Added: support for no maxsize ( value = -1 )

3. Make cVar "csstats_rank" and "csstats_rankbots" constant after ServerActivate (Map Change) / Init

Any change to the value of "csstats_rank" should be accounted for only
at ServerActivate/Init. This will ensure that the field "unique" of players
which defines a player's identity in the module, remain constant before next
Call of ServerActivate\Init.

Any change to the value of "csstats_rankbots" should be accounter for only
at ServerActivate/Init. It's a critical requirement as the identity of the 
already connected Bots, would not exist in the module. In that case, the 
module would malfunction. To further prohit a possible crash, also replaced
the check for "CPlayer::ignoreBots" during rank update ( ResetHUD ), with "CPlayer::rank"
which will check for existance of identity.

4. Made Saving Rank more Precise

Added: Check for mannual exploit of ResetHUD ( Check for "fullupdate" command ). This will also ensure Player Round Stats don't get cleared away before Next Respawn
Added: Delay for per player Rank Update

5. Fixed: get_stats() and get_stats2() did not recognise the position of the last rank.

6. Fixed: new players were assigned last rank by default regardless of death toll of other players.

7. Fixed: Last Death declaring Round_End logEvent wasn't counted.

8. Replaced: Sequential Search with Binary Search in some of the Natives for faster Processing ( by Shooting King ). Affected Natives:

get_stats
get_stats2
set_stats
set_stats2
reset_stats
remove_stats



-----------------------------
Natives Details:
-----------------------------


/*  
*  Returns the Rank of Player. --> index = id.  
*  Returns 0 if no Rank exists. 
*/ 
native get_user_rank(index); 

/* 
*  Gets current session score of player --> index = id 
*  Returns 0 if player is not valid/out of range 
*  Returns 1 
*/ 
native get_user_score(index,&frags,&deaths); 

/* 
*  Sets overall stats of Player with given parameters. --> index = id.  
*  Returns 0 if player is not valid/out of range 
*  Returns new Rank 
* 
*  Note - If you don't want to edit/set a stats/bodyhits field, 
*  you can set its value as -1 
*/ 
native set_user_stats(index,stats[8],bodyhits[8]); 
native set_user_stats2(index,stats2[4]); 

/* 
*  Sets overall stats with given parameters. --> index = position.  
*  Returns new Rank 
* 
*  Note - If you don't want to edit/set a stats/bodyhits field, 
*  you can set its value as -1 
*/ 
native set_stats(index,stats[8],bodyhits[8]); 
native set_stats2(index,stats2[4]); 

/* 
*  Sets current session score of player --> index = id 
*  Returns 0 if player is not valid/out of range 
*  Returns 1 
* 
*  Note - If you don't want to edit/set a stats/bodyhits field, 
*  you can set its value as -1 
*/ 
native set_user_score(index,frags,deaths); 

/* 
*  Resets overall stats to null --> index = id. 
*  Returns 0 if player is not valid/out of range 
*  Returns new Rank 
* 
*  Note - This doesn't differentiate stats or stats2. 
*  It simply nulls the entire Stats structure of index. 
*/ 
native reset_user_stats(index); 

/* 
*  Resets overall stats to null --> index = position. 
*  Returns new Rank 
* 
*  Note - This doesn't differentiate stats or stats2. 
*  It simply nulls the entire Stats structure of index. 
*/ 
native reset_stats(index); 

/* 
*  Adds/Pushes overall stats with given parameters. 
*  Returns 0 if Stats Entry already exists or can't be created 
*  Returns new Rank 
* 
*  Note - If you don't want to set a stats/bodyhits field, 
*  you can set its value as -1 
* 
* IMPORTANT : "unique" is based on the value of "csstats_rank" cvar  
* which sets the basis for uniquely defining a rankstats entry: 
* 0 - name[] 
* 1 - authid[] 
* 2 - ip[] ( make sure to set parameter isip = 1 ) 
* 
* WARNING : Use this function with Caution. 
*/ 
native push_stats(const unique[],const name[],stats[8],bodyhits[8],isip=0); 

/* 
* Removes and Deletes the Stats Entry. --> index = position.  
*  Returns -1 if position is not found. 
*  Returns 0 if player with the position is connected to server. 
*  Returns 1 if successfully removed. 
*/ 
native remove_stats(index); 

/*  
*  Reloads all the stats from the file and  
*  refresh rankstats of all the connected  
*  players and also reset current score of player 
*  Returns 1 
* 
*  Note - If the stats are not externally edited 
*  then this will restore the stats from previous 
*  map change or from start of HLDS as by default, 
*  stats file is saved only at these moments. 
* 
*  If the stats file doesn't exist, the function 
*  will not load stats. The stats in-game will remain 
*  the same as before. 
* 
*  default stats file path : amxmodx/data/csstats.dat 
*/ 
native force_load_stats(); 

/* 
*  Manually saves all the stats till previous round 
*  before Restart of Map/Server. 
*  Returns 1 
* 
*  default stats file path : amxmodx/data/csstats.dat 
*/ 
native force_save_stats(); 
