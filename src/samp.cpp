/*

	PROJECT:		m0d_s0beit_sa
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:		Copyright 2007, 2008, 2009, 2010 we_sux

	m0d_s0beit_sa is available from http://code.google.com/p/m0d-s0beit-sa/

	m0d_s0beit_sa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	m0d_s0beit_sa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with m0d_s0beit_sa.  If not, see <http://www.gnu.org/licenses/>.

	$LastChangedDate: 2010-01-05 01:40:59 -0600 (Tue, 05 Jan 2010) $
	$LastChangedBy: futnucks $
	$Revision: 43 $
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/samp.cpp $
	$Id: samp.cpp 43 2010-01-05 07:40:59Z futnucks $

*/

#include "main.h"

#define SAMP_DLL "samp.dll"
#define SAMP_CMP "766A8B060FB6481451E8"

int				g_renderSAMP_initSAMPstructs;
stSAMP			*g_SAMP;
stPlayerPool	*g_Players;
stVehiclePool	*g_Vehicles;
stChatInfo		*g_Chat;
stInputInfo		*g_Input;
stKillInfo		*g_DeathList;


// new functions to check for bad pointers
int isBadPtr_SAMP_iVehicleID(int iVehicleID)
{
	if (g_Vehicles == NULL) return 1;
	return !g_Vehicles->iIsListed[iVehicleID];
	// this hasn't been required yet
	//if (g_Vehicles->pSAMP_Vehicle[iVehicleID] == NULL) continue;
}

int isBadPtr_SAMP_iPlayerID(int iPlayerID)
{
	if (g_Players == NULL) return 1;
	return !g_Players->iIsListed[iPlayerID];
}




void getSamp()
{
	if(set.basic_mode) return;

	uint32_t samp_dll = getSampAddress();

	if(samp_dll != NULL)
	{
		g_dwSAMP_Addr = (uint32_t)samp_dll;

		if(g_dwSAMP_Addr != NULL)
		{
			if(!memcmp_safe((uint8_t *)g_dwSAMP_Addr + 0xBABE, hex_to_bin(SAMP_CMP), 10))
			{
				Log("Unknown SA:MP version. %s is only supported in this version.", SAMP_VERSION);
				g_dwSAMP_Addr = NULL;
				return;
			}

			Log("g_dwSAMP_Addr: 0x%p", g_dwSAMP_Addr);
		}
	}
	else
	{
		Log("Could not get the samp DLL.");
	}

	return;
}

uint32_t getSampAddress()
{
	if(set.run_mode == RUNMODE_SINGLEPLAYER) return 0x0;

	uint32_t samp_dll;

	if(set.run_mode == RUNMODE_SAMP) 
	{
		if(set.wine_compatibility)
		{
			HMODULE temp = LoadLibrary(SAMP_DLL);
			__asm mov samp_dll, eax
		}
		else
		{
			void *temp = dll_baseptr_get(SAMP_DLL);
			__asm mov samp_dll, eax
		}
	}

	if(samp_dll == NULL) return 0x0;

	return samp_dll;
}


struct stSAMP *stGetSampInfo(void)
{
	if(g_dwSAMP_Addr == NULL) return NULL;
	uint32_t info_ptr;
	info_ptr = (UINT_PTR)*(uint32_t *)((uint8_t *) (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_INFO_OFFSET));
	if(info_ptr == NULL) return NULL;
	
	return (struct stSAMP *)info_ptr;
}

struct stChatInfo *stGetSampChatInfo(void)
{
	if(g_dwSAMP_Addr == NULL) return NULL;
	uint32_t chat_ptr;
	chat_ptr = (UINT_PTR)*(uint32_t *)((uint8_t *) (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_CHAT_INFO_OFFSET));
	if(chat_ptr == NULL) return NULL;

	return (struct stChatInfo *)chat_ptr;
}

struct stInputInfo *stGetInputInfo(void)
{
	if(g_dwSAMP_Addr == NULL) return NULL;
	uint32_t input_ptr;
	input_ptr = (UINT_PTR)*(uint32_t *)((uint8_t *) (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_CHAT_INPUT_INFO_OFFSET));
	if(input_ptr == NULL) return NULL;

	return (struct stInputInfo *)input_ptr;
}

struct stKillInfo *stGetKillInfo(void)
{
	if(g_dwSAMP_Addr == NULL) return NULL;
	uint32_t kill_ptr;
	kill_ptr = (UINT_PTR)*(uint32_t *)((uint8_t *) (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_KILL_INFO_OFFSET));
	if(kill_ptr == NULL) return NULL;

	return (struct stKillInfo *)kill_ptr;
}

D3DCOLOR samp_color_get(int id)
{
	if(g_dwSAMP_Addr == NULL) return NULL;
	D3DCOLOR *color_table;
	if(id < 0 || id >= (SAMP_PLAYER_MAX + 3)) return D3DCOLOR_ARGB(0xFF, 0x99, 0x99, 0x99);
	
	switch(id)
	{
	case (SAMP_PLAYER_MAX): return 0xFF888888;
	case (SAMP_PLAYER_MAX + 1): return 0xFF0000AA;
	case (SAMP_PLAYER_MAX + 2): return 0xFF63C0E2;
	}

	color_table = (D3DCOLOR *)((uint8_t *)g_dwSAMP_Addr + SAMP_COLOR_OFFSET);
	return (color_table[id] >> 8) | 0xFF000000;
}

D3DCOLOR samp_color_get_trans(int id, DWORD trans)
{
	if(g_dwSAMP_Addr == NULL) return NULL;
	D3DCOLOR *color_table;
	if(id < 0 || id >= (SAMP_PLAYER_MAX + 3)) return D3DCOLOR_ARGB(0xFF, 0x99, 0x99, 0x99);
	
	switch(id)
	{
	case (SAMP_PLAYER_MAX): return 0xFF888888;
	case (SAMP_PLAYER_MAX + 1): return 0xFF0000AA;
	case (SAMP_PLAYER_MAX + 2): return 0xFF63C0E2;
	}

	color_table = (D3DCOLOR *)((uint8_t *)g_dwSAMP_Addr + SAMP_COLOR_OFFSET);
	return (color_table[id] >> 8) | trans;
}


extern int joining_server, iShowVehicles;
extern bool bShowChat;
void sampMainCheat()
{
	traceLastFunc("sampMainCheat()");


	// g_Vehicles & g_Players pointers need to be refreshed or nulled
	if (isBadPtr_writeAny(g_SAMP->pPool_Vehicle, sizeof(stVehiclePool)))
		g_Vehicles = NULL;
	else
		g_Vehicles = g_SAMP->pPool_Vehicle;
	if (isBadPtr_writeAny(g_SAMP->pPool_Player, sizeof(stPlayerPool)))
		g_Players = NULL;
	else
		g_Players = g_SAMP->pPool_Player;


	if(set.chatbox_logging)
	{
		static int chatbox_init;
		if(!chatbox_init)
		{
			SYSTEMTIME time;
			GetLocalTime(&time);
			LogChatbox(false, "Session started at %02d/%02d/%02d", time.wDay, time.wMonth, time.wYear);
			chatbox_init = 1;
		}
	}

	if(!cheat_state->render_player_tags) sampPatchDisableNameTags(1); else sampPatchDisableNameTags(0);

	if(KEY_PRESSED(set.key_map_show_vehicles))
		iShowVehicles ^= 1;

	if(KEY_DOWN(set.secondary_key))
	{
		if(KEY_PRESSED(set.key_player_info_list))
			cheat_state->player_info_list ^= 1;

		if(KEY_PRESSED(set.key_rejoin))
		{
			restartGame();
			disconnect(500);
			cheat_state_text("Rejoining in %d seconds...", set.rejoin_delay / 1000);
			cheat_state->_generic.rejoinTick = GetTickCount();
		}

		if(KEY_PRESSED(set.key_respawn))
			playerSpawn();

		if(KEY_PRESSED(set.key_render_player_tags))
			cheat_state->render_player_tags ^= 1;
		if(KEY_PRESSED(set.key_render_vehicle_tags))
			cheat_state->render_vehicle_tags ^= 1;
	}

	if(KEY_DOWN(set.chat_secondary_key))
	{
		if(KEY_PRESSED(set.d3dtext_chat_showhide_key))
		{
			if(bShowChat)
				bShowChat = false;
			else
				bShowChat = true;
		}

		int i, key, spam;
		const char *msg;
		for(i = 0; i < INI_CHATMSGS_MAX; i++)
		{
			struct chat_msg *msg_item = &set.chat[i];
			if(msg_item->key == NULL) continue;
			if(msg_item->msg == NULL) continue;
			if(msg_item->key != key_being_pressed) continue;
			key = msg_item->key;
			msg = msg_item->msg;
			spam = msg_item->spam;
			if(spam)
			{
				if(msg) if(KEY_DOWN(key)) say("%s", msg);
			}
			else
			{
				if(msg) if(KEY_PRESSED(key)) say("%s", msg);
			}
		}
	}

	static int iHooksInstalled;
	if(!iHooksInstalled)
	{
		installHooks();
		iHooksInstalled = 1;
	}

	if(cheat_state->_generic.rejoinTick && cheat_state->_generic.rejoinTick < (GetTickCount() - set.rejoin_delay))
	{
		g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
		cheat_state->_generic.rejoinTick = 0;
	}
	if(joining_server == 1)
	{
		restartGame();
		disconnect(500);
		cheat_state_text("Joining server in %d seconds...", set.rejoin_delay / 1000);
		cheat_state->_generic.join_serverTick = GetTickCount();
		joining_server = 2;
	}
	if(cheat_state->_generic.join_serverTick &&
	cheat_state->_generic.join_serverTick < (GetTickCount() - set.rejoin_delay) && joining_server == 2)
	{
		g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
		joining_server = 0;
		cheat_state->_generic.join_serverTick = 0;
	}
}


int getPlayerCount(void)
{
	if(g_Players == NULL) return NULL;
	
	int iCount = 0;
	int i;
	
	for(i = 0; i < SAMP_PLAYER_MAX; i++)
	{
		if(g_Players->iIsListed[i] != 1) continue;
		iCount++;
	}
	
	return iCount + 1;
}

int getVehicleCount(void)
{
	if(g_Vehicles == NULL) return NULL;

	int iCount = 0;
	int i;

	for(i = 0; i < SAMP_VEHICLE_MAX; i++)
	{
		if(g_Vehicles->iIsListed[i] != 1)  continue;
		iCount++;
	}

	return iCount;
}

int getPlayerPos(int iPlayerID, float fPos[3])
{
	traceLastFunc("getPlayerPos()");
	
	struct actor_info *pActor = NULL;
	struct vehicle_info *pVehicle = NULL;

	struct actor_info *pSelfActor = actor_info_get(ACTOR_SELF, 0);
	
	if(g_Players == NULL) return 0;
	if(g_Players->iIsListed[iPlayerID] != 1) return 0;
	if(g_Players->pRemotePlayer[iPlayerID] == NULL) return 0;

	if(g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor == NULL)
		return 0; // not streamed
	else
	{
		pActor = g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor->pGTA_Ped;

		if(g_Players->pRemotePlayer[iPlayerID]->pSAMP_Vehicle != NULL)
			pVehicle = g_Players->pRemotePlayer[iPlayerID]->pSAMP_Vehicle->pGTA_Vehicle;

		if(pVehicle != NULL && pActor->vehicle == pVehicle && pVehicle->passengers[0] == pActor)
		{
			// driver of a vehicle
			vect3_copy(&pActor->vehicle->base.matrix[4*3], fPos);
			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fVehiclePosition, fPos);
		}
		else if(pVehicle != NULL)
		{
			// passenger of a vehicle
			vect3_copy(&pActor->base.matrix[4*3], fPos);
			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fActorPosition, fPos);
		}
		else
		{
			// on foot
			vect3_copy(&pActor->base.matrix[4*3], fPos);
			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fActorPosition, fPos);
		}
	}
	
	if(pSelfActor != NULL)
	{
		if(vect3_dist(&pSelfActor->base.matrix[4*3], fPos) < 100.0f)
			vect3_copy(&pActor->base.matrix[4*3], fPos);
	}
	
	// detect zombies
	if(vect3_near_zero(fPos))
		vect3_copy(&pActor->base.matrix[4*3], fPos);
	
	return !vect3_near_zero(fPos);
}

const char *getPlayerName(int iPlayerID)
{
	if(g_Players == NULL || iPlayerID < 0 || iPlayerID >= SAMP_PLAYER_MAX) return NULL;
	if(iPlayerID == g_Players->sLocalPlayerID) return g_Players->szLocalPlayerName;
	if(g_Players->iIsListed[iPlayerID] != 1) return NULL;
	
	return g_Players->szPlayerName[iPlayerID];
}

int getPlayerState(int iPlayerID)
{
	if(g_Players == NULL || iPlayerID < 0 || iPlayerID >= SAMP_PLAYER_MAX) return NULL;
	if(iPlayerID == g_Players->sLocalPlayerID) return NULL;
	if(g_Players->iIsListed[iPlayerID] != 1) return NULL;

	return g_Players->pRemotePlayer[iPlayerID]->bytePlayerState;
}

int getPlayerVehicleGTAScriptingID(int iPlayerID)
{
	uint32_t car;

	if(iPlayerID != ACTOR_SELF)
	{
		if(g_Players == NULL)
		{
			ScriptCommand(&get_actor_car, 1, &car);
			return car;
		}

		ScriptCommand(&get_actor_car, g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor->ulGTA_Ped_ID, &car);
		return car;
	}
	else
	{
		ScriptCommand(&get_actor_car, 1, &car);
		return car;
	}
}

int getPlayerSAMPVehicleID(int iPlayerID)
{
	if(g_Players == NULL && g_Vehicles == NULL) return 0;
	if(g_Players->pRemotePlayer[iPlayerID] == NULL) return 0;
	if(g_Vehicles->pSAMP_Vehicle[g_Players->pRemotePlayer[iPlayerID]->sVehicleID] == NULL) return 0;

	int ret_id = (int)(((DWORD)g_Players->pRemotePlayer[iPlayerID]->pSAMP_Vehicle->pGTA_Vehicle) - 
		(DWORD)pool_vehicle->start)/2584;

	return ret_id;
//	return g_Players->pRemotePlayer[iPlayerID]->sVehicleID; -- sVehicleID = some samp intern vehicle id?
}






void spectatePlayer(int iID)
{
	if(getPlayerState(iID) == PLAYER_STATE_ONFOOT)
	{
		if(g_Players == NULL) return;
		ScriptCommand(&camera_on_actor, getPedGTAScriptingIDFromPlayerID(iID), 4, 1);
	}
	else if(getPlayerState(iID) == PLAYER_STATE_DRIVER)
	{
		if(g_Players == NULL || g_Vehicles == NULL) return;
		int iPlayerVehicleID = g_Players->pRemotePlayer[iID]->sVehicleID;
		ScriptCommand(&camera_on_vehicle, getVehicleGTAScriptingIDFromVehicleID(iPlayerVehicleID), 3, 1);
		
	}
	else if(getPlayerState(iID) == PLAYER_STATE_PASSENGER)
	{
		if(g_Players == NULL || g_Vehicles == NULL) return;
		int iPlayerVehicleID = g_Players->pRemotePlayer[iID]->sVehicleID;
		ScriptCommand(&camera_on_vehicle, getVehicleGTAScriptingIDFromVehicleID(iPlayerVehicleID), 3, 1);
	}
}


struct actor_info *getGTAPedFromSAMPPlayerID(int iPlayerID)
{
	if(g_Players == NULL || iPlayerID < 0 || iPlayerID >= SAMP_PLAYER_MAX) return NULL;
	if(iPlayerID == g_Players->sLocalPlayerID) return actor_info_get(ACTOR_SELF, 0);
	if(g_Players->iIsListed[iPlayerID] != 1) return NULL;
	if(g_Players->pRemotePlayer[iPlayerID] == NULL) return NULL;
	if(g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor == NULL) return NULL;
	if(g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor->pGTA_Ped == NULL) return NULL;
	
	return g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor->pGTA_Ped;
}

struct vehicle_info *getGTAVehicleFromSAMPVehicleID(int iVehicleID)
{
	if(g_Vehicles == NULL || iVehicleID < 0 || iVehicleID >= SAMP_VEHICLE_MAX) return NULL;
	if(iVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID) return vehicle_info_get(VEHICLE_SELF, 0);
	if(g_Vehicles->iIsListed[iVehicleID] != 1) return NULL;
	if(g_Vehicles->pGTA_Vehicle[iVehicleID] == NULL) return NULL;
	
	return g_Vehicles->pGTA_Vehicle[iVehicleID];
}

int getSAMPPlayerIDFromGTAPed(struct actor_info *pGTAPed)
{
	if(g_Players == NULL) return 0;
	if(actor_info_get(ACTOR_SELF, 0) == pGTAPed) return g_Players->sLocalPlayerID;
	
	int i;
	for(i = 0; i < SAMP_PLAYER_MAX; i++)
	{
		if(g_Players->iIsListed[i] != 1) continue;
		if(g_Players->pRemotePlayer[i] == NULL) continue;
		if(g_Players->pRemotePlayer[i]->pSAMP_Actor == NULL) continue;
		if(g_Players->pRemotePlayer[i]->pSAMP_Actor->pGTA_Ped == NULL) continue;
		if(g_Players->pRemotePlayer[i]->pSAMP_Actor->pGTA_Ped == pGTAPed) return i;
	}
	
	return ACTOR_SELF;
}

int getSAMPVehicleIDFromGTAVehicle(struct vehicle_info *pVehicle)
{
	if(g_Vehicles == NULL) return NULL;
	if(vehicle_info_get(VEHICLE_SELF, 0) == pVehicle
		&& g_Players != NULL)
		return g_Players->pLocalPlayer->sCurrentVehicleID;

	int i, iReturn = 0;
	for(i = 0; i < SAMP_VEHICLE_MAX; i++)
	{
		if(g_Vehicles->iIsListed[i] != 1) continue;
		if(g_Vehicles->pGTA_Vehicle[i] == pVehicle) return i;
	}

	return VEHICLE_SELF;
}

uint32_t getPedGTAScriptingIDFromPlayerID(int iPlayerID)
{
	if(g_Players == NULL) return NULL;

	if(g_Players->iIsListed[iPlayerID] != 1) return NULL;
	if(g_Players->pRemotePlayer[iPlayerID] == NULL) return NULL;
	if(g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor == NULL) return NULL;

	return g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor->ulGTA_Ped_ID;
}

uint32_t getVehicleGTAScriptingIDFromVehicleID(int iVehicleID)
{
	if(g_Vehicles == NULL) return NULL;

	if(g_Vehicles->iIsListed[iVehicleID] != 1) return NULL;
	if(g_Vehicles->pSAMP_Vehicle[iVehicleID] == NULL) return NULL;

	return g_Vehicles->pSAMP_Vehicle[iVehicleID]->ulGTA_Vehicle_ID;
}


struct gui *gui_samp_cheat_state_text = &set.guiset[2];
void addMessageToChatWindow(const char* text, ...)
{
	if(g_SAMP != NULL)
	{
		va_list ap;
		if(text == NULL) return;

		char tmp[512];
		memset(tmp,0,512);

		va_start(ap, text);
		vsprintf(tmp, text, ap);
		va_end(ap);
		
		addToChatWindow(tmp, D3DCOLOR_XRGB(gui_samp_cheat_state_text->red,
			gui_samp_cheat_state_text->green, gui_samp_cheat_state_text->blue));
	}
	else
	{
		va_list ap;
		if(text == NULL) return;

		char tmp[512];
		memset(tmp,0,512);

		va_start(ap, text);
		vsprintf(tmp, text, ap);
		va_end(ap);

		cheat_state_text(tmp, D3DCOLOR_ARGB(255,0,200,200));
	}
}

void addMessageToChatWindowSS(const char* text, ...)
{
	if(g_SAMP != NULL)
	{
		va_list ap;
		if(text == NULL) return;

		char tmp[512];
		memset(tmp,0,512);

		va_start(ap, text);
		vsprintf(tmp, text, ap);
		va_end(ap);

		addMessageToChatWindow(tmp, D3DCOLOR_ARGB(255,0,200,200));
	}
	else
	{
		va_list ap;
		if(text == NULL) return;

		char tmp[512];
		memset(tmp,0,512);

		va_start(ap, text);
		vsprintf(tmp, text, ap);
		va_end(ap);

		cheat_state_text(tmp, D3DCOLOR_ARGB(255,0,200,200));
	}
}

#define FUNC_ADDTOCHATWND 0x36590
void addToChatWindow(char* text, D3DCOLOR textColor)
{
	if(text == NULL) return;

	uint32_t chatinfo = g_dwSAMP_Addr + SAMP_CHAT_INFO_OFFSET;
	uint32_t func = g_dwSAMP_Addr + FUNC_ADDTOCHATWND;

	__asm mov eax, dword ptr [chatinfo]
	__asm mov ecx, dword ptr [eax]
	__asm push 0
	__asm push textColor
	__asm push 0
	__asm push text
	__asm push 2
	__asm call func
	__asm pop eax
	__asm pop ecx
}

#define FUNC_RESTARTGAME 0x74C0
void restartGame()
{
	uint32_t samp_info = g_dwSAMP_Addr + SAMP_INFO_OFFSET;
	uint32_t func = g_dwSAMP_Addr + FUNC_RESTARTGAME;
	__asm mov eax, dword ptr [samp_info]
	__asm mov ecx, dword ptr [eax]
	__asm call func
	__asm pop eax
	__asm pop ecx
}

void say(char *text, ...)
{
	if(text == NULL) return;
	if (isBadPtr_readAny(text,128)) return; 
	traceLastFunc("say()");

	va_list ap;
	char tmp[128];
	memset(tmp,0,128);

	va_start(ap, text);
	vsprintf(tmp, text, ap);
	va_end(ap);

	addSayToChatWindow(tmp);
}

#define FUNC_SAY 0x4010
#define FUNC_SENDCMD 0x378B0
void addSayToChatWindow(char *msg)
{
	if (msg == NULL) return;
	if (isBadPtr_readAny(msg,128)) return; 
	traceLastFunc("addSayToChatWindow()");

	if (msg[0] == '/')
	{

		uint32_t func = g_dwSAMP_Addr + FUNC_SENDCMD;
		__asm push msg
		__asm call func
	}
	else
	{
		uint32_t func = g_dwSAMP_Addr + FUNC_SAY;
		void* lpPtr = g_Players->pLocalPlayer;
		__asm mov ebx, dword ptr [lpPtr]
		__asm push msg
		__asm call func
		__asm pop ebx
	}
}

#define FUNC_GAMETEXT 0x2CAB0
void showGameText(char *text, int time, int textsize)
{
	uint32_t func = g_dwSAMP_Addr + FUNC_GAMETEXT;
	__asm push textsize
	__asm push time
	__asm push text
	__asm call func
}

#define FUNC_SPAWN 0x2E70
void playerSpawn(void)
{
	uint32_t func = g_dwSAMP_Addr + FUNC_SPAWN;
	void* lpPtr = g_Players->pLocalPlayer;
	__asm mov ecx, dword ptr [lpPtr]
	__asm call func
	__asm pop ecx
}

void disconnect(int reason /*0=timeout, 500=quit*/)
{
	void* rakptr = g_SAMP->pRakNet;
	__asm mov ecx, dword ptr [rakptr]
	__asm mov eax, dword ptr [ecx]
	__asm push 0
	__asm push reason
	__asm call dword ptr [eax+8]
	__asm pop ecx
	__asm pop eax
}

void setPassword(char* password)
{
	void* rakptr = g_SAMP->pRakNet;
	__asm mov ecx, dword ptr [rakptr]
	__asm mov eax, dword ptr [ecx]
	__asm push password
	__asm call dword ptr [eax+16]
	__asm pop ecx
	__asm pop eax
}

#define FUNC_SENDINTERIOR 0x3F70
void sendSetInterior(uint8_t interiorID)
{
	uint32_t func = g_dwSAMP_Addr + FUNC_SENDINTERIOR;
	void* lpPtr = g_Players->pLocalPlayer;
	__asm mov ecx, dword ptr [interiorID]
	__asm push ecx
	__asm mov ecx, dword ptr [lpPtr]
	__asm call func
	__asm pop ecx
}

#define FUNC_SETSPECIALACTION 0x2610
uint8_t g_byteSpecialAction = 0;
void setSpecialAction(uint8_t byteSpecialAction)
{
	if(g_Players->pLocalPlayer == NULL) return;

	g_Players->pLocalPlayer->onFootData.byteSpecialAction = byteSpecialAction;
	g_byteSpecialAction = byteSpecialAction;

	uint32_t func = g_dwSAMP_Addr + FUNC_SETSPECIALACTION;
	void* lpPtr = g_Players->pLocalPlayer;
	__asm mov ecx, dword ptr [byteSpecialAction]
	__asm push ecx
	__asm mov ecx, dword ptr [lpPtr]
	__asm call func
	__asm pop ecx
}

void sendSCMEvent(int iEvent, int iVehicleID, int iParam1, int iParam2)
{
	uint32_t func = g_dwSAMP_Addr + 0x1890;
	__asm push iParam2
	__asm push iParam1
	__asm push iVehicleID
	__asm push iEvent
	__asm call func
}


DWORD anticarjacked_ebx_backup;
DWORD anticarjacked_jmp;
uint8_t _declspec(naked) carjacked_hook(void)
{
	__asm mov anticarjacked_ebx_backup, ebx

	__asm pushad

	cheat_state->_generic.anti_carjackTick = GetTickCount();
	cheat_state->_generic.car_jacked = true;

	__asm popad

	__asm mov ebx, g_dwSAMP_Addr
	__asm add ebx, 0xCE26
	__asm mov anticarjacked_jmp, ebx
	__asm xor ebx, ebx
	__asm mov ebx, anticarjacked_ebx_backup
	__asm jmp anticarjacked_jmp
}

DWORD hook_streamedout_ebx_backup;
DWORD hook_streamedout_pos_continue;
int iStreamedOutPlayerID_temp;
float fStreamedOutPlayerPosX_temp,	fStreamedOutPlayerPosY_temp, fStreamedOutPlayerPosZ_temp;
stStreamedOutPlayerInfo g_stStreamedOutInfo;
uint8_t _declspec(naked) streamedout_pos_hook(void)
{
	__asm mov hook_streamedout_ebx_backup, ebx

	__asm mov ebx, dword ptr [esp+0xC]
	__asm mov iStreamedOutPlayerID_temp, ebx
	__asm mov ebx, dword ptr [esp]
	__asm mov fStreamedOutPlayerPosX_temp, ebx
	__asm mov ebx, dword ptr [esp+0x4]
	__asm mov fStreamedOutPlayerPosY_temp, ebx
	__asm mov ebx, dword ptr [esp+0x8]
	__asm mov fStreamedOutPlayerPosZ_temp, ebx

	__asm pushad
	g_stStreamedOutInfo.iPlayerID[iStreamedOutPlayerID_temp] = iStreamedOutPlayerID_temp;
	g_stStreamedOutInfo.fPlayerPos[iStreamedOutPlayerID_temp][0] = fStreamedOutPlayerPosX_temp;
	g_stStreamedOutInfo.fPlayerPos[iStreamedOutPlayerID_temp][1] = fStreamedOutPlayerPosY_temp;
	g_stStreamedOutInfo.fPlayerPos[iStreamedOutPlayerID_temp][2] = fStreamedOutPlayerPosZ_temp;
	__asm popad

	__asm push 0
	__asm mov ebx, g_dwSAMP_Addr
	__asm add ebx, 0x2CB30
	__asm call ebx

	__asm mov ebx, g_dwSAMP_Addr
	__asm add ebx, 0xB79C
	__asm mov hook_streamedout_pos_continue, ebx

	__asm mov ebx, hook_streamedout_ebx_backup

	__asm jmp hook_streamedout_pos_continue
}

DWORD chatboxlog_hook_ebx_backup, chatboxlog_hook_continue;
int chatboxlog_hook_type;
char *chatboxlog_hook_nickname, *chatboxlog_hook_string;
uint8_t _declspec(naked) chatboxlog_hook(void)
{
	__asm mov chatboxlog_hook_ebx_backup, ebx

	__asm mov ebx, dword ptr [esp+0x4]
	__asm mov chatboxlog_hook_type, ebx
	__asm mov ebx, dword ptr [esp+0x8]
	__asm mov chatboxlog_hook_string, ebx
	__asm mov ebx, dword ptr [esp+0xC]
	__asm mov chatboxlog_hook_nickname, ebx

	__asm pushad
	switch(chatboxlog_hook_type)
	{
	case 0:
		break;
	case 1:
		LogChatbox(false, "%s: %s", chatboxlog_hook_nickname, chatboxlog_hook_string);
		break;
	case 2:
		LogChatbox(false, "%s", chatboxlog_hook_string);
		break;
	case 3:
		LogChatbox(false, "%s", chatboxlog_hook_string);
		break;
	}
	__asm popad

	__asm mov ebx, g_dwSAMP_Addr
	__asm add ebx, 0x36596
	__asm mov chatboxlog_hook_continue, ebx

	__asm mov ebx, chatboxlog_hook_ebx_backup

	__asm mov eax, dword ptr [esp+0xC]
	__asm test eax, eax

	__asm jmp chatboxlog_hook_continue
}

#define SAMP_HOOK_STATECHANGE 0xCE15
#define SAMP_HOOK_STREAMEDOUT_POS 0xB795
#define SAMP_HOOK_CHATBOXLOG 0x36590
#ifdef M0D_DEV
 #define SAMP_HOOK_RPCRETR 0x1A779
 #define SAMP_HOOK_RPCRETR2 0x1A6C2
#endif
void installHooks()
{
	CDetour api;

	if(set.anti_carjacking)
	{
		if(memcmp_safe((uint8_t *)g_dwSAMP_Addr + SAMP_HOOK_STATECHANGE, hex_to_bin("6A0568"), 3))
		{
			if(api.Create((uint8_t *)((uint32_t )g_dwSAMP_Addr) + SAMP_HOOK_STATECHANGE, (uint8_t *)carjacked_hook, DETOUR_TYPE_JMP, 5) == 0)
				Log("Failed to hook StateChange.");
		} else
			Log("Failed to hook StateChange (memcmp)");
	}

	if(memcmp_safe((uint8_t *)g_dwSAMP_Addr + SAMP_HOOK_STREAMEDOUT_POS, hex_to_bin("6A00E8"), 3))
	{
		if(api.Create((uint8_t *)((uint32_t )g_dwSAMP_Addr) + SAMP_HOOK_STREAMEDOUT_POS, (uint8_t *)streamedout_pos_hook, DETOUR_TYPE_JMP, 5) == 0)
			Log("Failed to hook streamedout_pos_hook.");
	} else
		Log("Failed to hook streamedout_pos_hook (memcmp)");

	if(set.chatbox_logging)
	{
		if(memcmp_safe((uint8_t *)g_dwSAMP_Addr + SAMP_HOOK_CHATBOXLOG, hex_to_bin("8B4424"), 3))
		{
			if(api.Create((uint8_t *)((uint32_t )g_dwSAMP_Addr) + SAMP_HOOK_CHATBOXLOG, (uint8_t *)chatboxlog_hook, DETOUR_TYPE_JMP, 5) == 0)
				Log("Failed to hook chatboxlog_hook.");
		} else
			Log("Failed to hook chatboxlog_hook (memcmp)");
	}
}



#define SAMP_CAMPOS					0x7FC4
#define SAMP_LOOKATPOS				0x7FE2
#define SAMP_POSFORSTREAMINVEH		0x7F86
#define SAMP_POSFORSTREAMONFOOT		0x7FA3
void setSAMPInitScreenMatrix(float camposX, float camposY, float camposZ,
							 float camlookatX, float camlookatY, float camlookatZ)
{
	if(!set.samp_custom_init_screen_enable) return;
	if(g_dwSAMP_Addr == NULL) return;
	if(g_SAMP == NULL) return;

	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_CAMPOS), &camposX, sizeof(float));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_CAMPOS - 0x5), &camposY, sizeof(float));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_CAMPOS - 0xA), &camposZ, sizeof(float));
	
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_LOOKATPOS), &camlookatX, sizeof(float));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_LOOKATPOS - 0x5), &camlookatY, sizeof(float));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_LOOKATPOS - 0xA), &camlookatZ, sizeof(float));

	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_POSFORSTREAMINVEH), &camposX, sizeof(float));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_POSFORSTREAMINVEH - 0x5), &camposY, sizeof(float));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_POSFORSTREAMINVEH - 0xA), &camposZ, sizeof(float));

	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_POSFORSTREAMONFOOT), &camposX, sizeof(float));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_POSFORSTREAMONFOOT - 0x5), &camposY, sizeof(float));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_POSFORSTREAMONFOOT - 0xA), &camposZ, sizeof(float));
}

#define SAMP_ONFOOTSENDRATE 0x67098
#define SAMP_INCARSENDRATE 0x6709C
#define SAMP_AIMSENDRATE 0x670A0
#define SAMP_HEADSYNCSENDRATE 0x5B53
void setSAMPCustomSendRates(int iOnFoot, int iInCar, int iAim, int iHeadSync)
{
	if(!set.samp_custom_sendrates_enable) return;
	if(g_dwSAMP_Addr == NULL) return;
	if(g_SAMP == NULL) return;
	
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_ONFOOTSENDRATE), &iOnFoot, sizeof(int));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_INCARSENDRATE), &iInCar, sizeof(int));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_AIMSENDRATE), &iAim, sizeof(int));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_HEADSYNCSENDRATE), &iHeadSync, sizeof(int));
}



#define SAMP_DISABLE_NAMETAGS 0x3D7B0
int sampPatchDisableNameTags(int iEnabled)
{
   static struct patch_set patch = {
         "Remove player status", 0, 0, {
         { 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_DISABLE_NAMETAGS), NULL, (uint8_t *)"\xC3", NULL }
      }
   };

   if(iEnabled)
      return patcher_remove(&patch);
   else
      return patcher_install(&patch);
}

#define SAMP_SENDINTERIOR 0x56DF
#define SAMP_SETINTERIOR 0x311C0
int sampPatchDisableInteriorUpdate(int iEnabled)
{
   static struct patch_set patch = {
         "NOP sendinterior", 0, 0, {
			 { 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_SENDINTERIOR), NULL, (uint8_t *)"\xEB", NULL },
			 { 3, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_SETINTERIOR), NULL, (uint8_t *)"\xC2\x04\x00", NULL }
      }
   };

   if(iEnabled)
      return patcher_install(&patch);
   else
      return patcher_remove(&patch);
}

#define SAMP_NOPSCREENSHOT 0x3E71D
int sampPatchDisableScreeenshotKey(int iEnabled)
{
   static struct patch_set patch = {
         "NOP screenshot key", 0, 0, {
			 { 6, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_NOPSCREENSHOT), NULL, (uint8_t *)"\xE9\x4A\x01\x00\x00\x90", NULL },
      }
   };

   if(iEnabled)
      return patcher_install(&patch);
   else
      return patcher_remove(&patch);
}
