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

// camera defines
#define CLASS_CCamera 0xB6F028 // ##SA##
#define MAX_CAMS 3
#define NUMBER_OF_VECTORS_FOR_AVERAGE 2
#define CAM_NUM_TARGET_HISTORY 4
#define FLOAT_EPSILON 0.0001f



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
	VECTOR at; // front
	float  pad_a;
	VECTOR pos;
	float  pad_p;
} MATRIX4X4, *PMATRIX4X4;

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
   int		spiderWheels_on;
   CVector	gravityVector;
   //VECTOR	gravityVector;
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

struct cheat_state_hooks
{
	// i'm a dummy
	bool dummy;
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
   struct cheat_state_hooks hooks;
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


// object_base is now unioned with a CEntity pointer where applicable
struct object_base
{
	#pragma pack(1)
	CEntitySAInterfaceVTBL *vtbl;		// 0

// this is now in CEntity
//	CPlaceableSAInterface	Placeable;	// 4
// but the following variables exist for compatability till we remove object_base completely

	float		coords[3];				// 4
	union {
		// is MTA right?
		float		m_heading;				// 16
		// and are we wrong?
		float		*preMatrix;			// 16, a part of CPlaceable
		MATRIX4X4	*preMatrixStruct;	// 16, a part of CPlaceable
	};
	union {
		float		*matrix;			// 20
		MATRIX4X4	*matrixStruct;		// 20
	};

	RpClump			*m_pRwObject;		// 24

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

class CVehicleFlags
{
public:
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
	/*
	union
	{
		struct object_base base;
		struct
		{
			CEntitySAInterface m_CEntitySAInterface;
		};
	};
	*/
   uint8_t             flags;                     /* 66 - flags */
   uint8_t             __unknown_67;              /* 67 */
	union /* 68 */
	{
		struct
		{
			CVector			m_SpeedVec;
		};
		float			speed[3];
	};
	union /* 80 */
	{
		struct
		{
			CVector			m_SpinVec;
		};
		float			spin[3];
	};
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
	uint8_t				__unknown_1213[3];			/* 1213 */

   float               hitpoints;                 /* 1216 */
   float               armor;                     /* 1220 */
   struct vehicle_info *trailer;                  /* 1224 - pointer to the currently attached trailer; 0 if theres no trailer */
   uint8_t             __unknown_1228[44];        /* 1228 */
   uint32_t            door_status;               /* 1272 - car door status (1:Opened 2:Locked) */
   uint8_t             __unknown_1276[148];       /* 1276 */
   uint8_t             vehicle_type;              /* 1424 - VEHICLE_TYPE_* */
   uint8_t             __unknown_1425[19];        /* 1425 */

	union
	{
		struct
		{
			float		m_fTrainSpeed;				/* 1444 - Train speed along rails */
			float		m_fTrainRailDistance;		/* 1448 - Distance along rail starting from first rail node (determines train position when on rails) */
		};
		struct
		{
			uint8_t		__unknown_1444;				/* 1444 */
			uint8_t		car_tire_status[4];			/* 1445 - LF, LR, RF, RR, 0 = ok, 1 = flat, 2 = landing gear up */
			uint8_t		__unknown_1449[4];			/* 1449 */
		};
	};

   uint8_t             __unknown_1453[79];        /* 1449 */
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
	uint8_t				__unknown_2280[280];		/* 2280 */
	uint8_t				SmokeTrailEnabled;			/* 2560 */
	uint8_t				__unknown_2561[23];			/* 2561 */
	/* 2584 */
};


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




struct CCamSAInterface // 568 bytes?
{
	bool	bBelowMinDist; //used for follow ped mode
	bool	bBehindPlayerDesired; //used for follow ped mode
	bool 	m_bCamLookingAtVector;
	bool 	m_bCollisionChecksOn;
	bool	m_bFixingBeta; //used for camera on a string
	bool 	m_bTheHeightFixerVehicleIsATrain;
	bool 	LookBehindCamWasInFront;
	bool 	LookingBehind;
	bool 	LookingLeft; // 32
	bool 	LookingRight;
	bool 	ResetStatics; //for interpolation type stuff to work
	bool 	Rotating;

	short	Mode;					// CameraMode
	unsigned int  m_uiFinishTime; // 52
	
	int 	m_iDoCollisionChecksOnFrameNum; 
	int 	m_iDoCollisionCheckEveryNumOfFrames;
	int 	m_iFrameNumWereAt;	// 64
	int 	m_iRunningVectorArrayPos;
	int 	m_iRunningVectorCounter;
	int 	DirectionWasLooking;
	
	float 	f_max_role_angle; //=DEGTORAD(5.0f);	
	float 	f_Roll; //used for adding a slight roll to the camera in the
	float 	f_rollSpeed; //camera on a string mode
	float 	m_fSyphonModeTargetZOffSet;
	float 	m_fAmountFractionObscured;
	float 	m_fAlphaSpeedOverOneFrame; // 100
	float 	m_fBetaSpeedOverOneFrame;
	float 	m_fBufferedTargetBeta;
	float 	m_fBufferedTargetOrientation;
	float	m_fBufferedTargetOrientationSpeed;
	float 	m_fCamBufferedHeight;
	float 	m_fCamBufferedHeightSpeed;
	float 	m_fCloseInPedHeightOffset;
	float 	m_fCloseInPedHeightOffsetSpeed; // 132
	float 	m_fCloseInCarHeightOffset;
	float 	m_fCloseInCarHeightOffsetSpeed;
	float 	m_fDimensionOfHighestNearCar;		
	float	m_fDistanceBeforeChanges;
	float 	m_fFovSpeedOverOneFrame;
	float 	m_fMinDistAwayFromCamWhenInterPolating;
	float 	m_fPedBetweenCameraHeightOffset;
	float 	m_fPlayerInFrontSyphonAngleOffSet; // 164
	float	m_fRadiusForDead;
	float	m_fRealGroundDist; //used for follow ped mode
	float 	m_fTargetBeta;
	float 	m_fTimeElapsedFloat;	
	float 	m_fTilt;
	float 	m_fTiltSpeed;

	float   m_fTransitionBeta;
	float 	m_fTrueBeta;
	float 	m_fTrueAlpha; // 200
	float	m_fInitialPlayerOrientation; //used for first person

	float	Alpha;
	float	AlphaSpeed;
	float	FOV;
    float	FOVSpeed;
	float	Beta;
	float	BetaSpeed;
	float	Distance; // 232
	float	DistanceSpeed;
	float 	CA_MIN_DISTANCE;
	float 	CA_MAX_DISTANCE;
	float	SpeedVar;
	float	m_fCameraHeightMultiplier; //used by TwoPlayer_Separate_Cars_TopDown
	
	// ped onfoot zoom distance
	float m_fTargetZoomGroundOne;
	float m_fTargetZoomGroundTwo; // 256
	float m_fTargetZoomGroundThree;
	// ped onfoot alpha angle offset
	float m_fTargetZoomOneZExtra;
	float m_fTargetZoomTwoZExtra;
	float m_fTargetZoomTwoInteriorZExtra; //extra one for interior
	float m_fTargetZoomThreeZExtra;
	
	float m_fTargetZoomZCloseIn;
	float m_fMinRealGroundDist;
	float m_fTargetCloseInDist;

	// For targetting in cooperative mode.
	float	Beta_Targeting; // 292
	float	X_Targetting, Y_Targetting;
	int	CarWeAreFocussingOn; //which car is closer to the camera in 2 player cooperative mode with separate cars.
	float	CarWeAreFocussingOnI; //interpolated version
	
	float m_fCamBumpedHorz; // 312
	float m_fCamBumpedVert;
	int	m_nCamBumpedTime; // 320
	static int CAM_BUMPED_SWING_PERIOD;
	static int CAM_BUMPED_END_TIME;
	static float CAM_BUMPED_DAMP_RATE;
	static float CAM_BUMPED_MOVE_MULT;

	CVector m_cvecSourceSpeedOverOneFrame; // 324
	CVector m_cvecTargetSpeedOverOneFrame; // 336
	CVector m_cvecUpOverOneFrame; // 348
	
	CVector m_cvecTargetCoorsForFudgeInter; // 360
	CVector m_cvecCamFixedModeVector; // 372
	CVector m_cvecCamFixedModeSource; // 384
  	CVector m_cvecCamFixedModeUpOffSet; // 396
	CVector m_vecLastAboveWaterCamPosition; //408  //helper for when the player has gone under the water

	CVector m_vecBufferedPlayerBodyOffset; // 420

	// The three vectors that determine this camera for this frame
	CVector	Front;	// 432											// Direction of looking in
	CVector	Source;													// Coors in world space
	CVector	SourceBeforeLookBehind;
	CVector	Up;														// Just that
	CVector	m_arrPreviousVectors[NUMBER_OF_VECTORS_FOR_AVERAGE];	// used to average stuff

	CVector m_aTargetHistoryPos[CAM_NUM_TARGET_HISTORY];
	DWORD m_nTargetHistoryTime[CAM_NUM_TARGET_HISTORY];
	DWORD m_nCurrentHistoryPoints;

	object_base *CamTargetEntity; // CEntitySAInterface
/*
//protected:
	float 		m_fCameraDistance;
	float 		m_fIdealAlpha;
	float 		m_fPlayerVelocity;
	//CVector TempRight;
	CAutomobileSAInterface	*m_pLastCarEntered; // So interpolation works
	CPedSAInterface			*m_pLastPedLookedAt;// So interpolation works 
	bool		m_bFirstPersonRunAboutActive;
*/
};

struct CCameraSAInterface 
{
	// CPlaceable
	CPlaceableSAInterface	Placeable;
	// End CPlaceable

	//move these out the class, have decided to set up a mirrored enumerated type thingy at the top
	bool 	m_bAboveGroundTrainNodesLoaded;
	bool 	m_bBelowGroundTrainNodesLoaded;
	bool 	m_bCamDirectlyBehind;	
	bool 	m_bCamDirectlyInFront;	
  	bool	m_bCameraJustRestored;
	bool 	m_bcutsceneFinished;
	bool 	m_bCullZoneChecksOn;
	bool 	m_bFirstPersonBeingUsed; // To indicate if the m_bFirstPersonBeingUsed viewer is being used.
	bool	m_bJustJumpedOutOf1stPersonBecauseOfTarget;
	bool 	m_bIdleOn;
	bool  	m_bInATunnelAndABigVehicle;
	bool 	m_bInitialNodeFound;
    bool 	m_bInitialNoNodeStaticsSet;
	bool   	m_bIgnoreFadingStuffForMusic;
	bool  	m_bPlayerIsInGarage;
	bool	m_bPlayerWasOnBike;
	bool  	m_bJustCameOutOfGarage;
	bool 	m_bJustInitalised;//Just so the speed thingy doesn't go mad right at the start
    unsigned char 	m_bJust_Switched;//Variable to indicate that we have jumped somewhere, Raymond needs this for the audio engine
	bool 	m_bLookingAtPlayer;
	bool 	m_bLookingAtVector;
	bool 	m_bMoveCamToAvoidGeom;
	bool 	m_bObbeCinematicPedCamOn;
	bool 	m_bObbeCinematicCarCamOn;
	bool 	m_bRestoreByJumpCut;
	bool   	m_bUseNearClipScript;
	bool 	m_bStartInterScript;
	unsigned char 	m_bStartingSpline;
	bool 	m_bTargetJustBeenOnTrain; //this variable is needed to be able to restore the camera
	bool 	m_bTargetJustCameOffTrain;
    bool 	m_bUseSpecialFovTrain;
	bool    m_bUseTransitionBeta;
	bool 	m_bUseScriptZoomValuePed;
	bool 	m_bUseScriptZoomValueCar;
	bool 	m_bWaitForInterpolToFinish;
	bool	m_bItsOkToLookJustAtThePlayer; //Used when interpolating
	bool 	m_bWantsToSwitchWidescreenOff;	
	bool 	m_WideScreenOn;	
	bool	m_1rstPersonRunCloseToAWall;
	bool	m_bHeadBob;
	bool 	m_bVehicleSuspenHigh;
	bool 	m_bEnable1rstPersonCamCntrlsScript; 
	bool 	m_bAllow1rstPersonWeaponsCamera;
	bool	m_bCooperativeCamMode;
	bool	m_bAllowShootingWith2PlayersInCar;
	bool	m_bDisableFirstPersonInCar;
	static bool	m_bUseMouse3rdPerson;
//#ifndef FINALBUILD
	bool 	bStaticFrustum;
//#endif

	// for debug keyboard stuff
//#ifndef MASTER
	unsigned char display_kbd_debug;
	float kbd_fov_value;
//#endif // MASTER

		// The following fields allow the level designers to specify the camera for 2 player games.
	short	m_ModeForTwoPlayersSeparateCars;
	short	m_ModeForTwoPlayersSameCarShootingAllowed;
	short	m_ModeForTwoPlayersSameCarShootingNotAllowed;
	short	m_ModeForTwoPlayersNotBothInCar;

	bool 	m_bGarageFixedCamPositionSet;
    bool 	m_vecDoingSpecialInterPolation;
	bool 	m_bScriptParametersSetForInterPol;

	
	bool 	m_bFading;//to indicate that we are fading 
	bool 	m_bMusicFading;
	bool 	m_bMusicFadedOut;

	bool 	m_bFailedCullZoneTestPreviously;
	bool 	m_FadeTargetIsSplashScreen;//used as hack for fading 
	bool 	WorldViewerBeingUsed; // To indicate if the world viewer is being used.										 

	
	unsigned char	m_uiTransitionJUSTStarted;	// This is the first frame of a transition.
	unsigned char	m_uiTransitionState;		// 0:one mode 1:transition
	unsigned char	ActiveCam;				// Which one at the moment (0 or 1)
										// Their is a fudge at the end when the renderware matrix will receive either
										// the active camera or the worldviewer camera
	unsigned int	m_uiCamShakeStart;			// When did the camera shake start.
	unsigned int 	m_uiFirstPersonCamLastInputTime;
 	unsigned int 	m_uiLongestTimeInMill;
  	unsigned int 	m_uiNumberOfTrainCamNodes;
    unsigned int 	m_uiTimeLastChange;
	unsigned int 	m_uiTimeWeLeftIdle_StillNoInput;
	unsigned int  m_uiTimeWeEnteredIdle;
	unsigned int	m_uiTimeTransitionStart;	// When was the transition started ?
	unsigned int	m_uiTransitionDuration;		// How long does the transition take ?
	unsigned int	m_uiTransitionDurationTargetCoors;
	int 	m_BlurBlue;
	int 	m_BlurGreen;
	int 	m_BlurRed;
	int 	m_BlurType;
	int 	m_iWorkOutSpeedThisNumFrames;//duh	
	int 	m_iNumFramesSoFar; //counter
	int 	m_iCurrentTrainCamNode;//variable indicating which camera node we are at for the train
  	int 	m_motionBlur;//to indicate that we are fading

	int 	m_imotionBlurAddAlpha;
	int 	m_iCheckCullZoneThisNumFrames; 
	int 	m_iZoneCullFrameNumWereAt;
	int 	WhoIsInControlOfTheCamera; //to discern between obbe and scripts


//	float	CarZoomIndicator;
//	float 	CarZoomValue;
//	float	CarZoomValueSmooth;
//	float 	m_fCarZoomValueScript;
//	float 	PedZoomIndicator;
//	float	m_fPedZoomValue;
//	float	m_fPedZoomValueSmooth;
//	float 	m_fPedZoomValueScript;
	int	m_nCarZoom; 				// store zoom index
	float 	m_fCarZoomBase;				// store base zoom distance from index
	float	m_fCarZoomTotal;			// store total zoom after modded by camera modes
	float	m_fCarZoomSmoothed;			// buffered version of the var above
	float 	m_fCarZoomValueScript;
	int 	m_nPedZoom;					// store zoom index
	float	m_fPedZoomBase;				// store base zoom distance from index
	float	m_fPedZoomTotal;			// store total zoom after modded by camera modes
	float	m_fPedZoomSmoothed;			// buffered version of the var above
	float 	m_fPedZoomValueScript;


	float	CamFrontXNorm, CamFrontYNorm;
	float	DistanceToWater;
	float	HeightOfNearestWater;
	float 	FOVDuringInter;
	float	LODDistMultiplier;	// This takes into account the FOV and the standard LOD multiplier Smaller aperture->bigger LOD multipliers.
	float 	GenerationDistMultiplier;	// This takes into account the FOV but noy the standard LOD multiplier

	float 	m_fAlphaSpeedAtStartInter;
  	float	m_fAlphaWhenInterPol;
	float 	m_fAlphaDuringInterPol;
	float   m_fBetaDuringInterPol;
	float 	m_fBetaSpeedAtStartInter;
  	float 	m_fBetaWhenInterPol;
	float 	m_fFOVWhenInterPol;
	float 	m_fFOVSpeedAtStartInter;
	float 	m_fStartingBetaForInterPol;
	float 	m_fStartingAlphaForInterPol;
  	float   m_PedOrientForBehindOrInFront;

	float 	m_CameraAverageSpeed; //this is an average depending on how many frames we work it out
	float 	m_CameraSpeedSoFar; //this is a running total
	float	m_fCamShakeForce;			// How severe is the camera shake.
    float 	m_fFovForTrain;
	float 	m_fFOV_Wide_Screen;

	float  	m_fNearClipScript;
	float	m_fOldBetaDiff;			// Needed for interpolation between 2 modes
	float 	m_fPositionAlongSpline;//Variable used to indicate how far along the spline we are 0-1 for started to completed respectively
	float 	m_ScreenReductionPercentage;
	float 	m_ScreenReductionSpeed;
	float 	m_AlphaForPlayerAnim1rstPerson;
	
	float	Orientation;			// The orientation of the camera. Used for peds walking.
	float	PlayerExhaustion;		// How tired is player (inaccurate sniping) 0.0f-1.0f
					// The following things are used by the sound code to
					// play reverb depending on the surroundings. From a point
					// in front of the camera the disance is measured to the
					// nearest obstacle (building)
	float	SoundDistUp; //, SoundDistLeft, SoundDistRight;		// These ones are buffered and should be used by the audio
	float	SoundDistUpAsRead; //, SoundDistLeftAsRead, SoundDistRightAsRead;
	float	SoundDistUpAsReadOld; //, SoundDistLeftAsReadOld, SoundDistRightAsReadOld;
					// Very rough distance to the nearest water for the sound to use
					// Front vector X&Y normalised to 1. Used by loads of stuff.

	
	float	m_fAvoidTheGeometryProbsTimer;
	short	m_nAvoidTheGeometryProbsDirn;
	
	float 	m_fWideScreenReductionAmount;//0 for not reduced 1 for fully reduced (Variable for Les)
	float   m_fStartingFOVForInterPol;
	
		// These ones are static so that they don't get cleared in CCamera::Init()
	static	float m_fMouseAccelHorzntl;// acceleration multiplier for 1st person controls
	static	float m_fMouseAccelVertical;// acceleration multiplier for 1st person controls
	static	float m_f3rdPersonCHairMultX;
	static	float m_f3rdPersonCHairMultY;

	CCamSAInterface	Cams[3];	// The actual cameras (usually only one of the two is active)
								// And to complicate this we have a third camera, this camera is 
								// used for debugging when we want to have a look at the world.
								// We can't change the camera mode because other objects depend on their

	// #########################################//
	// DATA NOT UPDATED FOR SA  BELOW HERE!!!!! //
	// #########################################//
	
/*
some day we might actually need some of this?

	CGarageSAInterface *pToGarageWeAreIn;
	CGarageSAInterface *pToGarageWeAreInForHackAvoidFirstPerson;
	CQueuedMode m_PlayerMode;
	// The higher priority player camera mode. This one is used
	// for the sniper mode and rocket launcher mode.
	// This one overwrites the m_PlayerMode above.
	CQueuedMode PlayerWeaponMode;
	CVector m_PreviousCameraPosition; //needed to work out speed
	CVector m_RealPreviousCameraPosition; // This cane be used by stuff outside the camera code. The one above is the same as the current coordinates outwidth the camera code.
										// an active camera for range finding etc
	CVector m_cvecAimingTargetCoors;		// Coors to look at with Gordons aiming thing
	// The player camera that is waiting to be used
	// This camera can replace the default camera where this is
	// needed (in tricky situations like tunnels for instance)
	CVector m_vecFixedModeVector;
	CVector m_vecFixedModeSource;
	CVector m_vecFixedModeUpOffSet;
	CVector m_vecCutSceneOffset;
	CVector m_cvecStartingSourceForInterPol;
	CVector m_cvecStartingTargetForInterPol;
	CVector m_cvecStartingUpForInterPol;
	CVector m_cvecSourceSpeedAtStartInter;
	CVector m_cvecTargetSpeedAtStartInter;
	CVector m_cvecUpSpeedAtStartInter;
	CVector m_vecSourceWhenInterPol;
	CVector m_vecTargetWhenInterPol;
	CVector m_vecUpWhenInterPol;
	CVector m_vecClearGeometryVec;
	CVector m_vecGameCamPos;
	CVector SourceDuringInter, TargetDuringInter, UpDuringInter;


	CVector m_vecAttachedCamOffset; // for attaching the camera to a ped or vehicle (set by level designers for use in cutscenes)
	CVector m_vecAttachedCamLookAt;	
	FLOAT m_fAttachedCamAngle; // for giving the attached camera a tilt.

	// RenderWare camera pointer
	DWORD * m_pRwCamera; // was RwCamera *
	///stuff for cut scenes
	CEntitySAInterface *pTargetEntity;
	CEntitySAInterface *pAttachedEntity;
	//CVector CutScene; 
	CCamPathSplines m_arrPathArray[MAX_NUM_OF_SPLINETYPES]; //These only get created when the script calls the load splines function
	// maybe this shouldn't be here depends if GTA_TRAIN is defined (its not)
	//CTrainCamNode 	m_arrTrainCamNode[MAX_NUM_OF_NODES];

	bool m_bMirrorActive;
	bool m_bResetOldMatrix;

//	protected:
	CMatrix_Padded m_cameraMatrix;
	CMatrix_Padded m_cameraMatrixOld;
	CMatrix_Padded m_viewMatrix;
	CMatrix_Padded m_matInverse;
	CMatrix_Padded m_matMirrorInverse;
	CMatrix_Padded m_matMirror;

	CVector m_vecFrustumNormals[4];
	CVector m_vecFrustumWorldNormals[4];
	CVector m_vecFrustumWorldNormals_Mirror[4];

	FLOAT m_fFrustumPlaneOffsets[4];
	FLOAT m_fFrustumPlaneOffsets_Mirror[4];

	CVector m_vecRightFrustumNormal;
	CVector m_vecBottomFrustumNormal;
	CVector m_vecTopFrustumNormal;

	CVector m_vecOldSourceForInter;
	CVector m_vecOldFrontForInter;
	CVector m_vecOldUpForInter;
	FLOAT 	m_vecOldFOVForInter;
	FLOAT 	m_fFLOATingFade;//variable representing the FLOAT version of CDraw::Fade. Necessary to stop loss of precision
	FLOAT 	m_fFLOATingFadeMusic;
	FLOAT 	m_fTimeToFadeOut;
	FLOAT 	m_fTimeToFadeMusic;
	FLOAT	m_fTimeToWaitToFadeMusic;
	FLOAT   m_fFractionInterToStopMoving; 
	FLOAT 	m_fFractionInterToStopCatchUp;
	FLOAT   m_fFractionInterToStopMovingTarget; 
	FLOAT 	m_fFractionInterToStopCatchUpTarget;

	FLOAT 	m_fGaitSwayBuffer;
	FLOAT   m_fScriptPercentageInterToStopMoving;
	FLOAT   m_fScriptPercentageInterToCatchUp;
	DWORD	m_fScriptTimeForInterPolation;


	short 	m_iFadingDirection;
	int 	m_iModeObbeCamIsInForCar;
	short 	m_iModeToGoTo;
	short 	m_iMusicFadingDirection;
	short 	m_iTypeOfSwitch;

	DWORD 	m_uiFadeTimeStarted;
	DWORD 	m_uiFadeTimeStartedMusic;
*/
};

struct CCamSA
{
	CCamSAInterface*    m_pInterface;
};

struct CCameraSA
{
	CCameraSAInterface *internalInterface;
	CCamSA Cams[MAX_CAMS]; // 0x4
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
