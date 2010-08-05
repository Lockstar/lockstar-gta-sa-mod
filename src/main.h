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
#ifndef __MODMAIN_H
#define __MODMAIN_H

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS 1

// main switch for development or release build
#define M0D_DEV

#define M0D_NAME		"m0d_s0beit_sa"
#define M0D_VERSION		"v4.1.0.0"
#define SAMP_VERSION	"SA:MP 0.3a"

#ifdef M0D_DEV
#define NAME	""M0D_NAME "-dev"" "M0D_VERSION " ("__DATE__ " """__TIME__ ")"
#else
#define NAME	""M0D_NAME """ "M0D_VERSION ""
#endif
#define INI_FILE				"m0d_s0beit_sa.ini"
#define RAW_TEX_FILE			"m0d_s0beit_sa.raw"

#define RUNMODE_SINGLEPLAYER	0
#define RUNMODE_SAMP			1

// let's do a precompiled header, why not
#pragma message( "Compiling precompiled header.\n" )

// illegal instruction size
#pragma warning( disable : 4409 )

// 'class1' : inherits 'class2::member' via dominance
#pragma warning( disable : 4250 )

// unreferenced formal parameter
#pragma warning( disable : 4100 )

// handler not registered as safe handler
#pragma warning( disable : 4733 )

// API/SDK includes
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <shellapi.h>
#include <d3dx9.h>
#include <Gdiplus.h>
#include <assert.h>
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

// typedefs/classes for legacy
#include "stddefs.h"
#include "stdtypes.h"
#include "CVector.h"
#include "CVector2D.h"
#include "CMatrix.h"
#include "CMatrix_Pad.h"

// public SA classes
// try to always include these in the private class's header
#include "SharedUtil.h"
#include <game/CGame.h>
#include <game/CWanted.h>

//#include <ijsify.h>
#include "SString.h"

// private SA classes
#include "CEntitySA.h"
#include "Common.h"
#include "CGameSA.h"
#include "CWorldSA.h"
#include "CPoolsSA.h"
#include "CClockSA.h"
#include "CFontSA.h"
#include "CRadarSA.h"
#include "CMenuManagerSA.h"
#include "CCameraSA.h"
#include "CCheckpointsSA.h"
#include "CRenderWareSA.h"
#include "CCoronasSA.h"
#include "CPickupsSA.h"
#include "CPathFindSA.h"
#include "CWeaponInfoSA.h"
#include "CExplosionManagerSA.h"
#include "CFireManagerSA.h"
#include "CHandlingManagerSA.h"
#include "CHudSA.h"
#include "C3DMarkersSA.h"
#include "CStatsSA.h"
#include "CTheCarGeneratorsSA.h"
#include "CPadSA.h"
#include "CAERadioTrackManagerSA.h"
#include "CWeatherSA.h"
#include "CTextSA.h"
#include "CPedSA.h"
#include "CPedSoundSA.h"
#include "CAudioSA.h"
#include "CPlayerInfoSA.h"
#include "CPopulationSA.h"
#include "CSettingsSA.h"
#include "CCarEnterExitSA.h"
#include "COffsets.h"
#include "CControllerConfigManagerSA.h"
#include "CProjectileInfoSA.h"
#include "CEventListSA.h"
#include "CGaragesSA.h"
#include "CTasksSA.h"
#include "CEventDamageSA.h"
#include "CEventGunShotSA.h"
#include "CEventGroupSA.h"
#include "CAnimManagerSA.h"
#include "CStreamingSA.h"
#include "CVisibilityPluginsSA.h"
#include "CKeyGenSA.h"
#include "CRopesSA.h"
#include "CFxSA.h"
#include "HookSystem.h"
#include "CModelInfoSA.h"
#include "CPedModelInfoSA.h"
#include "CColPointSA.h"
#include "CCivilianPedSA.h"
#include "CAnimBlendAssociationSA.h"
#include "CAnimBlendAssocGroupSA.h"
#include "CAnimBlendHierarchySA.h"
#include "CAnimBlendSequenceSA.h"
#include "CAnimBlendStaticAssociationSA.h"
#include "CAnimBlockSA.h"
#include "CAutomobileSA.h"
#include "CBikeSA.h"
#include "CBoatSA.h"
#include "CBmxSA.h"
#include "CQuadBikeSA.h"
#include "CMonsterTruckSA.h"
#include "CPlaneSA.h"
#include "CTrailerSA.h"
#include "CPlayerPedSA.h"
#include "CCivilianPedSA.h"
#include "CObjectSA.h"
#include "CBuildingSA.h"
#include "CRestartSA.h"
#include "CWaterManagerSA.h"
#include "CPedDamageResponseSA.h"
#include "CPedDamageResponseCalculatorSA.h"

// normal includes
#include "patcher.h"
#include "CDetour.h"
#include "cheat.h"
#include "ini.h"
#include "keyhook.h"
#include "math_stuff.h"
#include "cheat_funcs.h"
#include "cheat_generic.h"
#include "cheat_actor.h"
#include "cheat_vehicle.h"
#include "cheat_hookers.h"
#include "dumb_menu.h"
#include "samp.h"
#include "debug_classify.h"
#include "scripting.h"
#include "d3drender.h"
#include "GTAfuncs.h"
#include "proxyIDirect3D9.h"
#include "proxyIDirect3DDevice9.h"

void							traceLastFunc ( const char *szFunc );
void							Log ( const char *fmt, ... );
void							LogChatbox ( bool bLast, const char *fmt, ... );
void							setDebugPointer ( void *ptr );

// externals
extern HMODULE					g_hDllModule;
extern char						g_szWorkingDirectory[MAX_PATH];
extern FILE						*g_flLog;
extern FILE						*g_flLogAll;
extern char						g_szLastFunc[256];
extern uint32_t					g_dwSAMP_Addr;
extern CSettingsSAInterface		*g_pCSettingsSAInterface;
extern D3DPRESENT_PARAMETERS	*g_pGTAPresent;
extern RsGlobalType				*g_RsGlobal;
extern CCameraSA				g_CCamera;

// new MTA externals
extern CGameSA					*pGameInterface;
extern CPools					*pPools;
extern CPed						*pPedSelf;

// externals
#endif
