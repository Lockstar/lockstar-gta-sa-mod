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
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/GTAfuncs.h $
	$Id: GTAfuncs.h 43 2010-01-05 07:40:59Z futnucks $

*/


// gta function addresses
#define		ARRAY_ModelLoaded				0x8E4CD0
#define		ARRAY_ModelInfo					0xA9B0C8
#define		FUNC_CStreaming__HasModelLoaded	0x4044C0
#define		FUNC_GetModelFlags				0x4044E0
#define		FUNC_GetBoundingBox				0x4082F0
#define		FUNC_IsBoatModel				0x4c5a70
#define		FUNC_IsCarModel					0x4c5aa0
#define		FUNC_IsTrainModel				0x4c5ad0
#define		FUNC_IsHeliModel				0x4c5b00
#define		FUNC_IsPlaneModel				0x4c5b30
#define		FUNC_IsBikeModel				0x4c5b60
#define		FUNC_IsFakePlaneModel			0x4c5b90
#define		FUNC_IsMonsterTruckModel		0x4c5bc0
#define		FUNC_IsQuadBikeModel			0x4c5bf0
#define		FUNC_IsBmxModel					0x4c5c20
#define		FUNC_IsTrailerModel				0x4c5c50
#define		FUNC_IsVehicleModelType			0x4c5c80
#define		FUNC_RequestModel				0x4087e0
#define		FUNC_RemoveModel				0x4089a0
#define		FUNC_LoadAllRequestedModels		0x40ea10
#define     FUNC_FlushRequestList			0x40E4E0
#define     FUNC_HasVehicleUpgradeLoaded	0x407820
#define     FUNC_RequestMyVehicleUpgrade	0x408C70
#define     FUNC_RequestAnyVehicleUpgrade	0x6E3290
//#define		FUNC_CVehicle__SetRemapTexDictionary 0x6D0BC0
#define		FUNC_CVehicle__GetRemapIndex	0x6D0B70
#define		FUNC_CVehicle__SetRemap			0x6D0C00
#define		FUNC_SetCurrentVideoMode		0x745C70
#define		FUNC_GetCurrentVideoMode		0x7F2D20
#define		FUNC_GetNumVideoModes			0x7F2CC0
#define		FUNC_GetVideoModeInfo			0x7F2CF0

// new testing ish
#define		FUNC_RwD3D9ChangeVideoMode		0x7F8640


// gta class addresses
#define		CLASS_CMenuManager				0xBA6748


// additional defines
#define RWFORCEENUMSIZEINT ((int)((~((unsigned int)0))>>1))



////////////////////////
// structures & enums //
////////////////////////

enum VideoModeFlag // RwVideoModeFlag
{
    rwVIDEOMODEEXCLUSIVE    = 0x0001, /**<Exclusive (i.e. full-screen) */
    rwVIDEOMODEINTERLACE    = 0x0002, /**<Interlaced                   */
    rwVIDEOMODEFFINTERLACE  = 0x0004, /**<Flicker Free Interlaced      */

    /* Platform specific video mode flags. */

    rwVIDEOMODE_PS2_FSAASHRINKBLIT    = 0x0100,
    /**< \if sky2
     *   Full-screen antialiasing mode 0
     *   \endif
     */
    rwVIDEOMODE_PS2_FSAAREADCIRCUIT   = 0x0200,
    /**< \if sky2
     *   Full-screen antialiasing mode 1
     *   \endif
     */

    rwVIDEOMODE_XBOX_WIDESCREEN       = 0x0100,
    /**< \if xbox
     *   Wide screen.
     *   \endif
     */
    rwVIDEOMODE_XBOX_PROGRESSIVE      = 0x0200,
    /**< \if xbox
     *   Progressive.
     *   \endif
     */
    rwVIDEOMODE_XBOX_FIELD            = 0x0400,
    /**< \if xbox
     *   Field rendering.
     *   \endif
     */
    rwVIDEOMODE_XBOX_10X11PIXELASPECT = 0x0800,
    /**< \if xbox
     *   The frame buffer is centered on the display.
     *   On a TV that is 704 pixels across, this would leave 32 pixels of black
     *   border on the left and 32 pixels of black border on the right.
     *   \endif
     */

    rwVIDEOMODEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

struct VideoMode //RwVideoMode
{
    int             width;   /**< Width  */
    int             height;  /**< Height */
    int             depth;   /**< Depth  */
    VideoModeFlag   flags;   /**< Flags  */
    int             refRate; /**< Approximate refresh rate */
    int             format;  /**< Raster format \ see RwRasterFormat */
};

struct CSettingsSAInterface // see code around 0x57CE9A for where these are
{
	uint8_t		__unknown_h0[0x4];		// 0x0
	float		fStatsScrollSpeed;		// 0x4
	uint8_t		__unknown_h8[0x1C];		// 0x8
	uint8_t		drawRadar;				// 0x24 // 1 - only player blip and north, 2 - no radar
	uint8_t		__unknown_h25[0xE];		// 0x25
	uint8_t		activateMenu;			// 0x33
	uint8_t		menuAccessWidescreen;	// 0x34
	uint8_t		__unknown_h35[0x7];		// 0x35
	float		fBrightness;			// 0x3C
	float		fDrawDistance;			// 0x40
	bool		bSubtitles;				// 0x44
	bool		__boolPad_h45[5];		// 0x45
	bool		bLegend;				// 0x4A
	bool		bUseWideScreen;			// 0x4B
	bool		bFrameLimiter;			// 0x4C
	bool		bAutoRetune;			// 0x4D
	bool		__boolPad_h4E;			// 0x4E
	uint8_t		ucSfxVolume;			// 0x4F
	uint8_t		ucRadioVolume;			// 0x50
	uint8_t		ucRadioEq;				// 0x51
	uint8_t		ucRadioStation;			// 0x52
	uint8_t		__unknown_h53[0x6];		// 0x53
	uint8_t		drawRadarOrMap;			// 0x59
	uint8_t		__unknown_h5A[0x57];	// 0x5A
	bool		bInvertPadX1;			// 0xB1
	bool		bInvertPadY1;			// 0xB2
	bool		bInvertPadX2;			// 0xB3
	bool		bInvertPadY2;			// 0xB4
	bool		bSwapPadAxis1;			// 0xB5
	bool		bSwapPadAxis2;			// 0xB6
	uint8_t		__unknown_hB7[0x19];	// 0xB7
	bool		bUseKeyboardAndMouse;	// 0xD0
	uint8_t		__unknown_hD1[3];		// 0xD1
	int			dwVideoMode;			// 0xD4
	int			dwPrevVideoMode;		// 0xD8
	uint8_t		__unknown_hDC[4];		// 0xDC
	int			mousePosLeftA;			// 0xE0
	int			mousePosTopA;			// 0xE4
	// nothing else mapped till the end
	uint8_t		__unknown_hE8[0x1A90];	// 0xE8
	// the end							// 0x1B78
};

struct RsInputDevice
{
	DWORD	inputDeviceType;
	DWORD	used;
	DWORD	*inputEventHandler; // typedef RsEventStatus (*RsInputEventHandler)(RsEvent event, void *param);
};

struct PsGlobalType
{
	DWORD	window;
	DWORD	instance;
	int		fullscreen;
	DWORD	lastMousePos_X;
	DWORD	lastMousePos_Y;
	DWORD	__unknown_h14;
	DWORD	diInterface;
	DWORD	diMouse;
	DWORD	diDevice1;
	DWORD	diDevice2;
};

struct RsGlobalType
{
	DWORD			AppName;		// 0x0
	int				MaximumWidth;	// 0x4
	int				MaximumHeight;	// 0x8
	int				frameLimit;		// 0xC // (use with WM_ACTIVATE?)
	DWORD			quit;			// 0x10
	PsGlobalType	*ps;			// 0x14
	RsInputDevice	keyboard;		// 0x18
	RsInputDevice	mouse;			// 0x24
	RsInputDevice	pad;			// 0x30

};


//////////////
// GTAfuncs //
//////////////

// model funcs
int GTAfunc_isModelLoaded(int iModelID);
void GTAfunc_requestModelLoad(int iModelID);
void GTAfunc_loadRequestedModels(void);
bool GTAfunc_vehicle_iModelID_IsValid(int iModelID);
bool GTAfunc_vehicle_iModelID_IsCar(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsBike(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsPlane(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsHeli(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsBoat(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsQuadBike(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsBmx(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsMonsterTruck(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsTrailer(int m_dwModelID);
bool GTAfunc_vehicle_iModelID_IsTrain(int m_dwModelID);

// vehicle mod funcs
bool GTAfunc_IsUpgradeAvailable(int m_dwModelID, eVehicleUpgradePosn posn);
void GTAfunc_requestAnyVehicleUpgrade(vehicle_info *vinfo, int iModelID);
void GTAfunc_removeVehicleUpgrade(vehicle_info *vinfo, int iModelID);
int GTAfunc_vehicle_getRemapIndex(vehicle_info *vinfo);
void GTAfunc_vehicle_setRemap(vehicle_info *vinfo, int iRemap);

// game core funcs
int GTAfunc_getNumVideoModes(void);
VideoMode *GTAfunc_getVideoModeInfo(VideoMode *modeInfo, int modeIndex);
int GTAfunc_getCurrentVideoMode(void);
void GTAfunc_setCurrentVideoMode(int modeIndex);
int GTAfunc_RwD3D9ChangeVideoMode(int modeIndex);
