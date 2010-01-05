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
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/cheat.h $
	$Id: cheat.h 43 2010-01-05 07:40:59Z futnucks $

*/

/* only works for vehicles atm */
//#define TELEPORT_DETACHABLES


#define MSEC_TO_TIME(v)    ((uint32_t)(v) * 10)
#define TIME_TO_FLOAT(v)   ((float)((double)(v) / (double)MSEC_TO_TIME(1000)))

// main pool addresses
#define ACTOR_POOL_POINTER    0x00B74490
#define ACTOR_POINTER_SELF    0x00B7CD98
#define ACTOR_SELF            -1
#define VEHICLE_POOL_POINTER  0x00B74494
#define VEHICLE_POINTER_SELF  0x00B6F980
#define VEHICLE_SELF          -1

// CPed actor states
#define ACTOR_STATE_DRIVING   50
#define ACTOR_STATE_DYING     54
#define ACTOR_STATE_DEAD      55

#define ACTOR_MSTATE_STILL    1
#define ACTOR_MSTATE_RUN      6
#define ACTOR_MSTATE_RUN_FAST 7

// predefined vehicle & actor invulnerability flags
#define VEHICLE_FLAGS_INVULNERABLE  (4 | 8 | 16 | 128)
#define ACTOR_FLAGS_INVULNERABLE    (4 | 8 | 64 | 128)

// old vehicle_info vehicle types
#define VEHICLE_TYPE_CAR   0  /* + helicopters and planes */
#define VEHICLE_TYPE_BOAT  5
#define VEHICLE_TYPE_TRAIN 6
#define VEHICLE_TYPE_BIKE  9

// new vehicle types
enum eClientVehicleType
{
    CLIENTVEHICLE_NONE,
    CLIENTVEHICLE_CAR,
    CLIENTVEHICLE_BOAT,
    CLIENTVEHICLE_TRAIN,
    CLIENTVEHICLE_HELI,
    CLIENTVEHICLE_PLANE,
    CLIENTVEHICLE_BIKE,
    CLIENTVEHICLE_MONSTERTRUCK,
    CLIENTVEHICLE_QUADBIKE,
    CLIENTVEHICLE_BMX,
    CLIENTVEHICLE_TRAILER
};

// types of vehicle mod upgrades
enum eVehicleUpgradePosn
{
    VEHICLE_UPGRADE_POSN_BONET = 0,
    VEHICLE_UPGRADE_POSN_VENT,
    VEHICLE_UPGRADE_POSN_SPOILER,
    VEHICLE_UPGRADE_POSN_SIDESKIRT,
    VEHICLE_UPGRADE_POSN_FRONT_BULLBARS,
    VEHICLE_UPGRADE_POSN_REAR_BULLBARS,
    VEHICLE_UPGRADE_POSN_HEADLIGHTS,
    VEHICLE_UPGRADE_POSN_ROOF,
    VEHICLE_UPGRADE_POSN_NITRO,
    VEHICLE_UPGRADE_POSN_HYDRAULICS,
    VEHICLE_UPGRADE_POSN_STEREO,
    VEHICLE_UPGRADE_POSN_UNKNOWN,
    VEHICLE_UPGRADE_POSN_WHEELS,
    VEHICLE_UPGRADE_POSN_EXHAUST,
    VEHICLE_UPGRADE_POSN_FRONT_BUMPER,
    VEHICLE_UPGRADE_POSN_REAR_BUMPER,
    VEHICLE_UPGRADE_POSN_MISC,
};

// upgrade model ids
#define VEHICLEUPGRADE_NITRO_5X 1008
#define VEHICLEUPGRADE_NITRO_2X 1009
#define VEHICLEUPGRADE_NITRO_10X 1010
#define VEHICLEUPGRADE_HYDRAULICS 1087

// maximum passengers including driver
#define VEHICLE_MAX_PASSENGERS 9

// main cheat states
#define CHEAT_STATE_NONE      0
#define CHEAT_STATE_ACTOR     1
#define CHEAT_STATE_VEHICLE   2

// teleport pool sizes
#define TELEPORT_MAX          10
#define TELEPORT_HIST_MAX     10
#define STATIC_TELEPORT_MAX   200


/////////////////////////////////////
// RenderWare Variables & Pointers //
/////////////////////////////////////
#define		__rwD3D9CPUSupportsMMX		0xC980A0 // int
#define		__rwD3D9CPUSupportsSSE		0xC980A4 // int
#define		__rwD3D9CPUSupportsSSE2		0xC980A8 // int
#define		__rwD3D9CPUSupports3DNow	0xC980AC // int
#define		__RwD3DDevice				0xC97C28 // IDirect3DDevice9*

/*
#define		__RwD3D9D3D9ViewTransform	0xC9BC80 // D3DMATRIX
#define		__RwD3D9D3D9ProjTransform	0x8E2458 // VECTOR
#define		__RwD3D9ActiveViewProjTransform 0xC97C64 // D3DMATRIX*
#define		_D3D9Transform				0x8D7390 // D3DMATRIX
#define		_D3D9ActiveTransform		0xC94C70 // D3DMATRIX*
#define		_D3D9ActiveInverseTransform	0xC94C74 // D3DMATRIX*
#define		_D3D9ActiveInverseNormalizedTransform 0xC94C78 // D3DMATRIX*
#define		_D3D9ActiveNormalizedTransform 0xC94C7C // D3DMATRIX*
*/


//////////////////////////////
// GTA Variables & Pointers //
//////////////////////////////
#define		GTAvar__windowsVersion		0xC8CF68 // int or VersionInformation struct
#define		GTAvar__D3D9UseVersion		0xC94C00 // int
#define		GTAvar__DXVersion			0xC8CF6C // int
#define		GTAvar__gameState			0xC8D4C0 // int
#define		GTAvar_osPlatform			0xC9AC08 // int
#define		GTAvar_osVer				0xC9AC0C // int
#define		GTAvar_winVer				0xC9AC10 // int
#define		GTAvar_winMajor				0xC9AC14 // int




///////////////////////////////////////////////////////

struct cheat_state_actor
{
   float coords[3];
   int   invulnerable;
   int   air_brake;
   int   air_brake_slowmo;
   int   stick;
   int   autoaim;
   int   hp_regen_on;
};

struct cheat_state_vehicle
{
   float	coords[3];
   int		invulnerable;
   int		air_brake;
   int		air_brake_slowmo;
   int		stick;
   int		protection;
   int		hp_tire_support;
   int		hp_minimum_on;
   int		hp_damage_reduce_on;
   int		hp_regen_on;
   int		brkdance;
   int		is_engine_on;
   int		infNOS_toggle_on;
};

struct cheat_state_teleport
{
   int   set;
   float matrix[16];
   int   interior_id;
};

struct cheat_state_generic
{
   int	hp_cheat;
   int	money;
   int	weapon;
   int	map;
   int	menu;
   int	spoof_weapon;        /* set to the weapon type id */
   int	vehicles_unlock;
   int	vehicles_warp_invert;
   int	vehicles_freeze;
   int	weapon_disable;

   int	onfoot_data;
   int	aim_data;
   int	vehicle_data;
   int	passenger_data;
   int	statsupd;
   int	spec_data;

   int	teletext;

   DWORD rejoinTick;
   DWORD join_serverTick;

   int pickuptexts;
   int objecttexts;

   bool car_jacked;
   DWORD anti_carjackTick;
   short car_jacked_last_vehicle_id;
   bool got_vehicle_id;
   float car_jacked_lastPos[3];

   int interior_patch_enabled;

   int nocols_enabled;
};


#define DEBUG_DATA_SIZE 320
#define DEBUG_HIST_LEN  100

struct debug_info
{
   int      data_type;  /* 0 byte, 1 word, 2 dword, 3 float */

   int      hist_pos;
   uint8_t  *ptr[DEBUG_HIST_LEN];
   int      offset[DEBUG_HIST_LEN];

   uint8_t  data[DEBUG_DATA_SIZE];
   uint8_t  data_prev[DEBUG_DATA_SIZE];
   uint32_t modify_time[DEBUG_DATA_SIZE];

   int      data_prev_clear;

   uint8_t  cursor_data[sizeof(void *)];
   char     ptr_hist_str[64];
};

struct cheat_state
{
   int   state;         /* CHEAT_STATE_* */
   struct cheat_state_actor   actor;
   struct cheat_state_vehicle vehicle;
   struct cheat_state_generic _generic;
   struct cheat_state_teleport teleport[TELEPORT_MAX];
   struct cheat_state_teleport teleport_hist[TELEPORT_HIST_MAX];
   int teleport_hist_count;
   int   debug_enabled;
   struct debug_info debug;
   float game_speed;
   int   text_time;
   char  text[128];
   int player_info_list;
   int render_player_tags;
   int render_vehicle_tags;
};

struct pool
{
   #pragma pack(1)
   void     *start;     /* pointer to start of pool */
   void     *__end;     /* end of this structure? (not the pool) */
   int32_t  size;       /* max number of items in the pool */
   /* there's some more junk after this */
};

struct detachable
{
   #pragma pack(1)
   float    position[3];
   uint8_t  __unknown_12[4];
   float    vector[3];
   uint8_t  __unknown_28[16];
};

struct animation_aim
{
   #pragma pack(1)
   uint8_t  __unknown_0[8];
   float    *matrix;
};

struct animation
{
   #pragma pack(1)
   uint8_t              __unknown_0[44];
   struct animation_aim *animation_aim;
};

struct weapon
{
   #pragma pack(1)
   uint32_t id;
   uint32_t state;
   int32_t  ammo_clip;
   int32_t  ammo;
   uint32_t __unknown_count;
   uint32_t __goggle;  /* 0 off, 256 on */
   uint32_t __unknown;
};

typedef struct _VECTOR
{
	#pragma pack(1)
	float X,Y,Z;
} VECTOR, *PVECTOR;

typedef struct _MATRIX4X4
{
	#pragma pack(1)
	VECTOR right;
	DWORD  flags;
	VECTOR up;
	float  pad_u;
	VECTOR at;
	float  pad_a;
	VECTOR pos;
	float  pad_p;
} MATRIX4X4, *PMATRIX4X4;

struct object_base
{
	#pragma pack(1)
	UINT_PTR	vmt;				// 0
	float		coords[3];			// 4
	union {
		float		*preMatrix;			// 16, a part of CPlaceable
		MATRIX4X4	*preMatrixStruct;	// 16, a part of CPlaceable
	};
	union {
		float		*matrix;			// 20
		MATRIX4X4	*matrixStruct;		// 20
	};
	UINT_PTR	*ptrRenderWare;		// 24

	/********** BEGIN CFLAGS **************/
	unsigned long bUsesCollision : 1;			// does entity use collision
	unsigned long bCollisionProcessed : 1; 	// has object been processed by a ProcessEntityCollision function
	unsigned long bIsStatic : 1;				// is entity static
	unsigned long bHasContacted : 1;			// has entity processed some contact forces
	unsigned long bIsStuck : 1;				// is entity stuck
	unsigned long bIsInSafePosition : 1;		// is entity in a collision free safe position
	unsigned long bWasPostponed : 1;			// was entity control processing postponed
	unsigned long bIsVisible : 1;				//is the entity visible
	
	unsigned long bIsBIGBuilding : 1;			// Set if this entity is a big building
	unsigned long bRenderDamaged : 1;			// use damaged LOD models for objects with applicable damage
	unsigned long bStreamingDontDelete : 1;	// Dont let the streaming remove this 
	unsigned long bRemoveFromWorld : 1;		// remove this entity next time it should be processed
	unsigned long bHasHitWall : 1;				// has collided with a building (changes subsequent collisions)
	unsigned long bImBeingRendered : 1;		// don't delete me because I'm being rendered
	unsigned long bDrawLast :1;				// draw object last
	unsigned long bDistanceFade :1;			// Fade entity because it is far away
	
	unsigned long bDontCastShadowsOn : 1;		// Dont cast shadows on this object
	unsigned long bOffscreen : 1;				// offscreen flag. This can only be trusted when it is set to true
	unsigned long bIsStaticWaitingForCollision : 1; // this is used by script created entities - they are static until the collision is loaded below them
	unsigned long bDontStream : 1;				// tell the streaming not to stream me
	unsigned long bUnderwater : 1;				// this object is underwater change drawing order
	unsigned long bHasPreRenderEffects : 1;	// Object has a prerender effects attached to it
	unsigned long bIsTempBuilding : 1;			// whether or not the building is temporary (i.e. can be created and deleted more than once)
	unsigned long bDontUpdateHierarchy : 1;	// Don't update the aniamtion hierarchy this frame
	
	unsigned long bHasRoadsignText : 1;		// entity is roadsign and has some 2deffect text stuff to be rendered
	unsigned long bDisplayedSuperLowLOD : 1;
	unsigned long bIsProcObject : 1;			// set object has been generate by procedural object generator
	unsigned long bBackfaceCulled : 1;			// has backface culling on
	unsigned long bLightObject : 1;			// light object with directional lights
	unsigned long bUnimportantStream : 1;		// set that this object is unimportant, if streaming is having problems
	unsigned long bTunnel : 1;			// Is this model part of a tunnel
	unsigned long bTunnelTransition : 1;		// This model should be rendered from within and outside of the tunnel
	/********** END CFLAGS **************/

	uint8_t		wSeedColFlags;			// 32
	uint8_t		wSeedVisibleFlags;		// 33
	uint16_t	model_alt_id;			// 34 - model index
    uint8_t     __unknown_36[4];		// 36
//CReferences *pReferences; //36
	uint32_t	*m_pLastRenderedLink;	// 40 CLink<CEntity*>* m_pLastRenderedLink;
    uint16_t    timer;					// 44
	uint8_t		m_iplIndex;				// 46 used to define which IPL file object is in
    uint8_t     interior_id;			// 47
    uint8_t		__unknown_48[6];		// 48
	//********* BEGIN CEntityInfo **********//
	uint8_t		nType : 3;				// 54 what type is the entity (2 == Vehicle)
	uint8_t		nStatus : 5;			// 54 control status
										// 55 alignment
	//********* END CEntityInfo **********//
    uint8_t		__unknown_56[9];		// 56
	uint8_t		nImmunities;			// 65
	uint8_t		__unknown_66;			// 66
};

struct actor_info
{
   #pragma pack(1)
   struct object_base base;				  /* 65 */
   uint8_t           flags;               /* 66 immunities */
   uint8_t           __unknown_67[1];     /* 67 */
   float             speed[3];            /* 68 */
   float             spin[3];             /* 80 */
   uint8_t           __unknown_92[120];   /* 92 */
   float             speed_z;             /* 212 */
   uint8_t           __unknown_216[132];  /* 216 */
   uint8_t			 animation_state;	  /* 348 */
   uint8_t			 __unknown_349[7];	  /* 349 */
   float             step_pos[3];         /* 356 - coordinates, last foot step */
   uint8_t           __unknown_368[764];  /* 368 */
   uint8_t           player_check;        /* 1132 */
   uint8_t           jump_state;		  /* 1133 */
   uint8_t           __unknown_1134[2];   /* 1134 */
   uint32_t			 invuln_flags;		  /* 1136 */
   uint32_t			 __unk_flags;		  /* 1140 */
   uint8_t           __unknown_1144[4];   /* 1144 */
   struct animation  *animation;          /* 1148 */
   uint8_t           __unknown_1152[92];  /* 1152 */
   float             runspeed;            /* 1244 */
   uint8_t           __unknown_1248[36];  /* 1248 */
   uint16_t			 muzzle_flash;        /* 1284 */
   uint8_t			 __unknown_1286[14];  /* 1286 */
   float             x_angle;             /* 1300 - x_angle / z_angle */
   uint8_t           __unknown_1304[24];  /* 1304 */
   uint32_t          state;               /* 1328 - ACTOR_STATE_* */
   uint32_t          mstate;              /* 1332 - ACTOR_MSTATE_* */
   uint8_t           __unknown_1336[8];   /* 1336 */
   float             hitpoints;           /* 1344 */
   float             hitpoints_max;       /* 1348 - hmm, does not seem to be right.. it's set to "100.1318519" or something like that */
   float             armor;               /* 1352 */
   uint8_t           __unknown_1356[12];  /* 1356 */
   float             z_angle;             /* 1368 */
   float             z_angle2;            /* 1372 - same value as z_angle */
   uint8_t           __unknown_1376[8];   /* 1376 */
   struct vehicle_info *vehicle_contact;  /* 1384 */
   uint8_t           __unknown_1388[24];  /* 1388 */
   void              *building_contact;   /* 1412 */
   uint8_t           __unknown_1392[4];   /* 1416 */
   struct vehicle_info *vehicle;          /* 1420 */
   uint8_t           __unknown_1424[8];   /* 1424 */
   uint8_t			 actor_lock;		  /* 1432 */
   uint8_t			 __unknown_1433[7];   /* 1433 */
   struct weapon     weapon[13];          /* 1440 */
   uint8_t           __unknown_1804[12];  /* 1804 */
   uint8_t           weapon_slot;         /* 1816 */
   uint8_t           __unknown_1817[3];   /* 1817 */
   void              *ptr;                /* 1820 */
   uint8_t           __unknown_1824[32];  /* 1824 */
   uint32_t          weapon_model;        /* 1856 */
   uint8_t           __unknown_1860[28];  /* 1860 */
   uint32_t          weapon_hit_type;     /* 1888 - last hit by this weapon */
   struct actor_info *weapon_hit_by;      /* 1892 - last hit by this object (usually an actor) */
   uint8_t           __unknown_1889[92];  /* 1896 */
};                                        /* 1988 */

struct CVehicleFlags
{
	// byte 1
	unsigned char bIsLawEnforcer: 1; // Is this guy chasing the player at the moment
	unsigned char bIsAmbulanceOnDuty: 1; // Ambulance trying to get to an accident
	unsigned char bIsFireTruckOnDuty: 1; // Firetruck trying to get to a fire
	unsigned char bIsLocked: 1; // Is this guy locked by the script (cannot be removed)
	unsigned char bEngineOn: 1; // For sound purposes. Parked cars have their engines switched off (so do destroyed cars)
	unsigned char bIsHandbrakeOn: 1; // How's the handbrake doing ?
	unsigned char bLightsOn: 1; // Are the lights switched on ?
	unsigned char bFreebies: 1; // Any freebies left in this vehicle ?
	// byte 2
	unsigned char bIsVan: 1; // Is this vehicle a van (doors at back of vehicle)
	unsigned char bIsBus: 1; // Is this vehicle a bus
	unsigned char bIsBig: 1; // Is this vehicle a bus
	unsigned char bLowVehicle: 1; // Need this for sporty type cars to use low getting-in/out anims
	unsigned char bComedyControls: 1; // Will make the car hard to control (hopefully in a funny way)
	unsigned char bWarnedPeds: 1; // Has scan and warn peds of danger been processed?
	unsigned char bCraneMessageDone: 1; // A crane message has been printed for this car allready
	unsigned char bTakeLessDamage: 1; // This vehicle is stronger (takes about 1/4 of damage)
	// byte 3
	unsigned char bIsDamaged: 1; // This vehicle has been damaged and is displaying all its components
	unsigned char bHasBeenOwnedByPlayer : 1;// To work out whether stealing it is a crime
	unsigned char bFadeOut: 1; // Fade vehicle out
	unsigned char bIsBeingCarJacked: 1; // Fade vehicle out
	unsigned char bCreateRoadBlockPeds : 1;// If this vehicle gets close enough we will create peds (coppers or gang members) round it
	unsigned char bCanBeDamaged: 1; // Set to FALSE during cut scenes to avoid explosions
	unsigned char bOccupantsHaveBeenGenerated : 1; // Is true if the occupants have already been generated. (Shouldn't happen again)
	unsigned char bGunSwitchedOff: 1; // Level designers can use this to switch off guns on boats
	// byte 4
	unsigned char bVehicleColProcessed : 1;// Has ProcessEntityCollision been processed for this car?
	unsigned char bIsCarParkVehicle: 1; // Car has been created using the special CAR_PARK script command
	unsigned char bHasAlreadyBeenRecorded : 1; // Used for replays
	unsigned char bPartOfConvoy: 1;
	unsigned char bHeliMinimumTilt: 1; // This heli should have almost no tilt really
	unsigned char bAudioChangingGear: 1; // sounds like vehicle is changing gear
	unsigned char bIsDrowning: 1; // is vehicle occupants taking damage in water (i.e. vehicle is dead in water)
	unsigned char bTyresDontBurst: 1; // If this is set the tyres are invincible
	// byte 5
	unsigned char bCreatedAsPoliceVehicle : 1; // True if this guy was created as a police vehicle (enforcer, policecar, miamivice car etc)
	unsigned char bRestingOnPhysical: 1; // Dont go static cause car is sitting on a physical object that might get removed
	unsigned char bParking: 1;
	unsigned char bCanPark: 1;
	unsigned char bFireGun: 1; // Does the ai of this vehicle want to fire it's gun?
	unsigned char bDriverLastFrame: 1; // Was there a driver present last frame ?
	unsigned char bNeverUseSmallerRemovalRange: 1;// Some vehicles (like planes) we don't want to remove just behind the camera.
	unsigned char bIsRCVehicle: 1; // Is this a remote controlled (small) vehicle. True whether the player or AI controls it.
	// byte 6
	unsigned char bAlwaysSkidMarks: 1; // This vehicle leaves skidmarks regardless of the wheels' states.
	unsigned char bEngineBroken: 1; // Engine doesn't work. Player can get in but the vehicle won't drive
	unsigned char bVehicleCanBeTargetted : 1;// The ped driving this vehicle can be targetted, (for Torenos plane mission)
	unsigned char bPartOfAttackWave: 1; // This car is used in an attack during a gang war
	unsigned char bWinchCanPickMeUp: 1; // This car cannot be picked up by any ropes.
	unsigned char bImpounded: 1; // Has this vehicle been in a police impounding garage
	unsigned char bVehicleCanBeTargettedByHS  : 1;// Heat seeking missiles will not target this vehicle.
	unsigned char bSirenOrAlarm: 1; // Set to TRUE if siren or alarm active, else FALSE
	// byte 7
	unsigned char bHasGangLeaningOn: 1;
	unsigned char bGangMembersForRoadBlock : 1;// Will generate gang members if NumPedsForRoadBlock > 0
	unsigned char bDoesProvideCover: 1; // If this is false this particular vehicle can not be used to take cover behind.
	unsigned char bMadDriver: 1; // This vehicle is driving like a lunatic
	unsigned char bUpgradedStereo: 1; // This vehicle has an upgraded stereo
	unsigned char bConsideredByPlayer: 1; // This vehicle is considered by the player to enter
	unsigned char bPetrolTankIsWeakPoint : 1;// If false shootong the petrol tank will NOT Blow up the car
	unsigned char bDisableParticles: 1; // Disable particles from this car. Used in garage.
	// byte 8
	unsigned char bHasBeenResprayed: 1; // Has been resprayed in a respray garage. Reset after it has been checked.
	unsigned char bUseCarCheats: 1; // If this is true will set the car cheat stuff up in ProcessControl()
	unsigned char bDontSetColourWhenRemapping : 1;// If the texture gets remapped we don't want to change the colour with it.
	unsigned char bUsedForReplay: 1; // This car is controlled by replay and should be removed when replay is done.
};

class CTransmissionSAInterface
{
public:
    float           fUnknown  [18];                 // +44
    unsigned char   ucDriveType         :8;         // +116
    unsigned char   ucEngineType        :8;         // +117
    unsigned char   ucNumberOfGears     :8;         // +118
    unsigned char   ucUnknown           :8;         // +119
    unsigned int    uiHandlingFlags;                // +120
    float           fEngineAccelleration;           // +124     (value in handling.cfg * 0x86A950)
    float           fEngineInertia;                 // +128
    float           fMaxVelocity;                   // +132
    float           fUnknown2 [3];                  // +136
};

struct tHandlingDataSA
{
	uint8_t			__unknown_0[4];					// +0
	float           fMass;                          // +4
	float			__unknown_8;					// +8	Automatically calculated
	float           fTurnMass;                      // +12
	float           fDragCoeff;                     // +16
	VECTOR			vecCenterOfMass;				// +20
	unsigned int    uiPercentSubmerged;             // +32
	float			__unknown_36;					// +36	Automatically calculated
	float           fTractionMultiplier;            // +40
	//uint8_t			transmission[104];				// +44
	CTransmissionSAInterface Transmission;			// +44
	float           fBrakeDecelleration;            // +148
	float           fBrakeBias;                     // +152
	bool            bABS;                           // +156
	float           fSteeringLock;                  // +160
	float           fTractionLoss;                  // +164
	float           fTractionBias;                  // +168
	float           fSuspensionForceLevel;          // +172
	float           fSuspensionDamping;             // +176
	float           fSuspensionHighSpdDamping;      // +180
	float           fSuspensionUpperLimit;          // +184
	float           fSuspensionLowerLimit;          // +188
	float           fSuspensionFrontRearBias;       // +192
	float           fSuspensionAntidiveMultiplier;  // +196
	float           fCollisionDamageMultiplier;     // +200
	unsigned int    uiModelFlags;                   // +204
	unsigned int    uiHandlingFlags;                // +208
	float           fSeatOffsetDistance;            // +212
	unsigned int    uiMonetary;						// +216
	unsigned char   ucHeadLight: 8;					// +220
	unsigned char   ucTailLight: 8;					// +221
	unsigned char   ucAnimGroup: 8;					// +222
	unsigned char   ucUnused: 8;					// +223
};

struct vehicle_info
{
   #pragma pack(1)
   struct object_base base;
   uint8_t             flags;                     /* 66 - flags */
   uint8_t             __unknown_67;              /* 67 */
   float               speed[3];                  /* 68 */
   float               spin[3];                   /* 80 */
   uint8_t             __unknown_92[44];          /* 92 */

	// handling should start here
	uint8_t				__unknown_136[4];			/* 136 */
   float               mass;                      /* 140 - vehicle mass from handling.cfg */
   float               turn_mass;                 /* 144 - turn mass */
   float               grip_div;                  /* 148 - grip divisor */
   float               mass_to_grip_mult;         /* 152 - mass to grip multiplier */
	float				fTurnMass;					/* 156 - 0.05 or 0.1 */
   float               grip_level_norm;           /* 160 - normalized grip level */
   float               center_of_mass[3];         /* 164 - center of mass */
   void                *__unknown_176;            /* 176 - pointer to a "entry node info" pool item */
   void                *__unknown_180;            /* 180 - pointer to a "ptr node Double" pool item */
   uint8_t             __unknown_184[4];          /* 184 */
   void                *__unknown_188;            /* 188 - pointer to a "building" pool item */
   void                *__unknown_192;            /* 192 - pointer to a "building" pool item */
   uint8_t             __unknown_196[20];         /* 196 */
   float               collision_something;       /* 216 */
	uint8_t				__unknown_220[4];			/* 220 - probably collision stuff */
	// end of handling should be here
	uint8_t				__unknown_224[12];			/* 224 */
   float               collision_position[3];     /* 236 - last collision coordinates */
   uint8_t             __unknown_248[68];         /* 248 */
   void                *__unknown_316;            /* 316 - pointer to this something in this structure */
   uint8_t             __unknown_320[157];        /* 320 */
   uint8_t             in_vehicle;                /* 477 */
   uint8_t             __unknown_478[422];        /* 478 */

	tHandlingDataSA		*pHandlingData;				/* 900 */
	uint8_t				__unknown_904[100];			/* 904 */
	uint32_t			hFlagsLocal;				/* 1004 */
	uint8_t				__unknown_1008[56];			/* 1008 - AutoPilot */
	CVehicleFlags		m_nVehicleFlags;			/* 1064 - 60 bits/flags */
	uint32_t			m_TimeOfCreation;			/* 1072 - GetTimeInMilliseconds when this vehicle was created. */

   uint8_t             color[4];                  /* 1076 - as in carcolors.dat - body, stripe, body2, stripe2 */
   uint32_t            modding[8];                /* 1080 - modding data */
   float               wheel_size;                /* 1112 */
	unsigned short		CarAlarmState;				/* 1116 - time left for car alarm to sound in ms */
	unsigned short		ForcedRandomSeed;			/* 1118 - if this is non-zero the random wander gets deterministic */
   struct actor_info   *passengers[9];           /* 1120 (pointer to the passenger player structs) */

	unsigned char		m_nNumPassengers;			/* 1156 */
	unsigned char		m_nNumGettingIn;			/* 1157 */
	unsigned char		m_nGettingInFlags;			/* 1158 */
	unsigned char		m_nGettingOutFlags;			/* 1159 */
	unsigned char		m_nMaxPassengers;			/* 1160 */
	unsigned char		m_windowsOpenFlags;			/* 1161 */
	char				m_nNitroBoosts;				/* 1162 */
	char				m_nSpecialColModel;			/* 1163 */
	DWORD				*pEntityWeAreOnForVisibilityCheck;	/* 1164 - we need a CEntity */
	DWORD				*m_pFire;					/* 1168 - CFire*/

   float               steer_angles[2];           /* 1172 - steer angles */
   float               gas_pedal;                 /* 1180 - gas pedal */
   float               break_pedal;               /* 1184 - break pedal */

	// 1188
	unsigned char		VehicleCreatedBy;	// Contains information on whether this vehicle can be deleted or not. Probably only need this or IsLocked.
	short				ExtendedRemovalRange;

	// this padding probably isn't in the right place.
	uint8_t __unknown_1191;

	//1192
	unsigned char		car_bomb: 3; // 0 = None. 1 = Timed. 2 = On ignition, 3 = remotely set ? 4 = Timed Bomb has been activated. 5 = On ignition has been activated.
	unsigned char		OverrideLights: 2; // uses enum NO_CAR_LIGHT_OVERRIDE, FORCE_CAR_LIGHTS_OFF, FORCE_CAR_LIGHTS_ON
	unsigned char		WinchType: 2; // Does this vehicle use a winch?

	// this padding probably isn't in the right place.
	uint8_t __unknown_1193;

	//1194
	unsigned char		m_GunsCycleIndex: 2;// Cycle through alternate gun hardpoints on planes/helis
	unsigned char		m_OrdnanceCycleIndex: 2; // Cycle through alternate ordnance hardpoints on planes/helis

	// 1195
	unsigned char		nUsedForCover;// Has n number of cops hiding/attempting to hid behind it
	unsigned char		AmmoInClip;// Used to make the guns on boat do a reload.
	unsigned char		PacMansCollected;
	unsigned char		PedsPositionForRoadBlock;
	unsigned char		NumPedsForRoadBlock;

	//1200
	float				nBodyDirtLevel; // Dirt level of vehicle body texture: 0.0f=fully clean, 15.0f=maximum dirt visible, it may be altered at any time while vehicle's cycle of lige
	// values used by transmission
	unsigned char		m_nCurrentGear;				/* 1204 */
	float				m_fGearChangeCount;			/* 1205 */
	float				m_fWheelSpinForAudio;		/* 1209 */
	uint8_t				__unknown_1213[3];				/* 1213 */

   float               hitpoints;                 /* 1216 */
   float               armor;                     /* 1220 */
   struct vehicle_info *trailer;                  /* 1224 - pointer to the currently attached trailer; 0 if theres no trailer */
   uint8_t             __unknown_1228[44];        /* 1228 */
   uint32_t            door_status;               /* 1272 - car door status (1:Opened 2:Locked) */
   uint8_t             __unknown_1276[148];       /* 1276 */
   uint8_t             vehicle_type;              /* 1424 - VEHICLE_TYPE_* */
   uint8_t             __unknown_1425[20];        /* 1425 */
   uint8_t             car_tire_status[4];        /* 1445 - LF, LR, RF, RR, 0 = ok, 1 = flat, 2 = landing gear up */
   uint8_t             __unknown_1449[83];        /* 1449 */
   struct detachable   detachable_bike1[1];       /* 1532 - bike/motorcycle part */
   uint8_t             __unknown_1576[52];        /* 1576 */
   uint8_t             bike_tire_status[2];       /* 1628 Front, Rear, 0 = ok, 1 = flat */
   uint8_t             __unknown_1630[2];         /* 1630 */
   struct detachable   detachable_bike2[4];       /* 1632 - bike/motorcycle and boat parts */
   uint8_t             __unknown_1808[20];        /* 1808 */
   struct detachable   detachable_car[4];         /* 1828 - car/helicopter/airplane parts */
   uint8_t             __unknown_2004[12];        /* 2004 */
   float               suspension[4];             /* 2016 - FL, RL, FR, RR suspension height */
   uint8_t             __unknown_2032[244];       /* 2032 */
   float               burn_timer;                /* 2276 - burn timer counting up from 0.0f */
   uint8_t             __unknown_2280[304];       /* 2280 */
   /* 2280 might actually be the start of the next CVehicle */
};                                               /* 2584 */

struct object_info
{
   #pragma pack(1)
   struct object_base base;

};

struct checkpoint
{
   #pragma pack(1)
   uint32_t __unknown1;       /* Dunno, but I'm guessing the type of checkpoint is stored here */
   uint32_t counter;          /* Some sort of counter. */
   uint32_t color;            /* RGBA color value */
   uint32_t __unknown2;       /* 1024 most of the time */
   float    position[3];
   float    rotation[3];      /* direction from this checkpoint to the next */
   float    zero_point_two;
   float    radius;           /* radius? */
   uint32_t __unknown3;       /* always zero */
   uint32_t __unknown4;       /* always zero */
};

struct vehicle_state
{
   float                matrix[16];
   float                speed[3];
   float                spin[3];
   struct vehicle_state *trailer;
   struct vehicle_info  *trailerPtr;
};

static struct patch_set patch_vehicle_inf_NOS = {
	"Vehicle Infinite NOS", 0, 0, {
		{ 1,  (void *)0x006A3FFA, (uint8_t *)"\x7A", (uint8_t *)"\xEB", (uint8_t *)"\x7A" },
		{ 2,  (void *)0x006A3FB9, (uint8_t *)"\xFE\xC8", (uint8_t *)"\x90\x90", (uint8_t *)"\xFE\xC8" }
	} 
};

/* __time_current is set in cheat_hook(). the time is "cached".
   by doing this we save some CPU time, and we get a constant time */
#define time_get() __time_current

extern uint32_t            __time_current;

extern struct pool         *pool_actor;
extern struct pool         *pool_vehicle;

extern struct cheat_state  *cheat_state;
extern float               *cam_matrix;

void cheat_hook(HWND wnd);
