#include <main.h>
#include "portaudio.h"

#define SAMP_NETWORK_ID_OFFSET 0x22FF05

RakVoice rakVoice;
RakPeerInterface *rakPeer;

WDL_SharedMutex g_rV_OutputPCM;
WDL_SharedMutex g_rV_InputPCM;

// RakVoice only works with 16-bits sound data
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;

// Reads and writes per second of the sound data
// Speex only supports these 3 values
#define SAMPLE_RATE  (8000)
//#define SAMPLE_RATE  (16000)
//#define SAMPLE_RATE  (32000)

// I think one buffer has to be full (of samples) before you hear the sound.
// So higher frames per buffer means that there will be a larger latency before you hear the sound
// However, it would lock and unlock the buffer more often, hindering performance.
#define FRAMES_PER_BUFFER  (2048 / (32000 / SAMPLE_RATE))

int iVoiceEnabled = 0;
struct stPlayerTalking g_playerTalking[SAMP_PLAYER_MAX];

unsigned short sTempPlayerID; // player that is going to talk on this loop

static int PACallback( void *inputBuffer, void *outputBuffer,
					  unsigned long framesPerBuffer,
					  PaTimestamp outTime, void *userData )
{
	if(gta_menu_active())
	{
		memset(outputBuffer, 0, FRAMES_PER_BUFFER*sizeof(SAMPLE));
		return 0;
	}

	if (inputBuffer && iVoiceEnabled)
		rakVoice.SendFrame(inputBuffer);

	rakVoice.ReceiveFrame(outputBuffer);

	return 0;
}

#define SAMP_HOOKPOS_ReceiveHook	0x94CB
DWORD rakNet_receive_thread_return;
Packet *rakNet_receive_thread_pktptr;
uint8_t _declspec ( naked ) rakNet_receive_thread ( void )
{
	_asm
	{
		mov ecx, dword ptr [edi+0x10]
		mov al, byte ptr [ecx]

		mov rakNet_receive_thread_pktptr, edi
		pushad
	}

	if(!gta_menu_active())
		rakVoice.OnReceive(rakNet_receive_thread_pktptr);

	_asm
	{
		popad

		push eax
		mov eax, g_dwSAMP_Addr
		add eax, SAMP_HOOKPOS_ReceiveHook
		add eax, 5
		mov rakNet_receive_thread_return, eax
		pop eax

		jmp rakNet_receive_thread_return
	}

}

DWORD WINAPI voiceThread(PVOID)
{
	memset(&g_playerTalking, 0, sizeof(stPlayerTalking));

	// receive hook
	CDetour api;
	if ( memcmp_safe((uint8_t *)g_dwSAMP_Addr + SAMP_HOOKPOS_ReceiveHook, hex_to_bin("8B4F108A01"), 5) )
	{
		if ( api.Create((uint8_t *) ((uint32_t) g_dwSAMP_Addr) + SAMP_HOOKPOS_ReceiveHook,
			(uint8_t *)rakNet_receive_thread, DETOUR_TYPE_JMP, 5) == 0 )
			Log( "Failed to hook rakNet_receive_thread." );
	}
	else
		Log( "Failed to hook rakNet_receive_thread (memcmp)" );


	// Wait until samp's rakpeer is available
	while(1)
	{
		if(g_SAMP == NULL) return 0;
		if(g_SAMP->pRakClientInterface != NULL)
		{
			rakPeer = (RakPeerInterface *)g_SAMP->pRakClientInterface;
			_asm
			{
				mov edx, rakPeer
				sub edx, 0x9EA
				mov rakPeer, edx
			}
			if(rakPeer != NULL)
				break;
			else
				Sleep(1000);
		}
	}
	
	rakVoice.Init(SAMPLE_RATE, FRAMES_PER_BUFFER*sizeof(SAMPLE), rakPeer);
	rakVoice.SetVAD(false);

	PaError    err;
	PortAudioStream *stream;
	err = Pa_Initialize();
	if( err != paNoError ) goto error;

	err = Pa_OpenStream(
		&stream,
		Pa_GetDefaultInputDeviceID(),
		1, // Num channels, whatever that means
		PA_SAMPLE_TYPE,
		NULL,
		Pa_GetDefaultOutputDeviceID(),
		1, // Num channels
		PA_SAMPLE_TYPE,
		NULL,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,            /* frames per buffer */
		0,               /* number of buffers, if zero then use default minimum */
		0, /* paDitherOff, // flags */
		PACallback,
		0 );

	if( err != paNoError ) goto error;

	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;

	while(1)
	{
		rakVoice.Update();

		Pa_Sleep(30);
	}

	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto error;

error:
	rakVoice.Deinit();
	Pa_Terminate();

	return 0;
}
