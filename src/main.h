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
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/main.h $
	$Id: main.h 43 2010-01-05 07:40:59Z futnucks $

*/

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS 1

// main switch for development or release build
#define M0D_DEV

#define M0D_NAME "m0d_s0beit_sa"
#define M0D_VERSION "v4.1.0.0"
#define SAMP_VERSION "SA:MP 0.3a"

#ifdef M0D_DEV
  #define NAME "" M0D_NAME "-dev" " " M0D_VERSION " (" __DATE__" " "" __TIME__")"
 #else
  #define NAME "" M0D_NAME "" " " M0D_VERSION ""
#endif

#define INI_FILE "m0d_s0beit_sa.ini"
#define RAW_TEX_FILE "m0d_s0beit_sa.raw"

#define RUNMODE_SINGLEPLAYER	0
#define RUNMODE_SAMP			1


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <shellapi.h>
#include <d3dx9.h>

#include "stdint.h"
#include "patcher.h"
#include "cheat.h"
#include "ini.h"
#include "keyhook.h"
#include "math_stuff.h"
#include "cheat_funcs.h"
#include "cheat_generic.h"
#include "cheat_actor.h"
#include "cheat_vehicle.h"
#include "dumb_menu.h"
#include "samp.h"
#include "debug_classify.h"
#include "scripting.h"
#include "CDetour.h"
#include "d3drender.h"
#include "Gdiplus.h"
#include "GTAfuncs.h"
#include "proxyIDirect3D9.h"
#include "proxyIDirect3DDevice9.h"



/* warning C4100: 'foo' : unreferenced formal parameter */
#pragma warning(disable:4100)

#define snprintf  _snprintf
#define vsnprintf _vsnprintf
#define isfinite  _finite

void traceLastFunc(const char *szFunc);
void Log(const char *fmt, ...);
void LogChatbox(bool bLast, const char *fmt, ...);
void setDebugPointer(void *ptr);

// externals
extern HMODULE g_hDllModule;
extern char	g_szWorkingDirectory[MAX_PATH];
extern FILE	*g_flLog;
extern FILE	*g_flLogAll;
extern char	g_szLastFunc[256];
extern uint32_t	g_dwSAMP_Addr;
extern CSettingsSAInterface *g_pCSettingsSAInterface;
extern D3DPRESENT_PARAMETERS *g_pGTAPresent;
extern RsGlobalType *g_RsGlobal;
// externals
