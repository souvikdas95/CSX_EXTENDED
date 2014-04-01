<b><h2>Modifications:</h2></b>

<ol>
<li>Added: following Natives:
<ul>
<li>get_user_rank</li>
<li>get_user_score</li>
<li>set_stats</li>
<li>set_stats2</li>
<li>set_user_stats</li>
<li>set_user_stats2</li>
<li>set_user_score</li>
<li>reset_user_stats</li>
<li>reset_stats</li>
<li>reset_stats</li>
<li>push_stats</li>
<li>remove_stats</li>
<li>force_load_stats</li>
<li>force_save_stats</li>
</ul>
</li>

<li>Changes in cVar "csstats_maxsize"
<ul>
<li>Changed: default value changed from 3500 to 9000</li>
<li>Removed: Maximum Value Cap ( clamp removed with 0 min and 3500 max )</li>
<li>Added: support for no maxsize ( value = -1 )</li>
</ul>
</li>

<li>Made cVar "csstats_rank" and "csstats_rankbots" constant after ServerActivate (Map Change) / Init
<ul>
<li>Any change to the value of "csstats_rank" should be accounted for only
at ServerActivate/Init. This will ensure that the field "unique" of players
which defines a player's identity in the module, remain constant before next
Call of ServerActivate\Init.</li>
<li>Any change to the value of "csstats_rankbots" should be accounter for only
at ServerActivate/Init. It's a critical requirement as the identity of the 
already connected Bots, would not exist in the module. In that case, the 
module would malfunction. To further prohit a possible crash, also replaced
the check for "CPlayer::ignoreBots" during rank update ( ResetHUD ), with "CPlayer::rank"
which will check for existance of identity.</li>
</ul>
</li>

<li>Made Saving Rank more Precise
<ul>
<li>Added: Check for mannual exploit of ResetHUD ( Check for "fullupdate" command ).<br>
This will also ensure Player Round Stats don't get cleared away before Next Respawn</li>
<li>Added: Delay for per player Rank Update</li>
</ul>
</li>

<li>Fixed: get_stats() and get_stats2() did not recognise the position of the last rank.</li>

<li>Fixed: new players were assigned last rank by default regardless of death toll of other players.</li>

<li>Fixed: Last Death declaring Round_End logEvent wasn't counted.</li>

<li>Replaced: Sequential Search with Binary Search in some of the Natives for faster Processing ( by Shooting King ).<br> Affected Natives:
<ul>
<li>get_stats</li>
<li>get_stats2</li>
<li>set_stats</li>
<li>set_stats2</li>
<li>reset_stats</li>
<li>remove_stats</li>
</ul>
</li>
</ol>

<b><h2>New Natives:</h2></b>

//  <br>
//  Returns the Rank of Player. --> index = id.<br>
//  Returns 0 if no Rank exists.<br> 
//  <br>
<b>native get_user_rank(index);</b><br> 

//  <br>
//  Gets current session score of player --> index = id<br> 
//  Returns 0 if player is not valid/out of range<br> 
//  Returns 1<br> 
//  <br>
<b>native get_user_score(index,&frags,&deaths);</b><br> 

//  <br>
//  Sets overall stats of Player with given parameters. --> index = id.<br>
//  Returns 0 if player is not valid/out of range<br>
//  Returns new Rank<br>
//  <br>
//  Note - If you don't want to edit/set a stats/bodyhits field,<br>
//  you can set its value as -1<br>
//  <br>
<b>native set_user_stats(index,stats[8],bodyhits[8]);</b><br> 
<b>native set_user_stats2(index,stats2[4]);</b><br>

//  <br>
//  Sets overall stats with given parameters. --> index = position. <br> 
//  Returns new Rank <br>
//  <br>
//  Note - If you don't want to edit/set a stats/bodyhits field, <br>
//  you can set its value as -1 <br>
//  <br>
<b>native set_stats(index,stats[8],bodyhits[8]);</b><br> 
<b>native set_stats2(index,stats2[4]);</b><br>

//  <br>
//  Sets current session score of player --> index = id <br>
//  Returns 0 if player is not valid/out of range <br>
//  Returns 1 <br>
//  <br>
//  Note - If you don't want to edit/set a stats/bodyhits field, <br>
//  you can set its value as -1 <br>
//  <br>
<b>native set_user_score(index,frags,deaths);</b><br>

//  <br>
//  Resets overall stats to null --> index = id. <br>
//  Returns 0 if player is not valid/out of range <br>
//  Returns new Rank <br>
//  <br>
//  Note - This doesn't differentiate stats or stats2. <br>
//  It simply nulls the entire Stats structure of index. <br>
//  <br>
<b>native reset_user_stats(index);</b><br>

//  <br>
//  Resets overall stats to null --> index = position. <br>
//  Returns new Rank <br>
//  <br>
//  Note - This doesn't differentiate stats or stats2. <br>
//  It simply nulls the entire Stats structure of index. <br>
//  <br>
<b>native reset_stats(index);</b><br>

//  <br>
//  Adds/Pushes overall stats with given parameters. <br>
//  Returns 0 if Stats Entry already exists or can't be created <br>
//  Returns new Rank <br>
//  <br>
//  Note - If you don't want to set a stats/bodyhits field, <br>
//  you can set its value as -1 <br>
//  <br>
//  IMPORTANT : "unique" is based on the value of "csstats_rank" cvar  <br>
//  which sets the basis for uniquely defining a rankstats entry: <br>
//  0 - name[] <br>
//  1 - authid[] <br>
//  2 - ip[] ( make sure to set parameter isip = 1 ) <br>
//  <br>
//  WARNING : Use this function with Caution. <br>
//  <br>
<b>native push_stats(const unique[],const name[],stats[8],bodyhits[8],isip=0);</b><br>

//  <br>
//  Removes and Deletes the Stats Entry. --> index = position.  <br>
//  Returns -1 if position is not found. <br>
//  Returns 0 if player with the position is connected to server. <br>
//  Returns 1 if successfully removed. <br>
//  <br>
<b>native remove_stats(index);</b><br>

//  <br>
//  Reloads all the stats from the file and  <br>
//  refresh rankstats of all the connected  <br>
//  players and also reset current score of player <br>
//  Returns 1 <br>
//  <br>
//  Note - If the stats are not externally edited <br>
//  then this will restore the stats from previous <br>
//  map change or from start of HLDS as by default, <br>
//  stats file is saved only at these moments. <br>
//  <br>
//  If the stats file doesn't exist, the function <br>
//  will not load stats. The stats in-game will remain <br>
//  the same as before. <br>
//  <br>
//  default stats file path : amxmodx/data/csstats.dat <br>
//  <br>
<b>native force_load_stats();</b><br>

//  <br>
//  Manually saves all the stats till previous round <br>
//  before Restart of Map/Server. <br>
//  Returns 1 <br>
//  <br>
//  default stats file path : amxmodx/data/csstats.dat<br> 
//  <br>
<b>native force_save_stats();</b><br>
