#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
#include "main.h"

#define SAMP_PLAYER_MAX 500

#ifdef WIN32
#define SAMP_SVRINFO_OFFSET				0x4F6270
#define SAMP_NETWORKID					0x4A0225
#define RECEIVE_HOOKPOS					0x49BA65
#else
#define SAMP_SVRINFO_OFFSET				0x81837BC
#define SAMP_NETWORKID					0x8081568
#define RECEIVE_HOOKPOS					0x807C4E8
#endif

RakPeerInterface *rakPeer;

logprintf_t logprintf;
void **ppPluginData;
extern void *pAMXFunctions;
AMX *AMX_Scripts[17];

struct stPlayerVoice
{
	unsigned short messageNumber;
};
struct stPlayerVoice g_playerVoice[SAMP_PLAYER_MAX];

void installJump(unsigned long addr, void *func)
{
#ifdef WIN32
	unsigned long dwOld;
	VirtualProtect((LPVOID)addr, 5, PAGE_EXECUTE_READWRITE, &dwOld);
#else
	int pagesize = sysconf(_SC_PAGE_SIZE);
	void *unpraddr = (void *)(((int)addr + pagesize - 1) & ~(pagesize - 1)) - pagesize;
	mprotect(unpraddr, pagesize, PROT_WRITE);
#endif
	*(unsigned char *)addr = 0xE9;
	*(unsigned long *)((unsigned long)addr + 0x1) = (unsigned long)func - (unsigned long)addr - 5;
#ifdef WIN32
	VirtualProtect((LPVOID)addr, 5, dwOld, &dwOld);
#else
	mprotect(unpraddr, pagesize, PROT_READ | PROT_EXEC);
#endif
}

#ifdef WIN32
DWORD WINAPI getRakPeerPtrThread(PVOID)
#else
void *getRakPeerPtrThread(void *)
#endif
{
	while(1)
	{
		unsigned long temp;

		temp = (unsigned long)(void *)SAMP_SVRINFO_OFFSET;
		if(temp == NULL) goto ptr_null;
		logprintf("1: %X\n", temp);
		temp = (unsigned long)*(void **)temp;
		if(temp == NULL) goto ptr_null;
		logprintf("2: %X\n", temp);
		temp = (unsigned long)*(void **)temp;
		if(temp == NULL) goto ptr_null;
		logprintf("3: %X\n", temp);
#ifdef WIN32
		temp += 8;
		if(temp == NULL) goto ptr_null;
#endif
		rakPeer = (RakPeerInterface *)temp;
		//break;

ptr_null:;
#ifdef WIN32
		Sleep(5);
#else
		usleep(5);
#endif
	}

	return 0;
}

// callbacks
int iAMXIndex, iScriptIter;
cell OnPlayerOpenChannelRequest_ret, OnPlayerCloseChannel_ret, OnPlayerVoice_ret;
int AMX_CALLBACK_OnPlayerOpenChannelRequest(int iPlayerID)
{
	for(iScriptIter = 0; iScriptIter < 17; iScriptIter++)
	{
		if(AMX_Scripts[iScriptIter] != NULL)
		{
			if(amx_FindPublic(AMX_Scripts[iScriptIter], "OnPlayerOpenChannelRequest", &iAMXIndex) == AMX_ERR_NONE)
			{
				amx_Push(AMX_Scripts[iScriptIter], iPlayerID);
				amx_Exec(AMX_Scripts[iScriptIter], &OnPlayerOpenChannelRequest_ret, iAMXIndex);

				return OnPlayerOpenChannelRequest_ret;
			}
		}
	}

	return 1;
}
int AMX_CALLBACK_OnPlayerCloseChannel(int iPlayerID)
{
	for(iScriptIter = 0; iScriptIter < 17; iScriptIter++)
	{
		if(AMX_Scripts[iScriptIter] != NULL)
		{
			if(amx_FindPublic(AMX_Scripts[iScriptIter], "OnPlayerCloseChannel", &iAMXIndex) == AMX_ERR_NONE)
			{
				amx_Push(AMX_Scripts[iScriptIter], iPlayerID);
				amx_Exec(AMX_Scripts[iScriptIter], &OnPlayerCloseChannel_ret, iAMXIndex);

				return OnPlayerCloseChannel_ret;
			}
		}
	}

	return 1;
}
int AMX_CALLBACK_OnPlayerVoice(int iPlayerID, int iMessageNumber)
{
	for(iScriptIter = 0; iScriptIter < 17; iScriptIter++)
	{
		if(AMX_Scripts[iScriptIter] != NULL)
		{
			if(amx_FindPublic(AMX_Scripts[iScriptIter], "OnPlayerVoice", &iAMXIndex) == AMX_ERR_NONE)
			{
				amx_Push(AMX_Scripts[iScriptIter], iMessageNumber);
				amx_Push(AMX_Scripts[iScriptIter], iPlayerID);
				amx_Exec(AMX_Scripts[iScriptIter], &OnPlayerVoice_ret, iAMXIndex);

				return OnPlayerVoice_ret;
			}
		}
	}

	return 1;
}

unsigned long rakNet_receive_thread_return;
Packet *rakNet_receive_thread_pktptr;
unsigned char rakNet_receive_thread_packetid;
unsigned char *pInData;
RakNet::BitStream bOut;
int inLength;
PlayerIndex playerIndex;
void processVoicePackets()
{
	playerIndex = rakNet_receive_thread_pktptr->playerIndex;

	// invalid
	if(playerIndex < 0 || playerIndex > SAMP_PLAYER_MAX)
		return;

	if(rakNet_receive_thread_packetid == ID_RAKVOICE_OPEN_CHANNEL_REQUEST)
	{
		if(!AMX_CALLBACK_OnPlayerOpenChannelRequest((int)playerIndex))
			return;

		bOut.Reset();
		bOut.Write((unsigned char)ID_RAKVOICE_OPEN_CHANNEL_REPLY);
		bOut.Write(playerIndex);
		rakPeer->Send(&bOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true);
	}
	else if(rakNet_receive_thread_packetid == ID_RAKVOICE_CLOSE_CHANNEL)
	{
		if(!AMX_CALLBACK_OnPlayerCloseChannel(playerIndex))
			return;

		bOut.Reset();
		bOut.Write((unsigned char)ID_RAKVOICE_CLOSE_CHANNEL);
		bOut.Write(playerIndex);
		rakPeer->Send(&bOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true);
	}
	else if(rakNet_receive_thread_packetid == ID_RAKVOICE_DATA)
	{
		memcpy(&g_playerVoice[playerIndex].messageNumber, &rakNet_receive_thread_pktptr->data[1], sizeof(unsigned short));

		if(!AMX_CALLBACK_OnPlayerVoice((int)playerIndex, (int)g_playerVoice[playerIndex].messageNumber))
			return;

		bOut.Reset();
		bOut.Write((unsigned char)ID_RAKVOICE_DATA);
		bOut.Write(playerIndex);
		bOut.Write(g_playerVoice[playerIndex].messageNumber);

		pInData = (unsigned char *)&rakNet_receive_thread_pktptr->data[3];
		inLength = (rakNet_receive_thread_pktptr->length - 3);
		bOut.Write((char *)pInData, inLength);

		rakPeer->Send(&bOut, HIGH_PRIORITY, UNRELIABLE, 0, rakNet_receive_thread_pktptr->playerId, true);
	}
}

// receive hook
#ifdef WIN32
unsigned char _declspec(naked) rakNet_receive_thread ( void )
#else
unsigned char /*__attribute__((naked))*/ rakNet_receive_thread ( void )
#endif
{
#ifdef WIN32
	_asm
	{
		mov ecx, dword ptr [esi+0x10]
		mov al, byte ptr [ecx]

		mov rakNet_receive_thread_packetid, al
		mov rakNet_receive_thread_pktptr, esi
		pushad
	}
#else

	// hax cos the naked doesn't work :s
	__asm(
	".intel_syntax noprefix\n"
		"add esp, 8\n"
		"pop ebp\n"
	".att_syntax\n"
	);

	__asm(
	".intel_syntax noprefix\n"

		"push eax\n"
		"mov eax, dword ptr [esi]\n"
		"mov rakPeer, eax\n"
		"pop eax\n"

		"mov rakNet_receive_thread_packetid, al\n"
		"mov rakNet_receive_thread_pktptr, ebx\n"
		"pushad\n"
	".att_syntax\n"
	);

#endif

	processVoicePackets();

#ifdef WIN32
	_asm
	{
		popad

		push eax
		mov eax, RECEIVE_HOOKPOS
		add eax, 5
		mov rakNet_receive_thread_return, eax
		pop eax

		jmp rakNet_receive_thread_return
	}
#else
	__asm(
	".intel_syntax noprefix\n"
		"popad\n"

		"push eax\n"
	".att_syntax\n"
	);

	__asm(
	".intel_syntax noprefix\n"
		"mov eax, %0\n"
	".att_syntax\n"
		:
		: "r"(RECEIVE_HOOKPOS)
	);

	__asm(
	".intel_syntax noprefix\n"
		"add eax, 6\n"
		"mov rakNet_receive_thread_return, eax\n"
		"pop eax\n"

		"movzx eax, al\n"
		"sub eax, 0x20\n"

		"jmp dword ptr [rakNet_receive_thread_return]\n"
	".att_syntax\n"
	);
#endif
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	// change server network id
	/*DWORD a;
	VirtualProtect((void *)SAMP_NETWORKID, 1, PAGE_EXECUTE_READWRITE, &a);
	memcpy((void *)SAMP_NETWORKID, "\x7F", 1);*/

	memset(&g_playerVoice, 0, sizeof(g_playerVoice));

	// get rakpeer
#ifdef WIN32
	CreateThread(NULL, 0, getRakPeerPtrThread, NULL, 0, NULL);
#else
	/*pthread_t threadHandle;
	int error = pthread_create(&threadHandle, NULL, &getRakPeerPtrThread, 0);
	assert(error == 0);*/
#endif

	// receive hook
#ifdef WIN32
	if(memcmp((void *)RECEIVE_HOOKPOS, "\x8B\x4F\x10\x8A\x01", 5))
#else
	if(!memcmp((void *)RECEIVE_HOOKPOS, "\x0F\xB6\xC0\x83\xE8\x20", 6))
#endif
	{
		installJump(RECEIVE_HOOKPOS, (void*)rakNet_receive_thread);
	}
	else
		logprintf( "Failed to hook rakNet_receive_thread (memcmp)\n" );

	logprintf( "  Plugin " PLUGIN_FILE " got loaded." );

	return true;
}


PLUGIN_EXPORT void PLUGIN_CALL Unload( )
{
	for(int i = 0; i < 17; i++)
		AMX_Scripts[i] = NULL;

	logprintf( "  Plugin " PLUGIN_FILE " got unloaded." );
}


// natives
RakNet::BitStream bOutfnCloseChannel;
static cell AMX_NATIVE_CALL fnBroadcastCloseChannel( AMX* amx, cell* params )
{
	if(params[0] != (1 * 4))
	{
		logprintf("*** fnBroadcastCloseChannel: Expecting %d parameter(s), but found %d\n", 1, params[0] / 4);

		return 0;
	}

	PlayerIndex playerIndex = (PlayerIndex)params[1];

	bOutfnCloseChannel.Reset();
	bOutfnCloseChannel.Write((unsigned char)ID_RAKVOICE_CLOSE_CHANNEL);
	bOutfnCloseChannel.Write(playerIndex);
	rakPeer->Send(&bOutfnCloseChannel, HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakPeer->GetPlayerIDFromIndex(playerIndex), true);

	return 1;
}


AMX_NATIVE_INFO HelloWorldNatives[ ] =
{
	{ "BroadcastCloseChannel", fnBroadcastCloseChannel },
	{ 0,					0 }
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx ) 
{
	for(int i = 0; i < 17; i++)
	{
		if(AMX_Scripts[i] == 0)
		{
			AMX_Scripts[i] = amx;
			break;
		}
	}

	return amx_Register( amx, HelloWorldNatives, -1 );
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{
	for(int i = 0; i < 17; i++)
	{
		if(AMX_Scripts[i] == amx)
		{
			AMX_Scripts[i] = 0;
			break;
		}
	}

	return AMX_ERR_NONE;
}
