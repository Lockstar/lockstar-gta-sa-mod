#include <a_samp>
#include <mod_sa_voice>

#define STREAMING_DISTANCE 300.00

new g_ChannelOpen[MAX_PLAYERS];
new g_MutedPlayers[MAX_PLAYERS];
new g_IsStreamedOut[MAX_PLAYERS][MAX_PLAYERS];
new g_StreamUpdateTicks[MAX_PLAYERS];

public OnPlayerOpenChannelRequest(playerid)
{
	new i;
	new buf[64];

	if(!IsPlayerConnected(playerid) || IsPlayerNPC(playerid) || g_MutedPlayers[playerid])
		return 0;

	for(i = 0; i < MAX_PLAYERS; i++)
		if(g_IsStreamedOut[playerid][i])
	    	return 0;

	if(!g_ChannelOpen[playerid])
	{
		format(buf, 64, "OnPlayerOpenChannelRequest(playerid: %d)", playerid);
		SendClientMessageToAll(-1, buf);

		g_ChannelOpen[playerid] = 1;

		return 1;
	}

	return 0;
}

public OnPlayerCloseChannel(playerid)
{
    new i;
    new buf[64];
    
	if(!IsPlayerConnected(playerid) || IsPlayerNPC(playerid) || g_MutedPlayers[playerid])
		return 0;

	for(i = 0; i < MAX_PLAYERS; i++)
		if(g_IsStreamedOut[playerid][i])
	    	return 0;

	if(g_ChannelOpen[playerid])
	{
		format(buf, 64, "OnPlayerCloseChannel(playerid: %d)", playerid);
		SendClientMessageToAll(-1, buf);

   		g_ChannelOpen[playerid] = 0;

		return 1;
	}

    return 0;
}

public OnPlayerVoice(playerid, messageNumber)
{
	new i;
	//new buf[64];
	
	if(!IsPlayerConnected(playerid) || IsPlayerNPC(playerid) || g_MutedPlayers[playerid])
		return 0;

	for(i = 0; i < MAX_PLAYERS; i++)
		if(g_IsStreamedOut[playerid][i])
	    	return 0;
	
	if(g_ChannelOpen[playerid])
	{
/*
		format(buf, 64, "OnPlayerVoice(playerid: %d, messageNumber: %d)", playerid, messageNumber);
		SendClientMessageToAll(-1, buf);
*/
		return 1;
	}

    return 0;
}

public OnPlayerUpdate(playerid)
{
    if(g_StreamUpdateTicks[playerid] < (GetTickCount() - 1000))
    {
  		new i;
		new Float:x1, Float:y1, Float:z1;
		new Float:x2, Float:y2, Float:z2;

		for(i = 0; i < MAX_PLAYERS; i++)
		{
	    	if(IsPlayerConnected(i))
	    	{
    			if(i == playerid) continue;

	        	GetPlayerPos(playerid, x1, y1, z1);
	        	GetPlayerPos(i, x2, y2, z2);

	        	if(floatround(floatsqroot(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2)))) > STREAMING_DISTANCE)
	        		g_IsStreamedOut[playerid][i] = 1;
	        	else
	            	g_IsStreamedOut[playerid][i] = 0;
	    	}
  		}

        g_StreamUpdateTicks[playerid] = GetTickCount();
    }

	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	if(g_ChannelOpen[playerid])
	{
	    BroadcastCloseChannel(playerid);
		
		g_ChannelOpen[playerid] = 0;
		g_MutedPlayers[playerid] = 0;
	}
    
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(!IsPlayerAdmin(playerid)) return 1;
	
    new cmd[256];
    new tmp[256];
    new	idx;
    
	cmd = strtok(cmdtext, idx);
	
	if(strcmp("/vmute", cmd, true) == 0)
	{
		tmp = strtok(cmdtext, idx);
		
		if(!strlen(tmp) || strlen(tmp) > 5)
		{
			SendClientMessage(playerid, 0xFFFFFFFF, "Usage: /vmute [id]");
			return 1;
		}
		
		new id = strval(tmp);
		
		if(!IsPlayerConnected(id))
		{
			SendClientMessage(playerid, 0xFFFFFFFF, "Player is not connected.");
			return 1;
		}
		
		BroadcastCloseChannel(playerid);
     	g_MutedPlayers[id] = 1;
	    
	    return 1;
	}
	
	if(strcmp("/vunmute", cmd, true) == 0)
	{
		tmp = strtok(cmdtext, idx);

		if(!strlen(tmp) || strlen(tmp) > 5)
		{
			SendClientMessage(playerid, 0xFFFFFFFF, "Usage: /vunmute [id]");
			return 1;
		}

		new id = strval(tmp);

		if(!IsPlayerConnected(id))
		{
			SendClientMessage(playerid, 0xFFFFFFFF, "Player is not connected.");
			return 1;
		}

	    g_MutedPlayers[id] = 0;
	    
	    return 1;
	}
	
	return 0;
}

stock strtok(const string[], &index)
{
	new length = strlen(string);
	while ((index < length) && (string[index] <= ' '))
	{
		index++;
	}

	new offset = index;
	new result[20];
	while ((index < length) && (string[index] > ' ') && ((index - offset) < (sizeof(result) - 1)))
	{
		result[index - offset] = string[index];
		index++;
	}
	result[index - offset] = EOS;
	return result;
}

