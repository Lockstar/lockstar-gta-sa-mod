/*

	PROJECT:		m0d_s0beit_sa
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:	Copyright 2007, 2008, 2009, 2010 we_sux

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

/////////////////////////////////////////
//                                     //
// m0d_s0beit_sa                       //
// for SA:MP                           //
//                                     //
// Read the included INI file in the   //
// bin directory for more information. //
//                                     //
/////////////////////////////////////////

#include "stdafx.h"

char		szWorkingDir[MAX_PATH];
HINSTANCE   hOrigDll = NULL;
HMODULE		hDllModule = NULL;
FILE		*pFileLog = NULL;
FILE		*pFileLogAll = NULL;
FILE		*pFileGamemode = NULL;
char		szLastFunction[256];
DWORD		dwSampBase;

typedef proxyIDirect3D9 *(WINAPI *D3DC9)(UINT);
typedef HRESULT (WINAPI *HRESULT_VOID)(void);
typedef HRESULT (WINAPI *HRESULT_BOOL)(BOOL);
proxyIDirect3D9            *pDirect3D9 = NULL;
D3DC9                      orig_Direct3DCreate9 = NULL;

void traceproc(const char *szLastFn) { _snprintf_s(szLastFunction,sizeof(szLastFunction),szLastFn); }

void log_debug(const char *fmt, ...)
{
   SYSTEMTIME time;
   va_list ap;

   if(pFileLog == NULL)
   {
      char filename[512];
      snprintf(filename, sizeof(filename), "%s\\%s", szWorkingDir, "m0d_s0beit_sa.log");
	  
	  remove(filename);
      pFileLog = fopen(filename, "a");
      if(pFileLog == NULL) return;
   }

   GetLocalTime(&time);
   fprintf(pFileLog, "[%02d:%02d:%02d.%02d] ", time.wHour, time.wMinute, time.wSecond,time.wMilliseconds);
   va_start(ap, fmt);
   vfprintf(pFileLog, fmt, ap);
   va_end(ap);
   fprintf(pFileLog, "\n");
   fflush(pFileLog);

   if(pFileLogAll == NULL)
   {
      char filename_all[512];
	  snprintf(filename_all, sizeof(filename_all), "%s\\%s", szWorkingDir, "m0d_s0beit_sa_all.log");

      pFileLogAll = fopen(filename_all, "a");
      if(pFileLogAll == NULL) return;
   }

   GetLocalTime(&time);
   fprintf(pFileLogAll, "[%02d-%02d-%02d || %02d:%02d:%02d.%02d] ", time.wDay, time.wMonth, time.wYear,
	   time.wHour, time.wMinute, time.wSecond,time.wMilliseconds);
   va_start(ap, fmt);
   vfprintf(pFileLogAll, fmt, ap);
   va_end(ap);
   fprintf(pFileLogAll, "\n");
   fflush(pFileLogAll);
}

void log_gamemode_info(const char *fmt, ...)
{
   va_list ap;

   if(pFileGamemode == NULL)
   {
      char filename[512];

      snprintf(filename, sizeof(filename), "%s\\%s", szWorkingDir, "gamemode_info.log");
	  
      pFileGamemode = fopen(filename, "a");
      if(pFileGamemode == NULL)
         return;
   }

   va_start(ap, fmt);
   vfprintf(pFileGamemode, fmt, ap);
   va_end(ap);

   //fprintf(gamemode_file, "\r\n");

   fflush(pFileGamemode);
}

void debug_ptr_set(void *ptr)
{
   struct debug_info *debug = &cheat_state->debug;

   if(debug->hist_pos < DEBUG_HIST_LEN)
   {
      debug->hist_pos++;
   }
   else
   {
      memmove(debug->ptr, debug->ptr + 1, sizeof(debug->ptr) - sizeof(debug->ptr[0]));
      memmove(debug->offset, debug->offset + 1, sizeof(debug->offset) - sizeof(debug->offset[0]));
   }

   debug->ptr[debug->hist_pos] = (uint8_t *)ptr;
   debug->offset[debug->hist_pos] = 0;
   debug->data_prev_clear = 1;
}

static int init(void)
{
   traceproc("init()");
   char filename[MAX_PATH];

   if(hOrigDll == NULL)
   {
      srand(GetTickCount());

      if(GetModuleFileName(hDllModule, szWorkingDir, sizeof(szWorkingDir) - 32) != 0)
      {
         if(strrchr(szWorkingDir, '\\') != NULL)
            *strrchr(szWorkingDir, '\\') = 0;
         else
            strcpy(szWorkingDir, ".");
      }
      else
      {
         strcpy(szWorkingDir, ".");
      }

	  log_debug("Initializing %s", NAME);

#pragma warning(disable:4127)

      if(sizeof(struct vehicle_info) != 2584)
      {
         log_debug("sizeof(struct vehicle_info) == %d, aborting.", sizeof(struct vehicle_info));
         return 0;
      }

      if(sizeof(struct actor_info) != 1988)
      {
         log_debug("sizeof(struct actor_info) == %d, aborting.", sizeof(struct actor_info));
         return 0;
      }

	  if(sizeof(struct settings) > 262144)
	  {
		  log_debug("sizeof(struct settings) > 262144 (%d), aborting.", sizeof(struct settings));
         return 0;
	  }
#pragma warning(default:4127)

	  getSamp();
	  
	  ini_load();

      if(!set.i_have_edited_the_ini_file)
      {
         MessageBox(0,
            "Looks like you've not edited the .ini file like you were told to!\n"
            "\n"
            "Before you can use m0d_s0beit, you have to set \"i_have_edited_the_ini_file\" to true.\n",
            "You're a retard.", 0);
         ShellExecute(0, "open", "notepad", INI_FILE, szWorkingDir, SW_SHOW);
         return 0;
      }

	  InitScripting();

	  GetSystemDirectory(filename, (UINT)(MAX_PATH - strlen("\\d3d9.dll") - 1));
      strlcat(filename, "\\d3d9.dll", sizeof(filename));

      //log_debug("Loading library: %s", filename);

      hOrigDll = LoadLibrary(filename);
      if(hOrigDll == NULL)
      {
         log_debug("Failed to load %s", filename);
         return 0;
      }

      orig_Direct3DCreate9 = (D3DC9)GetProcAddress(hOrigDll, "Direct3DCreate9");
      if(orig_Direct3DCreate9 == NULL)
      {
         log_debug("%s does not export Direct3DCreate9!?", filename);
         FreeLibrary(hOrigDll);
         return 0;
      }
	  //log_debug("Done.");
	}
	return 1;
}

IDirect3D9 *WINAPI sys_Direct3DCreate9(UINT SDKVersion)
{
   pDirect3D9 = NULL;

   if(init())
      pDirect3D9 = new proxyIDirect3D9(orig_Direct3DCreate9(SDKVersion));

   //log_debug("Direct3DCreate9() called. Returning 0x%p.", pDirect3D9);

   return pDirect3D9;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
#ifdef PROXY
	lpReserved = lpReserved;
#endif

   switch(ul_reason_for_call)
   {
   case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls(hModule);
      hDllModule = hModule;
      break;

   case DLL_PROCESS_DETACH:
      if(hOrigDll != NULL)
      {
         //log_debug("Detached from process. Freeing d3d9.dll...");
         FreeLibrary(hOrigDll);
         hOrigDll = NULL;
         //log_debug("Freeing memory...");
         menu_free_all();
         ini_free();
		 log_debug("Exited\n");
      }
      if(pFileLog != NULL)
      {
         //log_debug("Closing log file.");
         fclose(pFileLog);
         pFileLog = NULL;
      }
      if(pFileLogAll != NULL)
      {
         //log_debug("Closing log file.");
         fclose(pFileLogAll);
         pFileLogAll = NULL;
      }
      if(pFileGamemode != NULL)
      {
         //log_debug("Closing gamemode file.");
         fclose(pFileGamemode);
         pFileGamemode = NULL;
      }

      break;
   }

   return TRUE;
}
