DWORD WINAPI voiceThread(PVOID);

extern RakVoice rakVoice;
extern RakPeerInterface *rakPeer;

extern WDL_SharedMutex g_rV_OutputPCM;
extern WDL_SharedMutex g_rV_InputPCM;

extern int iVoiceEnabled;

extern unsigned short sTempPlayerID;

struct stPlayerTalking
{
	int iIsChannelOpened;
	int isTalking;
	int isMuted;
	int talkingTime;
};

extern struct stPlayerTalking g_playerTalking[SAMP_PLAYER_MAX];
