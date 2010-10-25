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

*/

#define SAMP_PLAYER_MAX				503
#define SAMP_VEHICLE_MAX			2010
#define SAMP_PICKUP_MAX				2048
#define SAMP_OBJECTS_MAX			400
#define MAX_3DTEXT					2048
#define MAX_TEXTDRAW				2048
#define MAX_GANGZONES				1024
#define MAX_PLAYER_NAME				30
#define ALLOWED_PLAYER_NAME_LENGTH	20
#define MAX_CLIENTCMDS				144
#define SAMP_MENUS_MAX				128

// Variable offsets
#define SAMP_CHAT_INFO_OFFSET		0xEEF68
#define SAMP_CHAT_INPUT_INFO_OFFSET 0xEEF6C
#define SAMP_KILL_INFO_OFFSET		0xEEF70
#define SAMP_INFO_OFFSET			0xEEF78

#define SAMP_COLOR_OFFSET			0xD5F10

#define SAMP_FONT_INFO_OFFSET			0xEEF7C

#define VALUE_DRAWING_DISTANCE			0x637FC

#define GAMESTATE_NONE					0
#define GAMESTATE_AWAIT_JOIN			218
#define GAMESTATE_RESTARTING			219
#define GAMESTATE_WAIT_CONNECT			220
#define GAMESTATE_CONNECTING			221
#define GAMESTATE_CONNECTED				222
#define GAMESTATE_DISCONNECTED			223

#define PLAYER_STATE_NONE				0
#define PLAYER_STATE_ONFOOT				170
#define PLAYER_STATE_DRIVER				218
#define PLAYER_STATE_PASSENGER			202
#define PLAYER_STATE_WASTED				42
#define PLAYER_STATE_SPAWNED			154

#define PLAYER_MARKERS_MODE_OFF			0
#define PLAYER_MARKERS_MODE_GLOBAL		1
#define PLAYER_MARKERS_MODE_STREAMED	2

#define SPECIAL_ACTION_NONE				0
#define SPECIAL_ACTION_DUCK				1
#define SPECIAL_ACTION_USEJETPACK		2
#define SPECIAL_ACTION_ENTER_VEHICLE	3
#define SPECIAL_ACTION_EXIT_VEHICLE		4
#define SPECIAL_ACTION_DANCE1			5
#define SPECIAL_ACTION_DANCE2			6
#define SPECIAL_ACTION_DANCE3			7
#define SPECIAL_ACTION_DANCE4			8
#define SPECIAL_ACTION_HANDSUP			10
#define SPECIAL_ACTION_USECELLPHONE		11
#define SPECIAL_ACTION_SITTING			12
#define SPECIAL_ACTION_STOPUSECELLPHONE 13
#define SPECIAL_ACTION_DRINK_BEER		20
#define SPECIAL_ACTION_SMOKE_CIGGY		21
#define SPECIAL_ACTION_DRINK_WINE		22
#define SPECIAL_ACTION_DRINK_SPRUNK		23

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// SAMP STRUCTURES ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
struct stSAMP
{
#pragma pack( 1 )
	struct stSAMPPools		*pPools;
	void					*unk[2];
	struct stSampSettings	*pSettings;
	void					*pRakNet;
	uint32_t				*ulConnectTick;
	int						iGameState;
	int						iLanMode;
	uint32_t				ulPort;
	char					szIP[256];
	uint8_t					unknown[2];
	char					szHostname[256];
	uint8_t					unknown_two[2];
	uint32_t				ulMapIcons[100];
};

struct stSAMPPools
{
#pragma pack( 1 )
	struct stTextdrawPool	*pPool_Textdraw;
	struct stObjectPool		*pPool_Object;
	struct stPlayerPool		*pPool_Player;
	struct stVehiclePool	*pPool_Vehicle;
	void					*unk_2;
	struct stTextLabelPool	*pPool_Text3D;
	struct stPickupPool		*pPool_Pickup;
	void					*pPool_Menu;
};

struct stSampSettings
{
#pragma pack( 1 )
	uint8_t byteAllowFriendlyFire;
	uint8_t unknown;
	uint8_t byteSetTime;
	uint8_t unknown_2;
	uint8_t byteShowZoneNames;
	uint8_t unknown_2_2;
	uint8_t byteCJWalk;
	uint8_t byteWeather;
	uint8_t byteNoNametagsBehindWalls;
	float	fGravity;
	uint8_t unknown_3[4];
	uint8_t byteNoInteriorEnterExits;
	int		iSpawnsAvailable;
	uint8_t bytePlayerMarkersMode;
	uint8_t unknown_5[7];
	float	fNameTagsDistance;
	uint8_t byteShowNameTags;
	uint8_t byteWorldTime_Hour;
	uint8_t byteWorldTime_Minute;
	float	fWorldBoundaries[4];
};

struct stTextdraw
{
#pragma pack( 1 )
	char	text[1024];
	char	unknown[2095];
	float	position[2];
	char	unknown_[16];
};

struct stTextdrawPool
{
#pragma pack( 1 )
	int					iIsListed[MAX_TEXTDRAW];
	struct stTextdraw	*textdraw[MAX_TEXTDRAW];
};

struct stPickup
{
#pragma pack( 1 )
	int		iModelID;
	int		iType;
	float	fPosition[3];
};

struct stPickupPool
{
#pragma pack( 1 )
	struct stPickup pickup[SAMP_PICKUP_MAX];
};

struct stPlayerPool
{
#pragma pack( 1 )
	struct stLocalPlayer	*pLocalPlayer;
	int						iLocalPlayerScore;
	int						iLocalPlayerPing;
	short					sLocalPlayerID;
	uint8_t					unknown_x[4];
	union
	{
		char	szLocalPlayerName[20];
		char	*pszLocalPlayerName;
	};
	int						iStrlen_LocalPlayerName;
	int						iIsListed[SAMP_PLAYER_MAX];
	struct stRemotePlayer	*pRemotePlayer[SAMP_PLAYER_MAX];
};

struct stSAMPKeys
{
#pragma pack( 1 )
	uint8_t keys_primaryFire : 1;
	uint8_t keys_horn__crouch : 1;
	uint8_t keys_secondaryFire__shoot : 1;
	uint8_t keys_accel__zoomOut : 1;
	uint8_t keys_enterExitCar : 1;
	uint8_t keys_decel__jump : 1;			// on foot: jump or zoom in
	uint8_t keys_circleRight : 1;
	uint8_t keys_aim : 1;					// hydra auto aim or on foot aim
	uint8_t keys_circleLeft : 1;
	uint8_t keys_landingGear__lookback : 1;
	uint8_t keys_unknown__walkSlow : 1;
	uint8_t keys_specialCtrlUp : 1;
	uint8_t keys_specialCtrlDown : 1;
	uint8_t keys_specialCtrlLeft : 1;
	uint8_t keys_specialCtrlRight : 1;
	uint8_t keys__unused : 1;
};

struct stOnFootData
{
#pragma pack( 1 )
	uint16_t	sLeftRightKeys;
	uint16_t	sUpDownKeys;
	union
	{
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float		fPosition[3];
	float		fQuaternion[4];
	uint8_t		byteHealth;
	uint8_t		byteArmor;
	uint8_t		byteCurrentWeapon;
	uint8_t		byteSpecialAction;
	float		fMoveSpeed[3];
	float		fSurfingOffsets[3];
	uint16_t	sSurfingVehicleID;
	int			iCurrentAnimationID;
};

struct stInCarData
{
#pragma pack( 1 )
	uint16_t	sVehicleID;
	uint16_t	sLeftRightKeys;
	uint16_t	sUpDownKeys;
	union
	{
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float		fQuaternion[4];
	float		fPosition[3];
	float		fMoveSpeed[3];
	float		fVehicleHealth;
	uint8_t		bytePlayerHealth;
	uint8_t		byteArmor;
	uint8_t		byteCurrentWeapon;
	uint8_t		byteSiren;
	uint8_t		byteLandingGearState;
	uint16_t	sTrailerID;
	union
	{
		uint16_t	HydraThrustAngle[2];	//nearly same value
		float		fTrainSpeed;
	};
};

struct stAimData
{
#pragma pack( 1 )
	uint8_t byteMode;
	float	fFrontVector[3];
	float	fCamera_position[3];
	float	fAimZAxis;
	uint8_t byteUnknown_1[2];
};

struct stTrailerData
{
#pragma pack( 1 )
	uint16_t	sTrailerID;
	float		fDirection[3];
	float		fRoll[3];
	float		fPosition[3];
	float		fSpeed[3];
};

struct stPassengerData
{
#pragma pack( 1 )
	uint16_t	sVehicleID;
	uint8_t		byteSeatID;
	uint8_t		byteCurrentWeapon;
	uint8_t		byteHealth;
	uint8_t		byteArmor;
	uint16_t	sLeftRightKeys;
	uint16_t	sUpDownKeys;
	union
	{
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float		fPosition[3];
};

struct stDamageData
{
#pragma pack( 1 )
	uint16_t	sVehicleID_lastDamageProcessed;
	int			iBumperDamage;
	int			iDoorDamage;
	uint8_t		byteLightDamage;
	uint8_t		byteWheelDamage;
};

struct stSurfData
{
#pragma pack( 1 )
	int			iIsSurfing;
	float		fSurfPosition[3];
	uint16_t	sSurfingVehicleID;
	uint32_t	ulSurfTick;
	int			iSurfMode;	// 0 = not surfing, 1 = moving (unstable surf), 2 = fixed on vehicle
};

struct stHeadSync
{
#pragma pack( 1 )
	float	fHeadSync[3];
	int		iHeadSyncUpdateTick;
	int		iHeadSyncLookTick;
};

struct stLocalPlayer
{
#pragma pack( 1 )
	struct stOnFootData		onFootData;
	struct stInCarData		inCarData;
	struct stPassengerData	passengerData;
	struct stAimData		aimData;
	struct stTrailerData	trailerData;
	struct stSAMPPed		*pSAMP_Actor;
	uint16_t				sCurrentVehicleID;
	uint16_t				sLastVehicleID;
	int						iIsActive;
	int						iIsWasted;
	int						iCurrentAnimID;
	uint8_t					unknown_2[133];
	int						iSpawnSkin;
	uint8_t					unknown_x;
	float					fSpawnPos[3];
	float					fSpawnRot;
	int						iSpawnWeapon[3];
	int						iSpawnAmmo[3];
	int						iIsActorAlive;
	int						iSpawnClassLoaded;
	uint32_t				ulSpawnSelectionTick;
	uint32_t				ulSpawnSelectionStart;
	int						iIsSpectating;
	uint8_t					byteTeamID;
	uint8_t					unknown_5[2];
	uint32_t				ulSendTick;
	uint32_t				ulSpectateTick;
	uint32_t				ulAimTick;
	uint32_t				ulStatsUpdateTick;
	uint32_t				ulWeapUpdateTick;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteWeaponInventory[13];
	int						iWeaponAmmo[13];
	struct stHeadSync		headSyncData;
	uint32_t				ulUnoccupiedTick;
	uint32_t				ulUnknownTick;
	int						iPassengerDriveBy;
	uint8_t					byteCurrentInterior;
	uint8_t					byteIsInRCVehicle;
	uint8_t					unknown_7[3];
	struct stSurfData		surfData;
	uint8_t					byteSpectatingUnk;	//15[?],14[?],(3-Veh/4-Player [follow])
	uint8_t					byteSpectatingWHAT; //2 = Veh, 1 = Player
	int						iSpectateID;
	int						iInitiatedSpectating;
	int						iClassSelectionOnDeath;
	int						iSpawnClassID;
	int						iRequestToSpawn;
	int						iIsInSpawnScreen;
	uint32_t				ulZoneNameTick;
	struct stDamageData		vehicleDamageData;
};

struct stRemotePlayerData
{
#pragma pack( 1 )
	struct stOnFootData		onFootData;
	struct stInCarData		inCarData;
	struct stPassengerData	passengerData;
	struct stAimData		aimData;
	struct stTrailerData	trailerData;
	uint8_t					unk[16];
	float					fOnFootPos[3];
	float					fOnFootMoveSpeed[3];
	float					fVehiclePosition[3];
	float					fVehicleMoveSpeed[3];
	float					fVehicleRoll[4];
	uint8_t					unk_2[12];
	struct stSAMPPed		*pSAMP_Actor;
	struct stSAMPVehicle	*pSAMP_Vehicle;
	uint8_t					unk2[4];
	uint16_t				sPlayerID;
	uint8_t					unk3[5];
	uint8_t					bytePlayerState;
	uint16_t				sVehicleID;
	uint8_t					byteSeatID;
	uint8_t					byteTeamID;
	int						iPassengerDriveBy;
	struct stHeadSync		headSyncData;
	uint8_t					unk5[4];
	int						iShowNameTag;
	int						iHasJetPack;
	uint8_t					byteSpecialAction;
	float					fActorHealth;
	float					fActorArmor;
	uint8_t					unk6[12];
	int						iGlobalMarkerLoaded;
	int						iGlobalMarkerLocation[3];
	uint32_t				ulGlobalMarker_GTAID;
};

struct stRemotePlayer
{
#pragma pack( 1 )
	int					iIsNPC;
	int					iPing;
	stRemotePlayerData	*pPlayerData;
	int					iScore;
	uint8_t				unknown[4];
	union
	{
		char	szPlayerName[20];
		char	*pszPlayerName;
	};
	int iStrlenName;
};

struct stSAMPPed
{
#pragma pack( 1 )
	void				*pVTBL_sampPed;
	struct actor_info	*pGTA_Ped_;
	uint32_t			ulGTA_Ped_ID;
	int					usingCellPhone;
	struct actor_info	*pGTA_Ped;
	uint8_t				byteUnknown[41];
	int					DrinkingOrSmoking;
	int					object_in_hand;
	int					drunkLevel;	
	int					isUrinating;	
	uint32_t			ulGTA_UrinateParticle_ID;
	uint8_t				byteUnknown_3[5];
	int					isDancing;
	int					danceStyle;
	int					danceMove;
};

struct stVehiclePool
{
#pragma pack( 1 )
	uint8_t					unknown[4400+2*SAMP_VEHICLE_MAX+24120];
	struct vehicle_info		*pGTA_Vehicle[SAMP_VEHICLE_MAX];
	struct stSAMPVehicle	*pSAMP_Vehicle[SAMP_VEHICLE_MAX];
	int						iIsListed[SAMP_VEHICLE_MAX];
};

struct stSAMPVehicle
{
#pragma pack( 1 )
	void				*pVTBL_Vehicle;
	struct vehicle_info *pGTA_Vehicle__probWrong;
	uint32_t			ulGTA_Vehicle_ID;
	uint8_t				unknowns[4];
	struct vehicle_info *pGTA_Vehicle;
	uint8_t				unknow[8];
	int					isLocked;
};

struct stObject
{
#pragma pack( 1 )
	void				*pVTBL_Object;
	struct object_info	*pGTAObject;
	uint32_t			ulGTA_Ped_ID;
};

struct stObjectPool
{
#pragma pack( 1 )
	int				unknown;
	int				iIsListed[SAMP_OBJECTS_MAX];
	struct stObject *object[SAMP_OBJECTS_MAX];
};

struct stTextLabel
{
#pragma pack( 1 )
	char		*pText;
	DWORD		color;
	float		fPosition[3];
	float		fMaxViewDistance;
	uint8_t		byteShowBehindWalls;
	uint16_t	sAttachedToPlayerID;
	uint16_t	sAttachedToVehicleID;
};

struct stTextLabelPool
{
#pragma pack( 1 )
	struct stTextLabel	textLabel[MAX_3DTEXT];
	int					iIsListed[MAX_3DTEXT];
};

struct stChatEntry
{
#pragma pack( 1 )
	int			iType;			// 10 - text + prefix, 11 - text (server msg), 12 - text (debug)
	D3DCOLOR	clTextColor;
	D3DCOLOR	clPrefixColor;	// or textOnly colour
	uint32_t	SystemTime;
	char		szPrefix[28];
	char		szText[208];
};
struct stChatInfo
{
#pragma pack( 1 )
	uint8_t				unknown_0[24];
	int					iChatWindowMode;			// 2 - normal, 1 - light, 0 - disabled
	uint8_t				unk;
	uint8_t				bTimestamps;
	uint8_t				unknown[3];
	char				logFilePathChatLog[261];
	struct stChatEntry	chatEntry[101];
	uint8_t				unknown_2[4];
	D3DCOLOR			clTextColor;
	D3DCOLOR			clDebugColor;
	D3DCOLOR			clInfoColor;
	int					pagesize;
};

typedef void ( __cdecl *CMDPROC ) ( PCHAR );
struct stInputInfo
{
#pragma pack( 1 )
	void	*pD3DDevice;
	void	*pDXUTDialog;
	void	*pDXUTEditBox;
	CMDPROC pszCMDs[MAX_CLIENTCMDS];
	char	szCMDNames[MAX_CLIENTCMDS][33];
	int		iCMDCount;
	int		iInputEnabled;
	char	szInputBuffer[129];
	char	szRecallBufffer[10][129];
	char	szCurrentBuffer[129];
	int		iCurrentRecall;
	int		iTotalRecalls;
	CMDPROC pszDefaultCMD;
};

struct stKillEntry
{
#pragma pack( 1 )
	char		szKiller[25];
	char		szVictim[25];
	D3DCOLOR	clKillerColor;
	D3DCOLOR	clVictimColor;
	uint8_t		byteType;
};

struct stKillInfo
{
#pragma pack( 1 )
	int					iEnabled;
	struct stKillEntry	killEntry[5];
};

struct stStreamedOutPlayerInfo
{
	int		iPlayerID[SAMP_PLAYER_MAX];
	float	fPlayerPos[SAMP_PLAYER_MAX][3];
};

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// SUPPORT VARIABLES //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
struct stTranslateGTASAMP_vehiclePool
{
#pragma pack( 1 )
	// the index is the position in the vehicle pool
	//
	// translation data
	int iSAMPID[SAMP_VEHICLE_MAX];
};
struct stTranslateGTASAMP_pedPool
{
#pragma pack( 1 )
	// the index is the position in the vehicle pool
	//
	// translation data
	int iSAMPID[SAMP_PLAYER_MAX];
};

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// FUNCTIONS //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void											update_translateGTASAMP_vehiclePool ( void );
void											update_translateGTASAMP_pedPool ( void );

#ifdef M0D_DEV
extern int										iDebugVehicle;
void											cmd_debug_vehicle ( char *param );
#endif

bool											findstrinstr ( char *text, char *find );

int												isBadPtr_SAMP_iVehicleID ( int iVehicleID );
int												isBadPtr_SAMP_iPlayerID ( int iPlayerID );

void											getSamp ();
uint32_t										getSampAddress ();

struct stSAMP									*stGetSampInfo ( void );
struct stChatInfo								*stGetSampChatInfo ( void );
struct stInputInfo								*stGetInputInfo ( void );
struct stKillInfo								*stGetKillInfo ( void );
D3DCOLOR										samp_color_get ( int id );
D3DCOLOR										samp_color_get_trans ( int id, DWORD trans );

void											sampMainCheat ();

int												getNthPlayerID ( int n );
int												getPlayerCount ( void );
int												getVehicleCount ( void );

int												setLocalPlayerName ( const char *name );
int												getPlayerPos ( int iPlayerID, float fPos[3] );
const char										*getPlayerName ( int iPlayerID );
int												getPlayerState ( int iPlayerID );
int												getPlayerVehicleGTAScriptingID ( int iPlayerID );
int												getPlayerSAMPVehicleID ( int iPlayerID );
void											spectatePlayer ( int iPlayerID );

struct actor_info								*getGTAPedFromSAMPPlayerID ( int iPlayerID );
struct vehicle_info								*getGTAVehicleFromSAMPVehicleID ( int iPlayerID );
int												getSAMPPlayerIDFromGTAPed ( struct actor_info *info );
int												getSAMPVehicleIDFromGTAVehicle ( struct vehicle_info *pVehicle );
uint32_t										getPedGTAScriptingIDFromPlayerID ( int iPlayerID );
uint32_t										getVehicleGTAScriptingIDFromVehicleID ( int iVehicleID );

int												samp_vehicle_find_nearest ( int flags );

bool											get_isModCommandsActive ();
void											init_samp_chat_cmds ();
void											addClientCommand ( char *text, int function );
void											addToChatWindow ( char *text, D3DCOLOR textColor );
void											addMessageToChatWindow ( const char *text, ... );
void											addMessageToChatWindowSS ( const char *text, ... );
void											restartGame ();
void											addSayToChatWindow ( char *msg );
void											say ( char *text, ... );
void											showGameText ( char *text, int time, int textsize );
void											playerSpawn ( void );
void											disconnect ( int reason );
void											setPassword ( char *password );
void											sendSetInterior ( uint8_t interiorID );
void											setSpecialAction ( uint8_t byteSpecialAction );
void											sendSCMEvent ( int iEvent, int iVehicleID, int iParam1, int iParam2 );

void											installSAMPHooks ();
void											setSAMPInitScreenMatrix ( float camposX, float camposY, float camposZ,
																		  float camlookatX, float camlookatY,
																		  float camlookatZ );
void											setSAMPCustomSendRates ( int iOnFoot, int iInCar, int iAim, int iHeadSync );
int												sampPatchDisableNameTags ( int iEnabled );
int												sampPatchDisableInteriorUpdate ( int iEnabled );
int												sampPatchDisableScreeenshotKey ( int iEnabled );
int												sampPatchDisableScoreboardToggleOn ( int iEnabled );

// global pointer externals
extern int										g_renderSAMP_initSAMPstructs;
extern struct stSAMP							*g_SAMP;
extern struct stPlayerPool						*g_Players;
extern struct stVehiclePool						*g_Vehicles;
extern struct stChatInfo						*g_Chat;
extern struct stInputInfo						*g_Input;
extern struct stKillInfo						*g_DeathList;
extern struct stStreamedOutPlayerInfo			g_stStreamedOutInfo;

// global support variables externals
extern struct stTranslateGTASAMP_vehiclePool	translateGTASAMP_vehiclePool;
extern struct stTranslateGTASAMP_pedPool		translateGTASAMP_pedPool;

extern int										g_Spectate_playerID;
