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
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/proxyIDirect3DDevice9.cpp $
	$Id: proxyIDirect3DDevice9.cpp 43 2010-01-05 07:40:59Z futnucks $

*/

#include "main.h"
#include "resource.h"

// externals
unsigned long ulFullScreenRefreshRate;
extern D3DC9 orig_Direct3DCreate9 = NULL;;
// externals


#pragma data_seg(".d3d9_shared")
proxyIDirect3DDevice9	*pDirect3DDevice9 = NULL;
HRESULT_VOID			orig_Direct3DShaderValidatorCreate9 = NULL;
IDirect3DDevice9		*origIDirect3DDevice9;
IDirect3DDevice9		*pRwD3DDevice = (IDirect3DDevice9*)*(DWORD*)__RwD3DDevice;
IDirect3DTexture9		*pSpriteTexture;
D3DPRESENT_PARAMETERS	pPresentParam;
D3DMATRIX				m_mViewMatrix, m_mProjMatrix, m_mWorldMatrix;
#pragma data_seg()





bool bD3DRenderInit;
bool bD3DWindowModeSet;
bool g_isRequestingWindowModeToggle;
bool g_isRequesting_RwD3D9ChangeVideoMode;
float fps;

IDirect3DTexture9* tLoadingLogo;
ID3DXSprite* sLoadingLogo;
D3DXVECTOR3 posLoadingLogo;

D3DXVECTOR3 speedoPos;
D3DXVECTOR2 needlePos;

IDirect3DTexture9* tSpeedoPNG;
ID3DXSprite* sSpeedoPNG;
IDirect3DTexture9* tNeedlePNG;
ID3DXSprite* sNeedlePNG;

CD3DRender *render				= new CD3DRender(128);
CD3DFont *pD3DFont				= new CD3DFont("Verdana",		 10, FCR_NONE);
CD3DFont *pD3DFontFixed			= new CD3DFont("Lucida Console", 8, FCR_NONE);
CD3DFont *pD3DFontSmall			= new CD3DFont("Arial",			 8, FW_BOLD);
CD3DFont *pD3DFontFixedSmall	= new CD3DFont("Small Fonts",	 6, FCR_NONE);
CD3DFont *pD3DFontChat			= new CD3DFont("Tahoma",		 10, FCR_NONE);

#define MENU_ROWS  12
#define MENU_WIDTH 400

struct gui *hud_bar						= &set.guiset[0];
struct gui *hud_bar_specwarn			= &set.guiset[1];
struct gui *menu_titlebar_background	= &set.guiset[3];
struct gui *menu_background				= &set.guiset[4];
struct gui *menu_selected_item_bar		= &set.guiset[5];
struct gui *menu_selected_item_text		= &set.guiset[6];

struct gui *gta_hp_bar = &set.guiset[7];
struct gui *gta_money_hud = &set.guiset[8];




///////////////////////////////////////////////////////////////////////////////
// Common D3D functions.
///////////////////////////////////////////////////////////////////////////////


// new screenshot function
bool isRequestingScreenshot; // this gets referenced as an external in cheat.cpp
bool isPornographyMasterControlRunning = false;
bool isPornographyStuntCockReady = false;
DWORD g_lastPornographyTickCount = GetTickCount();

static WCHAR* ToWChar(char * str) 
{
	static WCHAR buffer[1024];
	_wcsset(buffer,0);
	MultiByteToWideChar(CP_ACP,0,str,strlen(str),buffer,1024);
	return buffer;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) 
{
	using namespace Gdiplus;
	UINT  num = 0;          // number of image encoders 
	UINT  size = 0;         // size of the image encoder array in bytes 
	ImageCodecInfo* pImageCodecInfo = NULL; 
	GetImageEncodersSize(&num, &size); 
	if(size == 0) 
		return -1;  // Failure 
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size)); 
	if(pImageCodecInfo == NULL) 
		return -1;  // Failure 
	GetImageEncoders(num, size, pImageCodecInfo); 
	for(UINT j = 0; j < num; ++j) 
	{ 
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) 
		{ 
			*pClsid = pImageCodecInfo[j].Clsid; 
			free(pImageCodecInfo); 
			return j;  // Success 
		}     
	} 
	free(pImageCodecInfo); 
	return -1;  // Failure 
}

HBITMAP PornographyGetPorn(void)
{
	// RenderTargetSurface.
	IDirect3DSurface9* pRenderTargetSurface = NULL;
	// DestinationTargetSurface
	IDirect3DSurface9* pDestinationTargetSurface = NULL;
	// DisplayMode
	D3DDISPLAYMODE d3dDipMode;
	// localize hWnd
	HWND m_hWnd = pPresentParam.hDeviceWindow;

	// make sure we have the original device
	if (origIDirect3DDevice9 == NULL) goto getpornfail;

	// Get the client rectangle
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	ClientToScreen(m_hWnd, LPPOINT(&rc.left));
	ClientToScreen(m_hWnd, LPPOINT(&rc.right));

	// Display Mode (d3dDipMode)
	if(FAILED(origIDirect3DDevice9->GetDisplayMode(D3DADAPTER_DEFAULT, &d3dDipMode))) goto getpornfail;

	// calculate correct D3DFormat
	D3DFORMAT m_D3DFMT;
	uint8_t m_CaptureBitCount;
	if (d3dDipMode.Format == D3DFMT_X8R8G8B8)
	{
		// 32bit
		m_D3DFMT = D3DFMT_A8R8G8B8;
		m_CaptureBitCount = 32;
	}
	else if (d3dDipMode.Format == D3DFMT_R5G6B5)
	{
		// 16bit
		m_D3DFMT = D3DFMT_R5G6B5;
		m_CaptureBitCount = 16;
	}
	else
	{
		// pray this works, but we shouldn't ever get this
		m_D3DFMT = D3DFMT_A8R8G8B8;
		m_CaptureBitCount = 32;
	}

	//GetDestinationTargetSurface
	if(FAILED(origIDirect3DDevice9->CreateOffscreenPlainSurface(
		(rc.right - rc.left),
		(rc.bottom - rc.top),
		m_D3DFMT,
		D3DPOOL_SYSTEMMEM,
		&pDestinationTargetSurface,
		NULL)))
		goto getpornfail;

	// GetRenderTargetSurface ASAP
	if(FAILED(origIDirect3DDevice9->GetRenderTarget(0, &pRenderTargetSurface))) goto getpornfail;

	//copy RenderTargetSurface -> DestTarget, and release pRenderTargetSurface
	if(FAILED(origIDirect3DDevice9->GetRenderTargetData(pRenderTargetSurface, pDestinationTargetSurface))) goto getpornfail;
	SAFE_RELEASE(pRenderTargetSurface);

	// create HDC device
	HDC hCaptureDC = CreateCompatibleDC(NULL);

	//Create a BITMAPINFO/BITMAPINFOHEADER structure and fill it(parameter 2 for CreateDIBSection())
	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biBitCount = m_CaptureBitCount;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biWidth = rc.right - rc.left;
	bmpInfo.bmiHeader.biHeight = rc.bottom - rc.top;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	//Create a lock on the DestinationTargetSurface
	D3DLOCKED_RECT lockedRC;
	if(FAILED(pDestinationTargetSurface->LockRect(
		&lockedRC,
		NULL,
		D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_READONLY|D3DLOCK_NOSYSLOCK)))
		goto getpornfail;

	// create the HBITMAP we'll return and populate it with lockedRC.pBits
	HBITMAP hbm = CreateBitmap(
		bmpInfo.bmiHeader.biWidth,
		bmpInfo.bmiHeader.biHeight,
		1, // planes
		m_CaptureBitCount, // bits per pixel
		lockedRC.pBits // pointer to bitmap
	);

	// unlock & release the RECT
	if(FAILED(pDestinationTargetSurface->UnlockRect())) goto getpornfail;
	SAFE_RELEASE(pDestinationTargetSurface);

	// select the HBITMAP into the HDC, converting format to A8R8G8B8 if needed
	SelectObject(hCaptureDC, hbm);

	// release the leftovers
	DeleteDC(hCaptureDC);

	// return the 32bit HBITMAP
	return hbm;

getpornfail:
	// release everything
	DeleteObject(hbm);
	DeleteDC(hCaptureDC);
	SAFE_RELEASE(pRenderTargetSurface);
	SAFE_RELEASE(pDestinationTargetSurface);
	return 0;
}

DWORD WINAPI PornographyMasterControl(LPVOID trash)
{
	// trigger states and wait for the next render pass
	isPornographyMasterControlRunning = true;

	// first let's make sure we can init GDI
	using namespace Gdiplus;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	if (Ok != GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL))
	{
		addMessageToChatWindowSS("Could not take a screenshot.  GDI+ FAIL!");
		isPornographyMasterControlRunning = false;
		ExitThread(0);
	}

	// let's get some variables for GDI...
	CLSID encoderClsid;
	EncoderParameters encoderParameters;
	ULONG encoderValue;

	// STUNT COCK!
	while (!isPornographyStuntCockReady) Sleep(100);

	// get some porn
	HBITMAP g0tP0rn = PornographyGetPorn();
	if (g0tP0rn == 0)
	{
		addMessageToChatWindowSS("Could not take a screenshot.  D3D FAIL!");
		goto fail1;
	}
	Bitmap* hardcorePorn = Bitmap::FromHBITMAP(g0tP0rn, NULL);

	GetEncoderClsid(L"image/jpeg", &encoderClsid);

	// encoder parameters
	encoderParameters.Count = 3;

	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderValue = set.jpg_quality;
	encoderParameters.Parameter[0].Value = &encoderValue;

	encoderParameters.Parameter[1].Guid = EncoderChrominanceTable;
	encoderParameters.Parameter[1].Type = EncoderParameterValueTypeShort;
	encoderParameters.Parameter[1].NumberOfValues = 1;
	encoderValue = set.jpg_chrominancetable;
	encoderParameters.Parameter[1].Value = &encoderValue;

	encoderParameters.Parameter[2].Guid = EncoderLuminanceTable;
	encoderParameters.Parameter[2].Type = EncoderParameterValueTypeShort;
	encoderParameters.Parameter[2].NumberOfValues = 1;
	encoderValue = set.jpg_luminancetable;
	encoderParameters.Parameter[2].Value = &encoderValue;

	// let's save the bastard
	char m_PornoName[256];
	SYSTEMTIME m_systemTime;
	GetLocalTime(&m_systemTime);

	if(g_SAMP != NULL)
	{
		sprintf(m_PornoName, "screenshots\\sa-mp_%04d-%02d-%02d_%02d-%02d-%02d.jpg",
			m_systemTime.wYear, m_systemTime.wMonth, m_systemTime.wDay,
			m_systemTime.wHour, m_systemTime.wMinute, m_systemTime.wSecond);
	}
	else
	{
		sprintf(m_PornoName, "screenshots\\gta-sa_%04d-%02d-%02d_%02d-%02d-%02d.jpg",
			m_systemTime.wYear, m_systemTime.wMonth, m_systemTime.wDay,
			m_systemTime.wHour, m_systemTime.wMinute, m_systemTime.wSecond);
	}

	if (Ok == hardcorePorn->Save(ToWChar(m_PornoName), &encoderClsid, &encoderParameters))
	{
		addMessageToChatWindowSS("Screenshot saved to: %s", m_PornoName);
	} else {
		addMessageToChatWindowSS("Could not take a screenshot.  Save FAIL!");
	};

	// clean up after the bastards
	DeleteObject(hardcorePorn);
	DeleteObject(g0tP0rn);
	GdiplusShutdown(gdiplusToken);

	// we're done ere innit'
	isPornographyMasterControlRunning = false;
	isPornographyStuntCockReady = false;
	g_lastPornographyTickCount = GetTickCount();
	ExitThread(0);
fail1:
	DeleteObject(g0tP0rn);
	GdiplusShutdown(gdiplusToken);
	isPornographyMasterControlRunning = false;
	isPornographyStuntCockReady = false;
	ExitThread(0);
}

bool Pornography()
{
	// hello to you Mr Kye.  you know this isn't public domain code
	// but we know you'll end up copy-pasting this anyways.
	// so enjoy.
	//

	// launch the worker bees
	HANDLE m_hPornoThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)PornographyMasterControl,
		NULL,
		0,
		NULL
	);

	if (m_hPornoThread) return true;
	return false;
}






void LoadSpriteTexture(void)
{
   char filename[MAX_PATH];
   D3DLOCKED_RECT d3dlr;    
   FILE *fd = NULL;
   int x, y;
   uint8_t *surface;

   SAFE_RELEASE(pSpriteTexture);
   pSpriteTexture = NULL;

   /* XXX use a different texture format */
   if(FAILED(origIDirect3DDevice9->CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pSpriteTexture, NULL)))
   {
      Log("Failed to create sprite texture!");
      goto out;
   }

   snprintf(filename, sizeof(filename), "%s\\%s", g_szWorkingDirectory, RAW_TEX_FILE);
   if((fd = fopen(filename, "rb")) == NULL)
   {
      Log("Failed to open %s!", filename);
      goto out;
   }

   if(FAILED(pSpriteTexture->LockRect(0, &d3dlr, 0, 0)))
   {
      Log("Failed to lock texture!");
      goto out;
   }

   surface = (uint8_t *)d3dlr.pBits;

   for(y=0; y<120; y++)
   {
      uint8_t data[240];
	  fread(data, 240, 1, fd);

      for(x=0; x<240; x++)
      {
         uint8_t alpha = (data[x] & 7) << 5;
         uint8_t value = data[x] & ~7;

         surface[x*4+0] = value;
         surface[x*4+1] = value;
         surface[x*4+2] = value;
         surface[x*4+3] = alpha;
      }

      surface += d3dlr.Pitch;
   }

   pSpriteTexture->UnlockRect(0);

   fclose(fd);
   //Log("Sprite texture loaded.");

   return;

out:;
   SAFE_RELEASE(pSpriteTexture)
   if(fd != NULL)
      fclose(fd);
   return;
}

static void mmm_yummy_poop(const void *info, int *enabled, int *prev_enabled, int *render, const char *teh_name)
{
   if(info == NULL)
   {
      *prev_enabled = -1;
      *render = 0;
   }
   else
   {
      if(*prev_enabled == -1)
      {
         *prev_enabled = *enabled;
         *render = !*enabled;
      }

      if(!*prev_enabled && *enabled && *render)
      {
         *enabled = 0;
         *prev_enabled = 0;
         *render = 0;
         cheat_state_text("Disabled %s", teh_name);
      }
      else if(*prev_enabled && !*enabled)
      {
         cheat_state_text("Switched to s0beit %s", teh_name);
         *render = 1;
      }
      else if(!*prev_enabled && *enabled)
      {
         cheat_state_text("Switched to SA:MP %s", teh_name);
         *render = 0;
      }
      *prev_enabled = *enabled;
   }
}

void RenderDebug(void)
{
   static const int ROW_HEIGHT = (int)ceilf(pD3DFontFixed->DrawHeight());
   static const int CHAR_WIDTH  = (int)ceilf(pD3DFontFixed->DrawLength("W"));
   static const int ROWS = DEBUG_DATA_SIZE / 16;
   static const int data_size[4] = { 1, 2, 4, 4 };
   struct debug_info *debug = &cheat_state->debug;
   static char str_data[17], str[256];
   int x, y, row, col, i;
   int offset, offset16;
   uint8_t *ptr, *data;


   ptr = debug->ptr[debug->hist_pos] - DEBUG_DATA_SIZE;
   offset = debug->offset[debug->hist_pos];
   offset16 = (unsigned int)offset % 16;
   ptr += offset - offset16 + DEBUG_DATA_SIZE / 2;

   memset(debug->data, 0, DEBUG_DATA_SIZE);
   memcpy_safe(debug->data, ptr, DEBUG_DATA_SIZE);

   if(!debug->data_prev_clear)
   {
      for(i=0; i<DEBUG_DATA_SIZE; i++)
      {
         if(debug->data[i] != debug->data_prev[i])
            debug->modify_time[i] = time_get();
      }
   }
   else
   {
      memset(debug->modify_time, 0, sizeof(debug->modify_time));
   }
   debug->data_prev_clear = 0;

   x = 324;
   y = 4;

   /* blue box */
   render->D3DBoxi(x - 2, y - 2, CHAR_WIDTH * 75 + 4, ROW_HEIGHT * 27 + 4, D3DCOLOR_ARGB(180, 0, 0, 150),NULL);

   /* render selection */
   for(row=offset16; row<offset16+data_size[debug->data_type]; row++)
   {
      col = (ROWS / 2) + row / 16;
      render->D3DBoxi(
         x + CHAR_WIDTH * (10 + (row % 16) * 3), y + ROW_HEIGHT * col,
         CHAR_WIDTH * 3, ROW_HEIGHT, D3DCOLOR_ARGB(127, 0, 255, 0),NULL);
      render->D3DBoxi(
         x + CHAR_WIDTH * (59 + (row % 16)), y + ROW_HEIGHT * col,
         CHAR_WIDTH, ROW_HEIGHT, D3DCOLOR_ARGB(127, 0, 255, 0),NULL);
   }

   /* render hex view */
   for(row=0; row<ROWS; row++)
   {
      int red_text = 0;
      int len = 10;
      int xpos = x;

      snprintf(str, sizeof(str), "%p  ", ptr + row * 16);

      for(col=0; col<16; col++)
      {
         uint32_t mod_time = debug->modify_time[row*16+col];
         uint8_t ch = debug->data[row*16+col];
         int color_switch = 0;


         if(mod_time != 0 && time_get() < mod_time + MSEC_TO_TIME(2000))
            color_switch = !red_text;  /* switch color, unless red_text == 1 */
         else if(red_text)
            color_switch = 1;

         if(color_switch)
         {
            pD3DFontFixed->PrintShadow((float)xpos, (float)y,
               red_text ? D3DCOLOR_XRGB(255, 31, 31) : D3DCOLOR_XRGB(191, 191, 191),
               str);

            xpos += CHAR_WIDTH * len;
            str[0] = 0;
            len = 0;

            red_text ^= 1;
         }

         snprintf(str + len, sizeof(str) - len, "%02x ", debug->data[row*16+col]);
         len += 3;

         str_data[col] = (char)((ch < 32) ? '.' : ch);
      }

      pD3DFontFixed->PrintShadow((float)xpos, (float)y,
         red_text ? D3DCOLOR_XRGB(255, 31, 31) : D3DCOLOR_XRGB(191, 191, 191),
         str);

      pD3DFontFixed->PrintShadow((float)x + CHAR_WIDTH * 59, (float)y, D3DCOLOR_XRGB(191, 191, 191), str_data);

      y += ROW_HEIGHT;
   }


   /* everything else \o/ */
   data = &debug->data[DEBUG_DATA_SIZE / 2 + offset16];
   memcpy(debug->cursor_data, data, sizeof(debug->cursor_data));

   /* data type info */
   snprintf(str, sizeof(str), "=>%c%02x(%-3d),%c%04x(%-5d),%c%08x(%d),%c%.4f, %s ptr",
      (debug->data_type == 0) ? '*' : ' ', *(uint8_t  *)data, *(uint8_t  *)data,
      (debug->data_type == 1) ? '*' : ' ', *(uint16_t *)data, *(uint16_t *)data,
      (debug->data_type == 2) ? '*' : ' ', *(uint32_t *)data, *(uint32_t *)data,
      (debug->data_type == 3) ? '*' : ' ', *(float *)data,
      isBadPtr_readAny(*(void **)data, 1) ? "Bad" : "OK");
   pD3DFontFixed->PrintShadow((float)x, (float)y, D3DCOLOR_XRGB(191, 191, 191), str);
   y += ROW_HEIGHT;

   /* ptr info */
   snprintf(str, sizeof(str), "Viewing: %s", debug_classify_pointer((void *)(debug->ptr[debug->hist_pos] + offset)));
   pD3DFontFixed->PrintShadow((float)x, (float)y, D3DCOLOR_XRGB(191, 191, 191), str);
   y += ROW_HEIGHT;

   snprintf(str, sizeof(str), "Pointer: %s", debug_classify_pointer(*(void **)data));
   pD3DFontFixed->PrintShadow((float)x, (float)y, D3DCOLOR_XRGB(191, 191, 191), str);
   y += ROW_HEIGHT;

   /* ptr stack info */
   snprintf(debug->ptr_hist_str, sizeof(debug->ptr_hist_str),
      "Ptr stack (%d): %s", debug->hist_pos, debug->hist_pos > 2 ? "... " : "");
   for(i=debug->hist_pos-2; i<=debug->hist_pos; i++)
   {
      int len = (int)strlen(debug->ptr_hist_str);

      if(i < 0)
         continue;
      snprintf(debug->ptr_hist_str + len, sizeof(debug->ptr_hist_str) - len, "%p+0x%x -> ",
         debug->ptr[i], debug->offset[i]);
   }
   debug->ptr_hist_str[strlen(debug->ptr_hist_str) - 4] = 0;
   pD3DFontFixed->PrintShadow((float)x, (float)y, D3DCOLOR_XRGB(191, 191, 191), debug->ptr_hist_str);
   y += ROW_HEIGHT * 2;

   /* print help */
   pD3DFontFixed->PrintShadow((float)x, (float)y, D3DCOLOR_XRGB(191, 191, 191),
      "np4/6,8/2,9/3 = move   np7 = return   np1 = go to pointer");
   y += ROW_HEIGHT;

   pD3DFontFixed->PrintShadow((float)x, (float)y, D3DCOLOR_XRGB(191, 191, 191),
      "np/ = data type   np-/+ = inc/dec   np* = ptr stack to clipboard");

   memcpy(debug->data_prev, debug->data, DEBUG_DATA_SIZE);
}

void RenderMenu(void)
{
   static const int ROW_HEIGHT = (int)ceilf(pD3DFont->DrawHeight());
   static const int MENU_HEIGHT = (int)ceilf(pD3DFont->DrawHeight() * (float)MENU_ROWS) + 2;

   char title[256];
   struct menu_item *item;
   struct menu *menu;
   int left, top;
   float x, y;
   int pos, top_pos, i;

   if(menu_active == NULL)
      return;

   /* find root menu */
   for(menu=menu_active; menu->parent!=NULL; menu=menu->parent);

   strlcpy(title, NAME, sizeof(title));

   while(menu != NULL)
   {
      if(menu->pos < 0 || menu->pos >= menu->count)
         break;
      item = &menu->item[menu->pos];
      if(item->submenu == NULL || menu == menu_active)
         break;
      snprintf(title + strlen(title), sizeof(title) - strlen(title), " > %s", item->name);
      menu = item->submenu;
   }

   /* draw titlebar */
   left = pPresentParam.BackBufferWidth / 2 - MENU_WIDTH / 2;
   top = pPresentParam.BackBufferHeight - MENU_HEIGHT - ROW_HEIGHT - 2 - 20 - 2;

   render->D3DBoxBorderi(left, top, (int)ceilf(pD3DFont->DrawLength(title)) + 2, ROW_HEIGHT + 2,
      D3DCOLOR_ARGB(223, 91, 91, 91), D3DCOLOR_ARGB(menu_titlebar_background->alpha,
	  menu_titlebar_background->red, menu_titlebar_background->green, menu_titlebar_background->blue) );
   pD3DFont->PrintShadow((float)(left + 1), (float)(top + 1), D3DCOLOR_XRGB(223, 223, 223), title);

   /* draw window */
   left = pPresentParam.BackBufferWidth  / 2 - MENU_WIDTH / 2;
   top = pPresentParam.BackBufferHeight - MENU_HEIGHT - 1 - 20;
   render->D3DBoxBorderi(left, top, MENU_WIDTH, MENU_HEIGHT,
      D3DCOLOR_ARGB(223, 63, 63, 63), D3DCOLOR_ARGB(menu_background->alpha, menu_background->red,
	  menu_background->green, menu_background->blue) );

   /* calculate positions */
   pos = menu_active->pos;
   top_pos = menu_active->top_pos;

   if(pos - MENU_ROWS >= top_pos)
      top_pos = pos - MENU_ROWS + 1;
   if(pos < top_pos)
      top_pos = pos;

   if(top_pos < 0)
      top_pos = 0;
   else if(top_pos >= menu_active->count)
      top_pos = menu_active->count - 1;

   menu_active->top_pos = top_pos;

   /* draw menu items */
   y = (float)top;
   x = (float)(left + 2);

   for(i=top_pos; i<top_pos+MENU_ROWS; i++,y+=pD3DFont->DrawHeight())
   {
      int enabled;

      if(i < 0 || i >= menu_active->count)
         continue;

      item = &menu_active->item[i];
      enabled = menu_active->callback(MENU_OP_ENABLED, item);

      if(i == pos)
      {
         render->D3DBoxi(
            left + 1, (int)floorf(y) + 1,
            MENU_WIDTH - 1, ROW_HEIGHT + 1,
            D3DCOLOR_ARGB(menu_selected_item_bar->alpha, menu_selected_item_bar->red, menu_selected_item_bar->green,
			menu_selected_item_bar->blue), NULL);
      }

      if(item->name[0] == '\t')
      {
         pD3DFont->PrintShadow(x, y, item->color, item->name + 1);
         render->D3DBoxi(left + 2, (int)floorf(y) + ROW_HEIGHT - 1,
            (int)ceilf(pD3DFont->DrawLength(item->name + 1)), 1,
            D3DCOLOR_ARGB(191, 127, 127, 127),NULL);
      }
      else
      {
         /* XXX ... */
         pD3DFont->PrintShadow(x, y, enabled ? D3DCOLOR_XRGB(
			 menu_selected_item_text->red, menu_selected_item_text->green, menu_selected_item_text->blue) : item->color, item->name);
      }

      if(item->submenu != NULL)
      {
         pD3DFont->PrintShadow(
            x + (float)MENU_WIDTH - pD3DFont->DrawLength(">") - 4, y,
            D3DCOLOR_ARGB(191, 127, 127, 127), ">");
      }
   }
}


void RenderMapDot(const float self_pos[3], const float pos[16], DWORD color, const char *name)
{
   static int init;
   float vect[3], rvect[2];
   float a, x, y;

   vect3_vect3_sub(pos, self_pos, vect);

   if(vect3_length(vect) > 1000.0f)
      return;

   color = (color & 0x00FFFFFF) | 0xDF000000;

   a = -atan2f(cam_matrix[4*0+0], cam_matrix[4*0+1]) - M_PI / 2.0f;

   rvect[0] = vect[0] * cosf(a) - -vect[1] * sinf(a);
   rvect[1] = vect[0] * sinf(a) + -vect[1] * cosf(a);

   rvect[1] /= pPresentParam.BackBufferWidth / pPresentParam.BackBufferHeight;

   x = (float)pPresentParam.BackBufferWidth  / 2 + roundf(rvect[0]);
   y = (float)pPresentParam.BackBufferHeight / 2 + roundf(rvect[1]);

   /* the "^ v o" icons are 9x9 large, and located at 168x98 in the texture */

   if(pSpriteTexture != NULL)
   {
	   float u, v, ts;

	   if(name != NULL)
		   pD3DFontFixed->PrintShadow(x - pD3DFontFixed->DrawLength(name) / 2.0f, y - 14.0f, color, name);

	   u = 168.0f;
	   v = 98.0f;

	   if(vect[2] < -4.0f)
		  u += 9.0f;
	   else if(vect[2] <= 4.0f)
		  u += 18.0f;

	   u /= 256.0f;
	   v /= 256.0f;
	   ts = 9.0f / 256.0f;

	   render->D3DBindTexture(pSpriteTexture);

	   render->D3DTexQuad(
		  x - 4.5f, y - 4.5f, x + 4.5f, y + 4.5f,
		  u, v, u + ts, v + ts);

	   render->D3DBindTexture(NULL);
   }
   else
   {
	   if(name != NULL)
		   pD3DFont->PrintShadow(x - pD3DFontFixed->DrawLength(name) / 2.0f, y - 14.0f, color, name);

	   if(vect[2] < -4.0f)
	   {
		   pD3DFont->PrintShadow(x - 4.5f, y - 4.5f, color, "v");
		   return;
	   }
	   else if(vect[2] <= 4.0f)
	   {
		   pD3DFont->PrintShadow(x - 4.5f, y - 4.5f, color, "o");
		   return;
	   }
	   else if(vect[2] > 4.0f)
	   {
		   pD3DFont->PrintShadow(x - 4.5f, y - 4.5f, color, "^");
		   return;
	   }
   }
}

int iShowVehicles;
void RenderMap(void)
{
	struct actor_info *self = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);
	if(self == NULL) return;
	const struct vehicle_entry *vehicle;
	float pos[3];
	char buf[256];
	int i;

   if(pool_actor != NULL)
   {
	   if(g_dwSAMP_Addr != NULL)
	   {
		   for(i=0; i<SAMP_PLAYER_MAX; i++)
		   {
			   if(!getPlayerPos(i, pos)) continue;
			   if(g_Players->pRemotePlayer[i] == NULL) continue;
			   _snprintf_s(buf,sizeof(buf),"%s(%d)", getPlayerName(i), i);
			   RenderMapDot(&self->base.matrix[4*3], pos, samp_color_get(g_Players->pRemotePlayer[i]->sPlayerID), buf);
		   }
	   }
	   else
	   {
		  for(i=0; i<pool_actor->size; i++)
		  {
			  struct actor_info *info = actor_info_get(i, ACTOR_ALIVE);
			  if(info == NULL) continue;
			  _snprintf_s(buf,sizeof(buf),"%d", i);
			  RenderMapDot(&self->base.matrix[4*3], &info->base.matrix[4*3], D3DCOLOR_XRGB(255,255,255), buf);
		  }
	   }
   }

	if(iShowVehicles)
	{
	   if(g_dwSAMP_Addr != NULL)
	   {
		   if(g_Vehicles != NULL)
		   {
			   for(i = 0; i < SAMP_VEHICLE_MAX; i++)
			   {
					if(g_Vehicles->iIsListed[i] != 1) continue;
					if(g_Vehicles->pSAMP_Vehicle[i] == NULL) continue;
					if(g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle != NULL)
					{
						vehicle = gta_vehicle_get_by_id(g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle->base.model_alt_id);
						if(g_Players->pLocalPlayer->sCurrentVehicleID == i) continue;

						_snprintf_s(buf,sizeof(buf),"%s(%d)", vehicle->name, i);
						RenderMapDot(&self->base.matrix[4*3], 
							&g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle->base.matrix[4*3], D3DCOLOR_XRGB(150,150,150), buf);

					}
			   }
		   }
	   }
	   else
	   {
		   for(i = 0; i < pool_vehicle->size; i++)
		   {
			   struct vehicle_info *vehs = vehicle_info_get(i, VEHICLE_ALIVE);
			   if(vehs == NULL) continue;
			   vehicle = gta_vehicle_get_by_id(vehs->base.model_alt_id);
			   _snprintf_s(buf,sizeof(buf),"%s (%d)", vehicle->name, i);
			   RenderMapDot(&self->base.matrix[4*3], &vehs->base.matrix[4*3], D3DCOLOR_XRGB(150,150,150), buf);
		   }
	   } 
   }
   
   if(cheat_state->_generic.teletext)
   {
	   for(i = 0; i < TELEPORT_MAX; i++)
	   {
		   vect3_copy(&cheat_state->teleport[i].matrix[4*3], pos);
		   if(vect3_near_zero(pos)) continue;
		   _snprintf_s(buf,sizeof(buf),"Teleport %d", i);
		   RenderMapDot(&self->base.matrix[4*3], pos, D3DCOLOR_XRGB(0, 200, 200), buf);
	   }
   }

   // self
   RenderMapDot(&self->base.matrix[4*3], &self->base.matrix[4*3], D3DCOLOR_XRGB(255,255,255), NULL);
}

void RenderPedHPBar(void)
{
	if(!set.left_bottom_bars_enable) return;

   struct actor_info *info = actor_info_get(ACTOR_SELF, 0);
   char text[32];
   int bottom;

   if(info == NULL) return;

   bottom = pPresentParam.BackBufferHeight;

   render->D3DBoxi(0, bottom - 10, 101, 10, D3DCOLOR_ARGB(127, 0, 0, 0),NULL);
   render->D3DBoxi(0, bottom - 9, (int)info->hitpoints, 8, D3DCOLOR_ARGB(127, 191, 0, 0), 100);
   _snprintf_s(text, sizeof(text), "Health: %d", (int)info->hitpoints);
   pD3DFontFixed->PrintShadow((float)(2), (float)(bottom - 9), D3DCOLOR_XRGB(255, 255, 255), text);

   render->D3DBoxi(0, bottom - 20, 101, 10, D3DCOLOR_ARGB(127, 0, 0, 0),NULL);
   if(info->armor == NULL)
   {
	   _snprintf_s(text, sizeof(text), "No armor");
	   pD3DFontFixed->PrintShadow((float)(2), (float)(bottom - 19), D3DCOLOR_XRGB(255, 255, 255), text);
   }
   else
   {
	   render->D3DBoxi(0, bottom - 19, (int)info->armor, 8, D3DCOLOR_ARGB(127, 255, 255, 255 ), 100);
	   _snprintf_s(text, sizeof(text), "Armor: %d", (int)info->armor);
	   pD3DFontFixed->PrintShadow((float)(2), (float)(bottom - 20), D3DCOLOR_XRGB(255, 255, 255), text);   
   }
}

void RenderVehicleHPBar(void)
{
	if(!set.left_bottom_bars_enable) return;

   struct actor_info *pinfo = actor_info_get(ACTOR_SELF, 0);
   struct vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
   char text[32];
   int hp, bottom;
   float speed;

   if(vinfo == NULL) return;
   if(pinfo == NULL) return;

   bottom = pPresentParam.BackBufferHeight;
   speed = vect3_length(vinfo->speed);
   if(speed > 260.0f) speed = 260.0f;

   if(vinfo->hitpoints > 1000.0f)
      hp = 100;
   else
      hp = (int)(vinfo->hitpoints / 10.0f);

   render->D3DBoxi(0, bottom - 10, 101, 10, D3DCOLOR_ARGB(127, 0, 0, 0),NULL);
   render->D3DBoxi(0, bottom - 9, hp, 8, D3DCOLOR_ARGB(127, 191, 0, 0), 1000);
   _snprintf_s(text, sizeof(text), "VHealth: %d", (int)vinfo->hitpoints);
   pD3DFontFixed->PrintShadow((float)(2), (float)(bottom - 9), D3DCOLOR_XRGB(255, 255, 255), text);

   if(!set.speedometer_old_enable)
   {
	   render->D3DBoxi(0, bottom - 20, 101, 10, D3DCOLOR_ARGB(127, 0, 0, 0),NULL);
	   render->D3DBoxi(0, bottom - 19, (int)pinfo->hitpoints, 8, D3DCOLOR_ARGB(127, 191, 0, 0), 100);
	   if(pinfo->armor != NULL)
		   render->D3DBoxi(0, bottom - 19, (int)pinfo->armor, 8, D3DCOLOR_ARGB(127, 255, 255, 255 ), 100);
	   _snprintf_s(text, sizeof(text), "PHealth: %d", (int)pinfo->hitpoints);
	   pD3DFontFixed->PrintShadow((float)(2), (float)(bottom - 20), D3DCOLOR_XRGB(255, 255, 255), text);
   }
   else
   {
	   float spood = vect3_length(vinfo->speed);
	   render->D3DBoxi(0, bottom - 20, 101, 10, D3DCOLOR_ARGB(127, 0, 0, 0),NULL);
	   render->D3DBoxi(0, bottom - 19, (int)(spood * 64), 8, D3DCOLOR_ARGB(127, 191, 191, 0), 100);
	   _snprintf_s(text, sizeof(text), "%0.2f km/h", (float)(spood * 170));
	   pD3DFontFixed->PrintShadow((float)(2), (float)(bottom - 20), D3DCOLOR_XRGB(255, 255, 255), text);
   }

   if(set.speedometer_enable)
   {
	   static float mult = set.speedometer_multiplier;

	   float rotationNeedle = 0.0f;
	   D3DXMATRIX mat;
	   
	   rotationNeedle = DEGTORAD((90.0f/100.0f) * ((speed * mult) * 1.55f));
	   rotationNeedle /= 2;
	   if(rotationNeedle > 3.29f) rotationNeedle = 3.29f;
	   
	   D3DXVECTOR2 axisSpeedo = D3DXVECTOR2(speedoPos.x, speedoPos.y);
	   D3DXVECTOR2 axisNeedle = D3DXVECTOR2((130.00f * needlePos.x), (152.00f * needlePos.y));

	   if(!gta_menu_active())
	   {
		   if ((sSpeedoPNG) && (tSpeedoPNG) && (sNeedlePNG) && (tNeedlePNG))
		   {
			   D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &needlePos, &axisNeedle, 0.0f, &axisSpeedo);
			   sSpeedoPNG->Begin(D3DXSPRITE_ALPHABLEND);
			   sSpeedoPNG->SetTransform(&mat);
			   sSpeedoPNG->Draw(tSpeedoPNG, NULL, NULL, NULL, 0xCCFFFFFF);
			   sSpeedoPNG->End();

			   D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &needlePos, &axisNeedle, rotationNeedle, &axisSpeedo);
			   sNeedlePNG->Begin(D3DXSPRITE_ALPHABLEND);
			   sNeedlePNG->SetTransform(&mat);
			   sNeedlePNG->Draw(tNeedlePNG, NULL, NULL, NULL, 0xCCFFFFFF);
			   sNeedlePNG->End();
		   }
	   }
   }
}

// Function taken from the MTA:SA source code (MTA10/core/CGraphics.cpp)
//void CalcScreenCoors ( D3DXVECTOR3 * vecWorld, D3DXVECTOR3 * vecScreen )
//{
//	/** C++-ifyed function 0x71DA00, formerly called by CHudSA::CalcScreenCoors **/
//
//	// Get the static view matrix as D3DXMATRIX
//	D3DXMATRIX m ( (float*)(0xB6FA2C) );
//
//	// Get the static virtual screen (x,y)-sizes
//	DWORD *dwLenX = (DWORD*)(0xC17044);
//	DWORD *dwLenY = (DWORD*)(0xC17048);
//
//	// Do a transformation
//	vecScreen->x = vecWorld->z * m._31 + vecWorld->y * m._21 + vecWorld->x * m._11 + m._41;
//	vecScreen->y = vecWorld->z * m._32 + vecWorld->y * m._22 + vecWorld->x * m._12 + m._42;
//	vecScreen->z = vecWorld->z * m._33 + vecWorld->y * m._23 + vecWorld->x * m._13 + m._43;
//
//	// Get the correct screen coordinates
//	float fRecip = 1.0f / vecScreen->z;
//	vecScreen->x *= fRecip * (*dwLenX);
//	vecScreen->y *= fRecip * (*dwLenY);
//}
// Function taken from the MTA:SA source code (MTA10/core/CGraphics.cpp)
void CalcScreenCoors ( D3DXVECTOR3 * vecWorld, D3DXVECTOR3 * vecScreen )
{
	/** C++-ifyed function 0x71DA00, formerly called by CHudSA::CalcScreenCoors **/

	// Get the static view matrix as D3DXMATRIX
	D3DXMATRIX m ( (float*)(0xB6FA2C) );

	// Get the static virtual screen (x,y)-sizes
	DWORD *dwLenX = (DWORD*)(0xC17044);
	DWORD *dwLenY = (DWORD*)(0xC17048);
	//DWORD *dwLenZ = (DWORD*)(0xC1704C);
	//double aspectRatio = (*dwLenX) / (*dwLenY);

	// Do a transformation
	vecScreen->x = (vecWorld->z * m._31) + (vecWorld->y * m._21) + (vecWorld->x * m._11) + m._41;
	vecScreen->y = (vecWorld->z * m._32) + (vecWorld->y * m._22) + (vecWorld->x * m._12) + m._42;
	vecScreen->z = (vecWorld->z * m._33) + (vecWorld->y * m._23) + (vecWorld->x * m._13) + m._43;

	// Get the correct screen coordinates
	double fRecip = (double)1.0 / vecScreen->z;//(vecScreen->z - (*dwLenZ));
	vecScreen->x *= (float)(fRecip * (*dwLenX));
	vecScreen->y *= (float)(fRecip * (*dwLenY));
}

// for renderPlayerTags()
struct playerTagInfo
{
	#pragma pack(1)
	float tagX;
	float tagY;
	float tagZ;
	float tagOffsetY;
	bool isStairStacked;
	float stairStackedOffset;
} g_playerTagInfo[SAMP_PLAYER_MAX];

// new player ESP
void renderPlayerTags(void)
{
	if (gta_menu_active()) return;
	if (GetAsyncKeyState(VK_TAB) < 0) return;
	if (GetAsyncKeyState(VK_F10) < 0) return;

	if(g_Players == NULL) return;
	struct actor_info *self = actor_info_get(ACTOR_SELF, 0);
	if (self == NULL) return;

	int i; // for fors
	char buf[256]; // format trash buffer
	bool isPlayerWithinView[SAMP_PLAYER_MAX];
	bool isPlayerESPCollided[SAMP_PLAYER_MAX];
	bool isPlayerESPStairStacked[SAMP_PLAYER_MAX];
	for (i = 0; i < SAMP_PLAYER_MAX; i++)
	{
		isPlayerESPCollided[i] = false;
		isPlayerWithinView[i] = false;
		isPlayerESPStairStacked[i] = true;
	}

	int ESP_tag_player_pixelOffsetY = -10;
	float ESP_tag_player_D3DBox_pixelOffsetX = -0.5;
	float ESP_tag_player_D3DBox_pixelOffsetY = -0.5;
	float ESP_tag_player_posOffsetZ = 1.0;
	//DWORD *dwLenX = (DWORD*)(0xC17044);
	//DWORD *dwLenY = (DWORD*)(0xC17048);

	// this loop figures out which players are valid and on screen
	// and populates some variables
	for (i = 0; i < SAMP_PLAYER_MAX; i++)
	{
		struct actor_info *actor = NULL;
		float pos[3];
		D3DXVECTOR3 poss, screenposs;

		if (g_Players->iIsListed[i] != 1) continue;
		if (g_Players->pRemotePlayer[i] == NULL) continue;
		if (isBadPtr_writeAny(g_Players->pRemotePlayer[i], sizeof(stRemotePlayer)) != 0) continue;
		if (g_Players->pRemotePlayer[i]->bytePlayerState == PLAYER_STATE_NONE) continue;
		if (g_Players->pRemotePlayer[i]->pSAMP_Actor != NULL
			&& isBadPtr_writeAny(g_Players->pRemotePlayer[i]->pSAMP_Actor, sizeof(stRemotePlayer)) == 0)
		{
			if ((actor = g_Players->pRemotePlayer[i]->pSAMP_Actor->pGTA_Ped) == NULL) continue;
			if (!getPlayerPos(i, pos)) continue;
			if (vect3_dist(pos, &self->base.matrix[4*3]) > set.player_tags_dist) continue;

			// get the player position in 2D
			poss.x = pos[0];
			poss.y = pos[1];
			poss.z = pos[2] + ESP_tag_player_posOffsetZ;
			CalcScreenCoors(&poss, &screenposs);

			// is this complicated enough?  so far, it seems good
			if (screenposs.z < 1.0f)
			{
				g_playerTagInfo[i].tagOffsetY = 0.0f;
				continue;
			}

			// local, mark this playerID ESP "on" for this frame
			isPlayerWithinView[i] = true;
			// global, set ESP position for tagOffsetY use
			g_playerTagInfo[i].tagX = screenposs.x;
			g_playerTagInfo[i].tagY = screenposs.y;
			g_playerTagInfo[i].tagZ = screenposs.z;
				//+ (self->base.matrix[4*3] * 0.05f);
		}
	}

	// remove staircase problem
	for (i = 0; i < SAMP_PLAYER_MAX; i++)
	{
		if (!isPlayerWithinView[i]
			|| g_playerTagInfo[i].tagOffsetY < 40.f && !g_playerTagInfo[i].isStairStacked)
		{
			continue;
		}
		// detect stair stacking per frame if ESP isn't already stair stacked
		if (!g_playerTagInfo[i].isStairStacked)
		{
			for (int g_pTI_i = 0; g_pTI_i < SAMP_PLAYER_MAX; g_pTI_i++)
			{
				if (!isPlayerWithinView[g_pTI_i]
					|| i == g_pTI_i) continue;
				if (abs(g_playerTagInfo[i].tagX - g_playerTagInfo[g_pTI_i].tagX) <= 100.f
					&& abs((g_playerTagInfo[i].tagY - (g_playerTagInfo[i].tagOffsetY / 2)) - (g_playerTagInfo[g_pTI_i].tagY - g_playerTagInfo[g_pTI_i].tagOffsetY)) <= 20.f)
				{
					isPlayerESPStairStacked[i] = false;
				}
			}
			// setup stair stack variables needed to un stack the ESP
			if (isPlayerESPStairStacked[i])
			{
				g_playerTagInfo[i].isStairStacked = true;
				g_playerTagInfo[i].stairStackedOffset = g_playerTagInfo[i].tagOffsetY / 2;
			}
		}
		// lower the offsets for stair stacked ESP
		// and turn off stack status of ESP that reaches the "available" offset
		if (g_playerTagInfo[i].isStairStacked)
		{
			g_playerTagInfo[i].tagOffsetY = g_playerTagInfo[i].tagOffsetY - 5.f;
			g_playerTagInfo[i].stairStackedOffset = g_playerTagInfo[i].stairStackedOffset - 5.f;
			if (g_playerTagInfo[i].stairStackedOffset < 5.f)
			{
				g_playerTagInfo[i].stairStackedOffset = 0.0f;
				g_playerTagInfo[i].isStairStacked = false;
			}
		}
	}

	// this loop detects collisions and figures out the tagOffsetY
	for (i = 0; i < SAMP_PLAYER_MAX; i++)
	{
		if (!isPlayerWithinView[i]
			|| g_playerTagInfo[i].isStairStacked) continue;
		for (int g_pTI_i = 0; g_pTI_i < SAMP_PLAYER_MAX; g_pTI_i++)
		{
			if (!isPlayerWithinView[g_pTI_i]
				|| i == g_pTI_i
				|| g_playerTagInfo[g_pTI_i].isStairStacked) continue;
			// player is within range, figure out if there's collision
			if (abs(g_playerTagInfo[i].tagX - g_playerTagInfo[g_pTI_i].tagX) <= 100.f
				&& abs((g_playerTagInfo[i].tagY - g_playerTagInfo[i].tagOffsetY) - (g_playerTagInfo[g_pTI_i].tagY - g_playerTagInfo[g_pTI_i].tagOffsetY)) <= 20.f)
			{
				// collision, figure out who gets to stay
				if (g_playerTagInfo[i].tagZ < g_playerTagInfo[g_pTI_i].tagZ)
				{
					// playerID "g_pTI_i" is farther, it should move up
					g_playerTagInfo[g_pTI_i].tagOffsetY = g_playerTagInfo[g_pTI_i].tagOffsetY + 5.f;
					isPlayerESPCollided[g_pTI_i] = true;
				}
				else if (g_playerTagInfo[i].tagZ > g_playerTagInfo[g_pTI_i].tagZ)
				{
					// playerID "i" is farther, it should move up
					// we should only need normal upward movement here
					g_playerTagInfo[i].tagOffsetY = g_playerTagInfo[i].tagOffsetY + 5.f;
					isPlayerESPCollided[i] = true;
				}
				else
				{
					// both playerIDs are the same position @_@ so prefer the lower ID#
					if (i < g_pTI_i)
					{
						g_playerTagInfo[g_pTI_i].tagOffsetY = g_playerTagInfo[g_pTI_i].tagOffsetY + 5.f;
						isPlayerESPCollided[g_pTI_i] = true;
					}
					else
					{
						g_playerTagInfo[i].tagOffsetY = g_playerTagInfo[i].tagOffsetY + 5.f;
						isPlayerESPCollided[i] = true;
					}
				}
			}
			// are we jigglin?  everybody likes ta jiggle.
			if (abs(g_playerTagInfo[i].tagX - g_playerTagInfo[g_pTI_i].tagX) <= 100.f
				&& abs((g_playerTagInfo[i].tagY - g_playerTagInfo[i].tagOffsetY) - (g_playerTagInfo[g_pTI_i].tagY - g_playerTagInfo[g_pTI_i].tagOffsetY)) - 5.f <= 20.f)
			{
				if (g_playerTagInfo[i].tagZ < g_playerTagInfo[g_pTI_i].tagZ)
				{
					isPlayerESPCollided[g_pTI_i] = true;
				} else {
					isPlayerESPCollided[i] = true;
				}
			}
		} // end loop: g_pTI_i
		// return tagOffsetY to zero if needed
		if (!isPlayerESPCollided[i])
		{
			if (g_playerTagInfo[i].tagOffsetY >= 5.f)
			{
				g_playerTagInfo[i].tagOffsetY = g_playerTagInfo[i].tagOffsetY - 5.f;
			}
			else
			{
				g_playerTagInfo[i].tagOffsetY = 0.0f;
			}
		}
	} // end loop: i

	// start render ESP tags
	// variables we need within the loop
	float w, h, playerBaseY;
	for (i = 0; i < SAMP_PLAYER_MAX; i++)
	{
		if (isPlayerWithinView[i])
		{
			// make sure the player is actually there so we don't crash it
			if (isBadPtr_writeAny(g_Players->pRemotePlayer[i], sizeof(stRemotePlayer)) != 0) continue;
			if (isBadPtr_writeAny(g_Players->pRemotePlayer[i]->pSAMP_Actor, sizeof(stSAMPPed)) != 0) continue;

			playerBaseY = g_playerTagInfo[i].tagY - g_playerTagInfo[i].tagOffsetY + ESP_tag_player_pixelOffsetY;
			D3DCOLOR player_color = samp_color_get_trans(g_Players->pRemotePlayer[i]->sPlayerID, 0xDD000000);
			h = pD3DFontSmall->DrawHeight();
			_snprintf_s(buf,sizeof(buf),"%s (%d)", getPlayerName(i), i);
			w = pD3DFontSmall->DrawLength(buf);
			pD3DFontSmall->PrintShadow(g_playerTagInfo[i].tagX, playerBaseY - h, player_color, buf);

			float vh = g_Players->pRemotePlayer[i]->fActorHealth;
			D3DCOLOR color = D3DCOLOR_ARGB(75,0,200,0);

			if (vh > 100.0f) vh = 100.0f;
			if (vh < 100.0f && vh > 60.0f) color = D3DCOLOR_ARGB(111,0,200,0);
			if (vh < 60.0f && vh > 20.0f) color = D3DCOLOR_ARGB(111,200,200,0);
			if (vh < 20.0f && vh > 0.0f) color = D3DCOLOR_ARGB(111,200,0,0);

			render->D3DBox(g_playerTagInfo[i].tagX + ESP_tag_player_D3DBox_pixelOffsetX, playerBaseY + ESP_tag_player_D3DBox_pixelOffsetY, 100.0f, 10.0f, D3DCOLOR_ARGB(111,0,0,0));
			render->D3DBox(g_playerTagInfo[i].tagX + 1.0f + ESP_tag_player_D3DBox_pixelOffsetX, playerBaseY + 1.0f + ESP_tag_player_D3DBox_pixelOffsetY, vh - 2.0f, 8.0f, color);

			if (g_Players->pRemotePlayer[i]->fActorArmor > 0.0f)
			{
				float va = g_Players->pRemotePlayer[i]->fActorArmor;
				if (va > 100.0f) va = 100.0f; va /= 1.0f;
				render->D3DBox(g_playerTagInfo[i].tagX + ESP_tag_player_D3DBox_pixelOffsetX, playerBaseY + ESP_tag_player_D3DBox_pixelOffsetY, va - 1.0f, 10.0f, D3DCOLOR_ARGB(111,0,0,0));
				render->D3DBox(g_playerTagInfo[i].tagX + 1.0f + ESP_tag_player_D3DBox_pixelOffsetX, playerBaseY + 1.0f + ESP_tag_player_D3DBox_pixelOffsetY, va - 2.0f, 8.0f, D3DCOLOR_ARGB(111,220,220,220));
			}

			_snprintf_s(buf,sizeof(buf),"H: %d, A: %d", (int)g_Players->pRemotePlayer[i]->fActorHealth, (int)g_Players->pRemotePlayer[i]->fActorArmor);
			pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[i].tagX + 8.0f, playerBaseY - h + 12.0f, D3DCOLOR_ARGB(130,0xFF,0x6A,0), buf);
		}
	}

	// end render ESP tags
}

void renderVehicleTags(void)
{
	traceLastFunc("renderVehicleTags");
	struct actor_info *self = actor_info_get(ACTOR_SELF, 0);
	if(self == NULL) return;
	if(gta_menu_active()) return;

	if(GetAsyncKeyState(VK_TAB) < 0) return;
	if(GetAsyncKeyState(VK_F10) < 0) return;

	const struct vehicle_entry *vehicle;
	char buf[256];
	int ESP_tag_vehicle_pixelOffsetY = 0;
	float ESP_tag_vehicle_D3DBox_pixelOffsetX = -0.5;
	float ESP_tag_vehicle_D3DBox_pixelOffsetY = -0.5;
	float ESP_tag_vehicle_posOffsetZ = -1.0f;
	int v;

	for(v=0; v<SAMP_VEHICLE_MAX; v++)
	{
		if(g_Vehicles != NULL)
		{
			float pos[3], screenpos[2];
			D3DXVECTOR3 poss, screenposs;
			float w, h;

			if (g_Vehicles->iIsListed[v] != 1) continue;
			if (g_Vehicles->pSAMP_Vehicle[v] == NULL) continue;
			if (g_Vehicles->pSAMP_Vehicle[v]->pGTA_Vehicle == NULL) continue;
			if (isBadPtr_readAny(g_Vehicles->pSAMP_Vehicle[v]->pGTA_Vehicle, sizeof(vehicle_info)) == 0)
			{
				if(g_Vehicles->pGTA_Vehicle[v]->base.interior_id != g_Players->pLocalPlayer->byteCurrentInterior) continue;
				if(g_Players->pLocalPlayer->sCurrentVehicleID == v) continue;

				vehicle = gta_vehicle_get_by_id(g_Vehicles->pSAMP_Vehicle[v]->pGTA_Vehicle->base.model_alt_id);
				vect3_copy(&g_Vehicles->pSAMP_Vehicle[v]->pGTA_Vehicle->base.matrix[4*3], pos);
				if(vect3_near_zero(pos)) continue;
				if(vect3_dist((float *)pos, &self->base.matrix[4*3]) > set.vehicle_tags_dist) continue;

				poss.x = pos[0];
				poss.y = pos[1];
				poss.z = pos[2] + ESP_tag_vehicle_posOffsetZ;
				CalcScreenCoors(&poss, &screenposs);
				screenpos[0] = screenposs.x;
				screenpos[1] = screenposs.y;
				screenpos[2] = screenposs.z;
				if(screenpos[2] < 1.f) continue;
		
				h = pD3DFontSmall->DrawHeight();
				_snprintf_s(buf,sizeof(buf),"%s (%d)", vehicle->name, v);
				w = pD3DFontSmall->DrawLength(buf);
				pD3DFontSmall->PrintShadow(screenpos[0], screenpos[1] - h + ESP_tag_vehicle_pixelOffsetY, D3DCOLOR_ARGB(255,255,255,255), buf);

				float vh = g_Vehicles->pSAMP_Vehicle[v]->pGTA_Vehicle->hitpoints;
				D3DCOLOR vcolor = D3DCOLOR_ARGB(170,150,150,150);

				if(vh > 1000.0f) vh = 1000.0f; vh /= 10.0f;
				if(vh > 25.0f && vh < 100.0f) vcolor = D3DCOLOR_ARGB(130,150,150,150);
				if(vh > 0.0f && vh < 25.0f) vcolor = D3DCOLOR_ARGB(130,200,0,0);

				render->D3DBox(screenpos[0] + ESP_tag_vehicle_D3DBox_pixelOffsetX, screenpos[1] + ESP_tag_vehicle_pixelOffsetY + ESP_tag_vehicle_D3DBox_pixelOffsetY, 100.0f, 11.0f, D3DCOLOR_ARGB(90,0,0,0));
				render->D3DBox(screenpos[0] + 1.0f + ESP_tag_vehicle_D3DBox_pixelOffsetX, screenpos[1] + 1.0f + ESP_tag_vehicle_pixelOffsetY + ESP_tag_vehicle_D3DBox_pixelOffsetY, vh - 2.0f, 9.0f, vcolor);

				_snprintf_s(buf,sizeof(buf),"Health: %0.2f", g_Vehicles->pSAMP_Vehicle[v]->pGTA_Vehicle->hitpoints);
				pD3DFontFixedSmall->PrintShadow(screenpos[0] + 4.0f, screenpos[1] - h + 13.0f + ESP_tag_vehicle_pixelOffsetY, D3DCOLOR_ARGB(90,0,255,0), buf);
			}
		}
	}
}

void RenderTeleportTexts(void)
{
	int i;
	struct actor_info *self = actor_info_get(ACTOR_SELF, 0);
	char buf[32];

	if(self != NULL)
	{
		for(i=0; i<TELEPORT_MAX; i++)
		{
			float pos[3], screenpos[3];
			D3DXVECTOR3 poss, screenposs;
			vect3_copy(&cheat_state->teleport[i].matrix[4*3], pos);
			if(vect3_near_zero(pos)) continue;
			if(vect3_dist(pos, &self->base.matrix[4*3]) > set.player_tags_dist) continue;
				 
			poss.x = pos[0];
			poss.y = pos[1];
			poss.z = pos[2];
			CalcScreenCoors(&poss, &screenposs);
			screenpos[0] = screenposs.x;
			screenpos[1] = screenposs.y;
			screenpos[2] = screenposs.z;
			if(screenpos[2] < 1.f) continue;

			sprintf(buf, "Teleport: %d", i);
			pD3DFontFixed->PrintShadow(screenpos[0], screenpos[1] - 5.0f, D3DCOLOR_XRGB(0, 200, 200), buf);
		}
	}
}

void RenderPickupTexts(void)
{
	struct actor_info *self = actor_info_get(ACTOR_SELF, 0);
	if(g_SAMP->pPool_Pickup == NULL) return;

	char buf[32];
	int i;
	if(self != NULL)
	{
		for(i = 0; i < SAMP_PICKUP_MAX; i++)
		{
			if(g_SAMP->pPool_Pickup->pickup[i].iModelID == 0) continue;
			if(g_SAMP->pPool_Pickup->pickup[i].iType == 0) continue;

			float pos[3], screenpos[3];
			D3DXVECTOR3 poss, screenposs;
			vect3_copy(g_SAMP->pPool_Pickup->pickup[i].fPosition, pos);
			if(vect3_near_zero(pos)) continue;
			if(vect3_dist(pos, &self->base.matrix[4*3]) > set.player_tags_dist) continue;
				 
			poss.x = pos[0];
			poss.y = pos[1];
			poss.z = pos[2];
			CalcScreenCoors(&poss, &screenposs);
			screenpos[0] = screenposs.x;
			screenpos[1] = screenposs.y;
			screenpos[2] = screenposs.z;
			if(screenpos[2] < 1.f) continue;

			sprintf(buf, "Pickup: %d, ModelID: %d", i, g_SAMP->pPool_Pickup->pickup[i].iModelID);
			pD3DFontFixed->PrintShadow(screenpos[0], screenpos[1] - 5.0f, D3DCOLOR_XRGB(0, 200, 0), buf);
		}
	}
}

void RenderObjectTexts(void)
{
	struct actor_info *self = actor_info_get(ACTOR_SELF, 0);
	if(g_SAMP->pPool_Object == NULL) return;

	char buf[32];
	int i;
	if(self != NULL)
	{
		for(i = 0; i < SAMP_OBJECTS_MAX; i++)
		{
			if(g_SAMP->pPool_Object->iIsListed[i] != 1) continue;
			if(g_SAMP->pPool_Object->object[i] == NULL) continue;
			if(g_SAMP->pPool_Object->object[i]->pGTAObject == NULL) continue;

			float pos[3], screenpos[3];
			D3DXVECTOR3 poss, screenposs;
			vect3_copy(&g_SAMP->pPool_Object->object[i]->pGTAObject->base.matrix[4*3], pos);
			if(vect3_near_zero(pos)) continue;
			if(vect3_dist(pos, &self->base.matrix[4*3]) > set.player_tags_dist) continue;
				 
			poss.x = pos[0];
			poss.y = pos[1];
			poss.z = pos[2];
			CalcScreenCoors(&poss, &screenposs);
			screenpos[0] = screenposs.x;
			screenpos[1] = screenposs.y;
			screenpos[2] = screenposs.z;
			if(screenpos[2] < 1.f) continue;

			sprintf(buf, "Object: %d, ModelID: %d", i, g_SAMP->pPool_Object->object[i]->pGTAObject->base.model_alt_id);
			pD3DFontFixed->PrintShadow(screenpos[0], screenpos[1] - 5.0f, D3DCOLOR_XRGB(200, 200, 0), buf);
		}
	}
}

void renderPlayerInfoList(void)
{
	if(gta_menu_active()) return;

	if(GetAsyncKeyState(VK_TAB) < 0) return;
	if(GetAsyncKeyState(VK_F1) < 0) return;
	if(GetAsyncKeyState(VK_F5) < 0) return;
	if(GetAsyncKeyState(VK_F10) < 0) return;

	if(g_Players == NULL && g_Vehicles == NULL) return;

	struct actor_info *self = actor_info_get(ACTOR_SELF, 0);
	if(self == NULL) return;

	float width = 50.0f, height = 10.0f;

	const struct vehicle_entry *vehicle;
	char buf[256];
	float pos[3];

	if(getPlayerCount() == 1)
	{
	   pD3DFont->PrintShadow(width, height, D3DCOLOR_XRGB(255,255,255), "There are no more players but you");
	   return;
	}

	int i;
	for(i = 0; i < SAMP_PLAYER_MAX; i++)
	{
		D3DCOLOR color = MENU_COLOR_DEFAULT;
		if(g_Players->iIsListed[i] != 1) continue;

		if(g_Players->pRemotePlayer[i]->pSAMP_Actor == NULL)
		{
			_snprintf_s(buf,sizeof(buf),"Name: %s (ID: %d), not streamed in", getPlayerName(i), i);
			pD3DFont->PrintShadow(width, height, color, buf);
			height += 1.0f + pD3DFont->DrawHeight();
			continue;
		}

		if(!getPlayerPos(i, pos)) continue;

		color = samp_color_get(g_Players->pRemotePlayer[i]->sPlayerID);
		const char *szPlayerName = getPlayerName(i);
		int iPlayerHealth = (int)g_Players->pRemotePlayer[i]->fActorHealth;
		int iPlayerArmor = (int)g_Players->pRemotePlayer[i]->fActorArmor;
		int iVehicleID = (int)g_Players->pRemotePlayer[i]->sVehicleID;
		int iVehicleHealth;
		if(g_Players->pRemotePlayer[i]->pSAMP_Vehicle != NULL)
		{
			uint16_t playerVehicleID = g_Players->pRemotePlayer[i]->sVehicleID;
			vehicle = gta_vehicle_get_by_id(g_Vehicles->pSAMP_Vehicle[playerVehicleID]->pGTA_Vehicle->base.model_alt_id);
			iVehicleHealth = (int)g_Players->pRemotePlayer[i]->pSAMP_Vehicle->pGTA_Vehicle->hitpoints;
		}

		switch(getPlayerState(i))
		{
		case PLAYER_STATE_NONE:
			_snprintf_s(buf,sizeof(buf),"Name: %s (ID: %d), State: None", szPlayerName, i);
			pD3DFont->PrintShadow(width, height, color, buf);
			break;
		case PLAYER_STATE_ONFOOT:
			_snprintf_s(buf,sizeof(buf),"Name: %s (ID: %d), State: On foot, Health: %d, Armor: %d, Distance: %0.2f",
				szPlayerName, i, iPlayerHealth, iPlayerArmor, vect3_dist((float *)pos, &self->base.matrix[4*3]));
			pD3DFont->PrintShadow(width, height, color, buf);
			break;
		case PLAYER_STATE_DRIVER:
			_snprintf_s(buf,sizeof(buf),"Name: %s (ID: %d), State: Driver (%s (%d)), VHealth: %d, Distance: %0.2f",
				szPlayerName, i, vehicle->name, iVehicleID,	iVehicleHealth,	vect3_dist((float *)pos, &self->base.matrix[4*3]));
			pD3DFont->PrintShadow(width, height, color, buf);
			break;
		case PLAYER_STATE_PASSENGER:
			_snprintf_s(buf,sizeof(buf),"Name: %s (ID: %d), State: Passenger (%s (%d)), Distance: %0.2f",
				szPlayerName, i, vehicle->name, iVehicleID,	vect3_dist((float *)pos, &self->base.matrix[4*3]));
			pD3DFont->PrintShadow(width, height, color, buf);
			break;
		case PLAYER_STATE_WASTED:
			_snprintf_s(buf,sizeof(buf),"Name: %s (ID: %d), State: Wasted", szPlayerName, i);
			pD3DFont->PrintShadow(width, height, color, buf);
			break;
		case PLAYER_STATE_SPAWNED:
			_snprintf_s(buf,sizeof(buf),"Name: %s (ID: %d), State: Spawned", szPlayerName, i);
			pD3DFont->PrintShadow(width, height, color, buf);
			break;
		}
		height += 1.0f + pD3DFont->DrawHeight();
	}
}

void renderKillList(void)
{
	if(g_DeathList == NULL) return;

	static int kill_last = -1, kill_render;
	
	if(GetAsyncKeyState(VK_TAB) < 0) return;
	if(GetAsyncKeyState(VK_F10) < 0) return;

	mmm_yummy_poop(g_DeathList, &g_DeathList->iEnabled, &kill_last, &kill_render, "kill list");
	
	if(kill_render && !gta_menu_active())
	{
		float x = (float)pPresentParam.BackBufferWidth - 180.0f;
		float y = 220.0f;
		float w, h;
		int i;
		
		if(pSpriteTexture != NULL)
			render->D3DBindTexture(pSpriteTexture);
		
		for(i = 0; i < 5; i++)
		{
			struct stKillEntry *ent = &g_DeathList->killEntry[i];
			
			if(strlen(ent->szVictim) > 0)
			{
				int type = ent->byteType;
				
				if(type == 49) {
					type = 20;  // run over
				}
				else if(type == 54) {
					type = 21;  // fall
				}
				else if((type >= 18 && type <= 20) || type >= 47) {
					type = 19;  // suicide 
				}
				
				if(pSpriteTexture != NULL)
				{
					float u = 0.0f, v = 0.0f;
					u = (float)((type % 10) * 24) / 256.0f;
					v = (float)((type / 10) * 24) / 256.0f;
					
					render->D3DTexQuad(
						x - 12.0f, y - 12.0f, x + 12.0f, y + 12.0f,
						u, v, u + 23.0f / 256.0f, v + 23.0f / 256.0f);
				}
				else
				{
					char buf[32];
					sprintf(buf, "%d", type);
					pD3DFont->PrintShadow(x - 12.0f, y - 12.0f, 0xFFFFFFFF, buf);
				}
				
				h = pD3DFont->DrawHeight() / 2.0f;
				if(strlen(ent->szKiller) == 0)
				{
					w = pD3DFont->DrawLength(ent->szVictim);
					pD3DFont->PrintShadow(x - 16.0f - w, y - h, ent->clVictimColor, ent->szVictim);
				}
				else
				{
					w = pD3DFont->DrawLength(ent->szKiller);
					pD3DFont->PrintShadow(x - 16.0f - w, y - h, ent->clKillerColor, ent->szKiller);
					pD3DFont->PrintShadow(x + 16.0f,     y - h, ent->clVictimColor, ent->szVictim);
				}
			}
			
			y += 24.0f;
		}
		
		if(pSpriteTexture != NULL)
			render->D3DBindTexture(NULL);
	}
}

float fYChatPosAdj = 167.0f;
bool bShowChat = true;
void renderChat(void)
{
	if(g_Chat == NULL) return;

	if(GetAsyncKeyState(VK_TAB) < 0) return;
	if(GetAsyncKeyState(VK_F1) < 0) return;
	if(GetAsyncKeyState(VK_F5) < 0) return;
	if(GetAsyncKeyState(VK_F10) < 0) return;

	static int chat_last = -1, chat_render;
	mmm_yummy_poop(g_Chat, &g_Chat->iChatWindowMode, &chat_last, &chat_render, "chat text");

	if(chat_render && !gta_menu_active() && bShowChat)
	{
		if(set.d3dtext_chat_lines <= 0) set.d3dtext_chat_lines = 10;
		if(set.d3dtext_chat_lines >= 100) set.d3dtext_chat_lines = 100;

		DWORD func = g_dwSAMP_Addr + 0x2D350;
		__asm push 1
		__asm call func

		float fYChatPos = fYChatPosAdj;

		if(KEY_DOWN(VK_PRIOR))
		{
			(fYChatPos) -= 20.0f;
			fYChatPosAdj = fYChatPos;
		}
		else
		{
			fYChatPos = fYChatPosAdj;
		}
		if(KEY_DOWN(VK_NEXT))
		{
			(fYChatPos) += 20.0f;
			fYChatPosAdj = fYChatPos;
		}
		else
		{
			fYChatPos = fYChatPosAdj;
		}

		float pw;
		int i = 100;
		int iLinesForLoop = (i - set.d3dtext_chat_lines);
		for(; i > iLinesForLoop; i--)
		{
			struct stChatEntry *ent = &g_Chat->chatEntry[i];
			
			switch(ent->iType)
			{
			case 1:
				pw = pD3DFontChat->DrawLength(ent->szPrefix) + pD3DFontChat->DrawLength(" ");
				pD3DFontChat->PrintShadow(35.0f,      fYChatPos, ent->clPrefixColor, ent->szPrefix);
				pD3DFontChat->PrintShadow(35.0f + pw, fYChatPos, ent->clTextColor,   ent->szText);
				break;
			case 2:
			case 3:
				pD3DFontChat->PrintShadow(35.0f, fYChatPos, ent->clTextColor, ent->szText);
				break;
			}
			fYChatPos -= 1.0f + pD3DFont->DrawHeight();
		}
	}
}

#ifdef M0D_DEV
void renderMainSAMPStructure()
{
	float y = 0.0f;
	D3DCOLOR color = 0xFFFFFFFF;
	char buf[512];
	(y) += 200.0f;
	sprintf(buf, "The main SA:MP structure:");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iGameState: %d", g_SAMP->iGameState);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iLanMode: %d", g_SAMP->iLanMode);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "ulConnectTick: %X", g_SAMP->ulConnectTick);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pPool_Pickup: 0x%p, %s", g_SAMP->pPool_Pickup, isBadPtr_readAny(g_SAMP->pPool_Pickup, sizeof(g_SAMP->pPool_Pickup)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pPool_Menu: 0x%p, %s", g_SAMP->pPool_Menu, isBadPtr_readAny(g_SAMP->pPool_Menu, sizeof(g_SAMP->pPool_Menu)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pPool_Textdraw: 0x%p, %s", g_SAMP->pPool_Textdraw, isBadPtr_readAny(g_SAMP->pPool_Textdraw, sizeof(g_SAMP->pPool_Textdraw)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pPool_Gangzone: 0x%p, %s", g_SAMP->pPool_Gangzone, isBadPtr_readAny(g_SAMP->pPool_Gangzone, sizeof(g_SAMP->pPool_Gangzone)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pPool_Text3D: 0x%p, %s", g_SAMP->pPool_Text3D, isBadPtr_readAny(g_SAMP->pPool_Text3D, sizeof(g_SAMP->pPool_Text3D)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pPool_Player: 0x%p, %s", g_SAMP->pPool_Player, isBadPtr_readAny(g_SAMP->pPool_Player, sizeof(g_SAMP->pPool_Player)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pPool_Vehicle: 0x%p, %s", g_SAMP->pPool_Vehicle, isBadPtr_readAny(g_SAMP->pPool_Vehicle, sizeof(g_SAMP->pPool_Vehicle)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pPool_Object: 0x%p, %s", g_SAMP->pPool_Object, isBadPtr_readAny(g_SAMP->pPool_Object, sizeof(g_SAMP->pPool_Object)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pRakNet: 0x%p, %s", g_SAMP->pRakNet, isBadPtr_readAny(g_SAMP->pRakNet, sizeof(g_SAMP->pRakNet)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "fGlobalChatRadious: %0.2f", g_SAMP->fGlobalChatRadious);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "fNameTagsDistance: %0.2f", g_SAMP->fNameTagsDistance);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteInteriorEnterExits: %u", g_SAMP->byteInteriorEnterExits);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteSpawnsAvailable: %u", g_SAMP->byteSpawnsAvailable);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_1: %u", g_SAMP->byteUnknown_1);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_2: %u", g_SAMP->byteUnknown_2);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_3: %u", g_SAMP->byteUnknown_3);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iPlayerMarkersMode: %d", g_SAMP->iPlayerMarkersMode);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteShowNameTags: %u", g_SAMP->byteShowNameTags);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteWorldTime_Hour: %u", g_SAMP->byteWorldTime_Hour);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteWorldTime_Minute: %u", g_SAMP->byteWorldTime_Minute);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_4: %u", g_SAMP->byteUnknown_4);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteAllowInteriorWeapons: %u", g_SAMP->byteAllowInteriorWeapons);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_5: %u", g_SAMP->byteUnknown_5);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "fGravity: %0.4f", g_SAMP->fGravity);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_6: %u", g_SAMP->byteUnknown_6);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_7: %u", g_SAMP->byteUnknown_7);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_8: %u", g_SAMP->byteUnknown_8);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_9: %u", g_SAMP->byteUnknown_9);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_10: %u", g_SAMP->byteUnknown_10);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_11: %u", g_SAMP->byteUnknown_11);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteUnknown_12: %u", g_SAMP->byteUnknown_12);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "bytePlayerPedAnims: %u", g_SAMP->bytePlayerPedAnims);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "byteWeather: %u", g_SAMP->byteWeather);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "fWorldBoundaries: %0.2f, %0.2f, %0.2f, %0.2f",
		g_SAMP->fWorldBoundaries[0], g_SAMP->fWorldBoundaries[1], g_SAMP->fWorldBoundaries[2], g_SAMP->fWorldBoundaries[3]);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "ulMapIcons: %u", g_SAMP->ulMapIcons[0]);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "ulPort: %u", g_SAMP->ulPort);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "szIP: %s", g_SAMP->szIP);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "szHostname: %s", g_SAMP->szHostname);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
}

void renderPlayerPoolStructure(int iPlayerIDToDebug)
{
	float y = 0.0f;
	D3DCOLOR color = 0xFFFFFFFF;
	char buf[512];
	(y) += 300.0f;
	sprintf(buf, "The player pool and infos on player %d",iPlayerIDToDebug);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pLocalPlayer: 0x%p, %s", g_Players->pLocalPlayer, isBadPtr_readAny(g_Players->pLocalPlayer, sizeof(g_Players->pLocalPlayer)) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "sLocalPlayerID: %u", g_Players->sLocalPlayerID);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();

	if(g_Players->pRemotePlayer[iPlayerIDToDebug] != NULL)
	{
		if(g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Actor == NULL)
		{
			sprintf(buf, "pRemotePlayer(%d): 0x%p, %s. pSAMP_Actor: streamed out or invalid.",
				iPlayerIDToDebug,
				g_Players->pRemotePlayer[iPlayerIDToDebug],
				isBadPtr_readAny(g_Players->pRemotePlayer[iPlayerIDToDebug], sizeof(g_Players->pRemotePlayer[iPlayerIDToDebug])) ? "Bad" : "OK");
		}
		else
		{
			sprintf(buf, "pRemotePlayer(%d): 0x%p, %s. pSAMP_Actor: 0x%p, %s.",
			iPlayerIDToDebug,
			g_Players->pRemotePlayer[iPlayerIDToDebug],
			isBadPtr_readAny(g_Players->pRemotePlayer[iPlayerIDToDebug], sizeof(g_Players->pRemotePlayer[iPlayerIDToDebug])) ? "Bad" : "OK",
			g_Players->pRemotePlayer[iPlayerIDToDebug],
			isBadPtr_readAny(g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Actor, sizeof(g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Actor)) ? "Bad" : "OK");

			if(g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Vehicle != NULL)
			{
				sprintf(buf, "pRemotePlayer(%d): 0x%p, %s. pSAMP_Actor: 0x%p, %s. pSAMP_Vehicle: 0x%p, %s",
				iPlayerIDToDebug,
				g_Players->pRemotePlayer[iPlayerIDToDebug],
				isBadPtr_readAny(g_Players->pRemotePlayer[iPlayerIDToDebug], sizeof(g_Players->pRemotePlayer[iPlayerIDToDebug])) ? "Bad" : "OK",
				g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Actor,
				isBadPtr_readAny(g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Actor, sizeof(g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Actor)) ? "Bad" : "OK",
				g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Vehicle,
				isBadPtr_readAny(g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Vehicle, sizeof(g_Players->pRemotePlayer[iPlayerIDToDebug]->pSAMP_Vehicle)) ? "Bad" : "OK");
			}
		}
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();

		sprintf(buf, "    pRemotePlayer(%d)->iIsNPC: %d", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->iIsNPC);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->sPlayerID: %u", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->sPlayerID);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->byteUpdateFromNetwork: %u", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->byteUpdateFromNetwork);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->bytePlayerState: %u", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->bytePlayerState);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->sVehicleID: %u", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->sVehicleID);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->byteSeatID: %u", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->byteSeatID);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->byteTeamID: %u", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->byteTeamID);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->iDriveby: %d", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->iDriveby);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->ulHeadTick: %X", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->ulHeadTick);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->ulStreamTick: %X", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->ulStreamTick);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->iShowNametag: %d", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->iShowNametag);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->iHasJetpack: %d", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->iHasJetpack);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->byteSpecialAction: %u", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->byteSpecialAction);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->fActorPosition: %0.2f, %0.2f, %0.2f", iPlayerIDToDebug,
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fActorPosition[0],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fActorPosition[1],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fActorPosition[2]);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->fActorMoveSpeed: %0.2f, %0.2f, %0.2f", iPlayerIDToDebug,
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fActorMoveSpeed[0],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fActorMoveSpeed[1],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fActorMoveSpeed[2]);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->fVehiclePosition: %0.2f, %0.2f, %0.2f", iPlayerIDToDebug,
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehiclePosition[0],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehiclePosition[1],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehiclePosition[2]);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->fVehicleMoveSpeed: %0.2f, %0.2f, %0.2f", iPlayerIDToDebug,
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehicleMoveSpeed[0],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehicleMoveSpeed[1],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehicleMoveSpeed[2]);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->fVehicleDirection: %0.2f, %0.2f, %0.2f", iPlayerIDToDebug,
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehicleDirection[0],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehicleDirection[1],
			g_Players->pRemotePlayer[iPlayerIDToDebug]->fVehicleDirection[2]);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->fActorHealth: %0.2f", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->fActorHealth);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();
		sprintf(buf, "    pRemotePlayer(%d)->fActorArmor: %0.2f", iPlayerIDToDebug, g_Players->pRemotePlayer[iPlayerIDToDebug]->fActorArmor);
		pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
		(y) += 1.0f + pD3DFontFixed->DrawHeight();

	}

	sprintf(buf, "iIsListed(%d): %d", iPlayerIDToDebug, g_Players->iIsListed[iPlayerIDToDebug]);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iIsNPC(%d): %d", iPlayerIDToDebug, g_Players->iIsNPC[iPlayerIDToDebug]);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iLocalPlayerScore: %d", g_Players->iLocalPlayerScore);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iScore(%d): %d", iPlayerIDToDebug, g_Players->iScore[iPlayerIDToDebug]);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iLocalPlayerPing: %d", g_Players->iLocalPlayerPing);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iPing(%d): %d", iPlayerIDToDebug, g_Players->iPing[iPlayerIDToDebug]);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "szLocalPlayerName: %s", g_Players->szLocalPlayerName);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "szPlayerName(%d): %s", iPlayerIDToDebug, g_Players->szPlayerName[iPlayerIDToDebug]);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
}


void renderVehiclePoolStructure(int iVehicleIDToDebug)
{
	float y = 0.0f;
	D3DCOLOR color = 0xFFFFFFFF;
	char buf[512];
	(y) += 200.0f;
	sprintf(buf, "The vehicle pool and infos on vehicle %d",iVehicleIDToDebug);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pGTA_Vehicle(%d): 0x%p, %s", iVehicleIDToDebug, g_Vehicles->pGTA_Vehicle[iVehicleIDToDebug],
		isBadPtr_readAny(g_Vehicles->pGTA_Vehicle[iVehicleIDToDebug], sizeof(g_Vehicles->pGTA_Vehicle[iVehicleIDToDebug])) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "pSAMP_Vehicle(%d): 0x%p, %s", iVehicleIDToDebug, g_Vehicles->pSAMP_Vehicle[iVehicleIDToDebug],
		isBadPtr_readAny(g_Vehicles->pGTA_Vehicle[iVehicleIDToDebug], sizeof(g_Vehicles->pSAMP_Vehicle[iVehicleIDToDebug])) ? "Bad" : "OK");
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	sprintf(buf, "iIsListed(%d): %d",iVehicleIDToDebug, g_Vehicles->iIsListed[iVehicleIDToDebug]);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();

}
#endif

void renderPlayerInfo(int iPlayerID)
{
	D3DCOLOR color;
	float y = 0.0f;
	(y) += 300.0f;

	if (iPlayerID == -2)
		goto renderPlayerInfo_renderLocal;

	color = samp_color_get(iPlayerID);
	char buf[512];
	sprintf(buf, "Infos on player %s(%d)", getPlayerName(iPlayerID), iPlayerID);
	pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
	(y) += 1.0f + pD3DFontFixed->DrawHeight();

	if(g_Players->pRemotePlayer[iPlayerID] != NULL)
	{
		if(g_Players->pRemotePlayer[iPlayerID]->pSAMP_Actor == NULL)
		{
			pD3DFontFixed->PrintShadow(20.0f, y, D3DCOLOR_XRGB(200,0,0), "Player is streamed out or invalid.");
			(y) += 1.0f + pD3DFontFixed->DrawHeight();
		}
		else
		{
			sprintf(buf, "Is NPC: %d", g_Players->pRemotePlayer[iPlayerID]->iIsNPC);
			pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
			(y) += 1.0f + pD3DFontFixed->DrawHeight();
			sprintf(buf, "Player state: %u", g_Players->pRemotePlayer[iPlayerID]->bytePlayerState);
			pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
			(y) += 1.0f + pD3DFontFixed->DrawHeight();
			sprintf(buf, "Team ID: %u", g_Players->pRemotePlayer[iPlayerID]->byteTeamID);
			pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
			(y) += 1.0f + pD3DFontFixed->DrawHeight();
			sprintf(buf, "Actor position: %0.2f, %0.2f, %0.2f",
				g_Players->pRemotePlayer[iPlayerID]->fActorPosition[0],
				g_Players->pRemotePlayer[iPlayerID]->fActorPosition[1],
				g_Players->pRemotePlayer[iPlayerID]->fActorPosition[2]);
			pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
			(y) += 1.0f + pD3DFontFixed->DrawHeight();
			sprintf(buf, "Actor health: %0.2f", g_Players->pRemotePlayer[iPlayerID]->fActorHealth);
			pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
			(y) += 1.0f + pD3DFontFixed->DrawHeight();
			sprintf(buf, "Actor armor: %0.2f", g_Players->pRemotePlayer[iPlayerID]->fActorArmor);
			pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
			(y) += 1.0f + pD3DFontFixed->DrawHeight();
			if(g_Players->pRemotePlayer[iPlayerID]->pSAMP_Vehicle != NULL)
			{
				pD3DFontFixed->PrintShadow(20.0f, y, color, "The player is in a vehicle");
				sprintf(buf, " Vehicle ID: %u", g_Players->pRemotePlayer[iPlayerID]->sVehicleID);
				pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
				(y) += 1.0f + pD3DFontFixed->DrawHeight();
				sprintf(buf, " Seat ID: %u", g_Players->pRemotePlayer[iPlayerID]->byteSeatID);
				pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
				(y) += 1.0f + pD3DFontFixed->DrawHeight();
				for (int iRow = 0; iRow < 4; iRow++)
				{
					for (int iCol = 0; iCol < 3; iCol++)
					{
						sprintf(buf, " MATRIX[4*%d+%d]: %0.3f", iRow, iCol, g_Players->pRemotePlayer[iPlayerID]->pSAMP_Vehicle->pGTA_Vehicle->base.matrix[iRow*iCol]);
						pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
						(y) += 1.0f + pD3DFontFixed->DrawHeight();
					}
				}
			}

		}
	}
	return;
renderPlayerInfo_renderLocal:;
	color = D3DCOLOR_ARGB(0xFF, 0xFF, 0x00, 0x00);
	pD3DFontFixed->PrintShadow(20.0f, y, color, "Local Player Info");
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);
	if (vinfo == NULL) return;
	color = D3DCOLOR_ARGB(0xFF, 0x99, 0x99, 0x99);
	pD3DFontFixed->PrintShadow(20.0f, y, color, "Vehicle");
	(y) += 1.0f + pD3DFontFixed->DrawHeight();
	for (int iRow = 0; iRow < 4; iRow++)
	{
		for (int iCol = 0; iCol < 3; iCol++)
		{
			sprintf(buf, " MATRIX[4*%d+%d]: %0.3f", iRow, iCol, vinfo->base.matrix[4*iRow+iCol]);
			pD3DFontFixed->PrintShadow(20.0f, y, color, buf);
			(y) += 1.0f + pD3DFontFixed->DrawHeight();
		}
	}
}

extern bool screenshot;
extern int iDebuggingPlayer, iViewingInfoPlayer;

void renderSAMP(void)
{
	traceLastFunc("renderSAMP()");

	//struct stSAMP *sampInstanced = stGetSampInfo();
	//if (sampInstanced == NULL) return;
	//if (isBadPtr_writeAny(sampInstanced, sizeof(stSAMP))) return;
	//delete sampInstanced;

	if(!g_renderSAMP_initSAMPstructs)
	{
		g_SAMP = stGetSampInfo();
		if (g_SAMP == NULL) return;
		if (isBadPtr_writeAny(g_SAMP, sizeof(stSAMP))) return;
		g_Players = g_SAMP->pPool_Player;
		if (g_Players == NULL) return;
		if (isBadPtr_writeAny(g_Players, sizeof(stPlayerPool))) return;
		g_Vehicles = g_SAMP->pPool_Vehicle;
		if (g_Vehicles == NULL) return;
		if (isBadPtr_writeAny(g_Vehicles, sizeof(stVehiclePool))) return;
		g_Chat = stGetSampChatInfo();
		if (g_Chat == NULL) return;
		if (isBadPtr_writeAny(g_Chat, sizeof(stChatInfo))) return;
		g_Input = stGetInputInfo();
		if (g_Input == NULL) return;
		if (isBadPtr_writeAny(g_Input, sizeof(stInputInfo))) return;
		g_DeathList = stGetKillInfo();
		if (g_DeathList == NULL) return;
		if (isBadPtr_writeAny(g_DeathList, sizeof(stKillInfo))) return;

		g_renderSAMP_initSAMPstructs = 1;
	}

	static int a;
	if(g_SAMP != NULL)
	{
		setSAMPCustomSendRates(set.onfoot_sendrate, set.incar_sendrate, set.aim_sendrate, set.headsync_sendrate);
		if (!g_SAMP->byteShowNameTags && set.nametags_show) g_SAMP->byteShowNameTags = 1;
		if (!g_SAMP->iPlayerMarkersMode && set.markers_show) g_SAMP->iPlayerMarkersMode = 1;
		if (set.samp_runanimation_cj) g_SAMP->bytePlayerPedAnims = 1;

		if (isPornographyMasterControlRunning && set.screenshot_clean)
		{
			g_SAMP->fNameTagsDistance = 70.0f;
			sampPatchDisableNameTags(1);
		}
		else
		{
			g_SAMP->fNameTagsDistance = set.line_of_sight_dist;
			if (cheat_state->_generic.pickuptexts)	RenderPickupTexts();
			if (cheat_state->_generic.objecttexts)	RenderObjectTexts();
			if (cheat_state->render_vehicle_tags)	renderVehicleTags();
			if (cheat_state->render_player_tags)	renderPlayerTags();
			if (cheat_state->player_info_list)		renderPlayerInfoList();
			if (cheat_state->_generic.map)			RenderMap();
			renderKillList();
			renderChat();
			if(iViewingInfoPlayer == -1) { } else { renderPlayerInfo(iViewingInfoPlayer); }
		}

#ifdef M0D_DEV
		//renderMainSAMPStructure();
		if(iDebuggingPlayer == -1) { } else { renderPlayerPoolStructure(iDebuggingPlayer); }		
		//renderVehiclePoolStructure(8);
#endif

		if(!a)
		{
			if (set.player_tags_dist > set.line_of_sight_dist)
				memcpy_safe((void *)(g_dwSAMP_Addr + VALUE_DRAWING_DISTANCE), &set.player_tags_dist, sizeof(float));
			else
				memcpy_safe((void *)(g_dwSAMP_Addr + VALUE_DRAWING_DISTANCE), &set.line_of_sight_dist, sizeof(float));

			setSAMPInitScreenMatrix(set.samp_init_screen_cam_pos[0], set.samp_init_screen_cam_pos[1], set.samp_init_screen_cam_pos[2],
				set.samp_init_screen_cam_look_at[0], set.samp_init_screen_cam_look_at[1], set.samp_init_screen_cam_look_at[2]);

			if(set.screenshot_enable) sampPatchDisableScreeenshotKey(1); else sampPatchDisableScreeenshotKey(0);

			a = 1;
		}
	}
}

typedef float (*FindGroundZForCoord_t)(float x, float y);
FindGroundZForCoord_t FindGroundZForCoord = (FindGroundZForCoord_t)0x569660;
void mapMenuTeleport(void)
{
	if ((*(int*)0xBA6774 != 0) && GetAsyncKeyState(VK_RBUTTON)&1)
	{
		// ty to Racer_S for this
		float mapPos[3];
		for(int i=0; i<(0xAF*0x28); i+=0x28)
		{
			if (*(short*)(0xBA873D + i) == 4611)
			{
				float* pos = (float*)(0xBA86F8 + 0x28 + i);
				gta_interior_id_set(0);
				mapPos[0] = *pos;
				mapPos[1] = *(pos+1);
				mapPos[2] = FindGroundZForCoord(mapPos[0], mapPos[1])+2.0f;
				cheat_teleport(mapPos,0);
			}
		}
	}
}

void loadingLogoInitResources(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	tLoadingLogo = NULL;
	sLoadingLogo = NULL;

	if(!tLoadingLogo)
		D3DXCreateTextureFromFileInMemoryEx(pDevice, &loadingLogo, sizeof(loadingLogo),
		197, 48, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
		0, NULL, NULL, &tLoadingLogo);

	if(!sLoadingLogo)
		D3DXCreateSprite(pDevice, &sLoadingLogo);

	posLoadingLogo.x = (float)(pPresentationParameters->BackBufferWidth * 0.13) - 2.0f;
	posLoadingLogo.y = (float)(pPresentationParameters->BackBufferHeight * 0.9) - 40.0f;
}

void texturesInitResources(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (set.speedometer_enable
		&& (fopen(set.speedometer_speedo_png_filename, "rb") == NULL
			|| fopen(set.speedometer_needle_png_filename, "rb") == NULL))
	{
		Log("Could not find the speedometer files, disabling it.");
		set.speedometer_enable = false;
		set.speedometer_old_enable = true;
	}
	else if (set.speedometer_enable)
	{
		// init speedo
		tSpeedoPNG = NULL;
		sSpeedoPNG = NULL;
		tNeedlePNG = NULL;
		sNeedlePNG = NULL;
		if (!tSpeedoPNG)
			D3DXCreateTextureFromFile(pDevice, set.speedometer_speedo_png_filename, &tSpeedoPNG);
		if (!sSpeedoPNG)
			D3DXCreateSprite(pDevice, &sSpeedoPNG);
		if (!tNeedlePNG)
			D3DXCreateTextureFromFile(pDevice, set.speedometer_needle_png_filename, &tNeedlePNG);
		if (!sNeedlePNG)
			D3DXCreateSprite(pDevice, &sNeedlePNG);
		needlePos.x = (pPresentationParameters->BackBufferWidth / 1024.0f);
		needlePos.y = (pPresentationParameters->BackBufferHeight / 768.0f);
		speedoPos.x = (750.0f * needlePos.x);
		speedoPos.y = pPresentationParameters->BackBufferHeight - (292.0f * needlePos.y);
	}
	// ret
}

float getFPS(void)
{
	static uint32_t fps_time1 = GetTickCount();
	uint32_t fps_time2 = GetTickCount();

	if(fps_time2 - fps_time1 >= 200)
	{
		fps_time1 = GetTickCount();
		fps = *(float *)0xB7CB50;
	}

	if(fps > 500.0f) return 0.0f;

	return fps;
}






///////////////////////////////////////////////////////////////////////////////
// End of common D3D functions.
///////////////////////////////////////////////////////////////////////////////



proxyIDirect3DDevice9::proxyIDirect3DDevice9(IDirect3DDevice9 *pOriginal)
{
   //Log("proxyIDirect3DDevice9 constructor called. Original IDirect3DDevice9 interface address is 0x%p", pOriginal);

	origIDirect3DDevice9 = pOriginal;
	bD3DRenderInit = false;
}

proxyIDirect3DDevice9::~proxyIDirect3DDevice9(void)
{
   Log("proxyIDirect3DDevice9 destructor called.");
}

HRESULT proxyIDirect3DDevice9::QueryInterface(REFIID riid, void **ppvObj)
{
   HRESULT hRes;

   *ppvObj = NULL;
   hRes = origIDirect3DDevice9->QueryInterface(riid, ppvObj); 
   if(hRes == NOERROR)
      *ppvObj = this;

   return hRes;
}

ULONG proxyIDirect3DDevice9::AddRef(void)
{
   return origIDirect3DDevice9->AddRef();
}

ULONG proxyIDirect3DDevice9::Release(void)
{
   ULONG count = origIDirect3DDevice9->Release();

   if(count == 0)
   {
      delete(this);
   }

   return count;
}

HRESULT proxyIDirect3DDevice9::TestCooperativeLevel(void)
{
   return origIDirect3DDevice9->TestCooperativeLevel();
}

UINT proxyIDirect3DDevice9::GetAvailableTextureMem(void)
{
   return origIDirect3DDevice9->GetAvailableTextureMem();
}

HRESULT proxyIDirect3DDevice9::EvictManagedResources(void)
{
   return origIDirect3DDevice9->EvictManagedResources();
}

HRESULT proxyIDirect3DDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
   return origIDirect3DDevice9->GetDirect3D(ppD3D9);
}

HRESULT proxyIDirect3DDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
   return origIDirect3DDevice9->GetDeviceCaps(pCaps);
}

HRESULT proxyIDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE *pMode)
{
   return origIDirect3DDevice9->GetDisplayMode(iSwapChain, pMode);
}

HRESULT proxyIDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
   return origIDirect3DDevice9->GetCreationParameters(pParameters);
}

HRESULT proxyIDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap)
{
   return origIDirect3DDevice9->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void proxyIDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags)
{
   origIDirect3DDevice9->SetCursorPosition(X, Y, Flags);
}

BOOL proxyIDirect3DDevice9::ShowCursor(BOOL bShow)
{
   return origIDirect3DDevice9->ShowCursor(bShow);
}

HRESULT proxyIDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **pSwapChain)
{
   return origIDirect3DDevice9->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT proxyIDirect3DDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9 **pSwapChain)
{
   return origIDirect3DDevice9->GetSwapChain(iSwapChain, pSwapChain);
}

UINT proxyIDirect3DDevice9::GetNumberOfSwapChains(void)
{
   return origIDirect3DDevice9->GetNumberOfSwapChains();
}

void proxyID3DDevice9_UnInitOurShit(void)
{
	traceLastFunc("proxyID3DDevice9_UnInitOurShit()");

	// fonts
	pD3DFont->Invalidate();
	pD3DFontFixed->Invalidate();
	pD3DFontSmall->Invalidate();
	pD3DFontFixedSmall->Invalidate();
	pD3DFontChat->Invalidate();

	SAFE_RELEASE(sLoadingLogo);
	SAFE_RELEASE(tLoadingLogo);
	if(set.speedometer_enable)
	{
		SAFE_RELEASE(sSpeedoPNG);
		SAFE_RELEASE(tSpeedoPNG);
		SAFE_RELEASE(sNeedlePNG);
		SAFE_RELEASE(tNeedlePNG);
	}

	// death texture
	SAFE_RELEASE(pSpriteTexture);

	// the main render class
	render->Invalidate();

	// supposedly this worked so set init state
	// this should probably actually check eventually
	static int proxyIDirect3DDevice9_init = 0;
	bD3DRenderInit = false;
}

void proxyID3DDevice9_InitOurShit(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	traceLastFunc("proxyID3DDevice9_InitOurShit()");

	// orig device has been reset, let's re-init
	render->Initialize(origIDirect3DDevice9);

	// load fonts
	pD3DFont->Initialize(origIDirect3DDevice9);
	pD3DFontFixed->Initialize(origIDirect3DDevice9);
	pD3DFontSmall->Initialize(origIDirect3DDevice9);
	pD3DFontFixedSmall->Initialize(origIDirect3DDevice9);
	pD3DFontChat->Initialize(origIDirect3DDevice9);

	// load GUI textures/sprits
	loadingLogoInitResources(origIDirect3DDevice9, pPresentationParameters);
	texturesInitResources(origIDirect3DDevice9,pPresentationParameters);

	// load death texture
	LoadSpriteTexture();

	// supposedly this worked so set init state
	// this should probably actually check eventually
	bD3DRenderInit = true;
}

bool g_InitWindowMode_ForceUpdate_Active = false;
void proxyID3DDevice9_InitWindowMode(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	traceLastFunc("proxyID3DDevice9_InitWindowMode()");

	// window mode toggle, flips set.window_mode bit
	if (g_isRequestingWindowModeToggle)
	{
		g_isRequestingWindowModeToggle = false;
		set.window_mode ^= 1;
	}

	// m_ForceChange settings
	bool m_ForceUpdate;
	// set to force the desired video mode if needed
	if (set.window_mode != pPresentationParameters->Windowed
		|| set.window_mode != g_pGTAPresent->Windowed)
	{
		// we need to force an update
		m_ForceUpdate = true;
	}

	// basic presentation parameters we want
	g_pGTAPresent->Flags = 0;
	g_pGTAPresent->PresentationInterval = 0;
	pPresentationParameters->Flags = 0;
	pPresentationParameters->PresentationInterval = 0;

	// get the current video mode & the correct back buffer size
	int m_dwVideoMode = g_pCSettingsSAInterface->dwVideoMode;
	VideoMode m_currentVideoMode;
	GTAfunc_getVideoModeInfo(&m_currentVideoMode, m_dwVideoMode);
	int presentWidth = m_currentVideoMode.width;
	int presentHeight = m_currentVideoMode.height;

	// set windowed or fullscreen
	if (g_isRequesting_RwD3D9ChangeVideoMode
		&& m_ForceUpdate
		&& !g_InitWindowMode_ForceUpdate_Active)
	{
		g_isRequesting_RwD3D9ChangeVideoMode = false;
		g_InitWindowMode_ForceUpdate_Active = true;
		if (set.window_mode)
		{
			// set to windowed
			if (GTAfunc_RwD3D9ChangeVideoMode(0) != 1)
			{
				Log("proxyID3DDevice9_InitWindowMode() failed to GTAfunc_RwD3D9ChangeVideoMode(0) #1");
				g_InitWindowMode_ForceUpdate_Active = false;
				return;
			}
			else
			{
				*(int*)0xC920CC = 1; // _?windowed
				g_RsGlobal->ps->fullscreen = 0;
				if (isBadPtr_writeAny(g_pGTAPresent, sizeof(D3DPRESENT_PARAMETERS)))
				{
					Log("proxyID3DDevice9_InitWindowMode() g_pGTAPresent pointer bad. Continuing anyways.");
				}
				else
				{
					g_pGTAPresent->Windowed = 1;
					g_pGTAPresent->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
					g_pGTAPresent->BackBufferWidth = presentWidth;
					g_pGTAPresent->BackBufferHeight = presentHeight;
				}
				if (isBadPtr_writeAny(pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS)))
				{
					Log("proxyID3DDevice9_InitWindowMode() pPresentationParameters pointer bad. Continuing anyways.");
				}
				else
				{
					pPresentationParameters->Windowed = 1;
					pPresentationParameters->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
					pPresentationParameters->BackBufferWidth = presentWidth;
					pPresentationParameters->BackBufferHeight = presentHeight;
				}
				if (GTAfunc_RwD3D9ChangeVideoMode(0) != 1)
				{
					Log("proxyID3DDevice9_InitWindowMode() failed to GTAfunc_RwD3D9ChangeVideoMode(0) #2");
					g_InitWindowMode_ForceUpdate_Active = false;
					return;
				}
				//GTAfunc_setCurrentVideoMode(0);
			}
		}
		else if (!set.window_mode)
		{
			// set to fullscreen
			if (GTAfunc_RwD3D9ChangeVideoMode(0) != 1)
			{
				Log("proxyID3DDevice9_InitWindowMode() failed to GTAfunc_RwD3D9ChangeVideoMode(0) #3");
				g_InitWindowMode_ForceUpdate_Active = false;
				return;
			}
			else
			{
				*(int*)0xC920CC = 0; // _?windowed
				g_RsGlobal->ps->fullscreen = 1;
				if (isBadPtr_writeAny(g_pGTAPresent, sizeof(D3DPRESENT_PARAMETERS)))
				{
					Log("proxyID3DDevice9_InitWindowMode() g_pGTAPresent pointer bad. Continuing anyways.");
				}
				else
				{
					g_pGTAPresent->Windowed = 0;
					g_pGTAPresent->FullScreen_RefreshRateInHz = ulFullScreenRefreshRate;
					g_pGTAPresent->BackBufferWidth = presentWidth;
					g_pGTAPresent->BackBufferHeight = presentHeight;
				}
				if (isBadPtr_writeAny(pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS)))
				{
					Log("proxyID3DDevice9_InitWindowMode() pPresentationParameters pointer bad. Continuing anyways.");
				}
				else
				{
					pPresentationParameters->Windowed = 0;
					pPresentationParameters->FullScreen_RefreshRateInHz = ulFullScreenRefreshRate;
					pPresentationParameters->BackBufferWidth = presentWidth;
					pPresentationParameters->BackBufferHeight = presentHeight;
				}
				if (GTAfunc_RwD3D9ChangeVideoMode(m_dwVideoMode) != 1)
				{
					Log("proxyID3DDevice9_InitWindowMode() failed to GTAfunc_RwD3D9ChangeVideoMode(m_dwVideoMode) #1, m_dwVideoMode=%d", m_dwVideoMode);
					g_InitWindowMode_ForceUpdate_Active = false;
					return;
				}
				//GTAfunc_setCurrentVideoMode(m_dwVideoMode);
			}
		}
		g_InitWindowMode_ForceUpdate_Active = false;
	}
	else if (m_ForceUpdate
		&& set.window_mode
		&& !g_InitWindowMode_ForceUpdate_Active)
	{
		g_InitWindowMode_ForceUpdate_Active = true;
		*(int*)0xC920CC = 1; // _?windowed
		g_RsGlobal->ps->fullscreen = 0;
		if (isBadPtr_writeAny(g_pGTAPresent, sizeof(D3DPRESENT_PARAMETERS)))
		{
			Log("proxyID3DDevice9_InitWindowMode() g_pGTAPresent pointer bad. Continuing anyways.");
		}
		else
		{
			g_pGTAPresent->Windowed = 1;
			g_pGTAPresent->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
			g_pGTAPresent->BackBufferWidth = presentWidth;
			g_pGTAPresent->BackBufferHeight = presentHeight;
		}
		if (isBadPtr_writeAny(pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS)))
		{
			Log("proxyID3DDevice9_InitWindowMode() pPresentationParameters pointer bad. Continuing anyways.");
		}
		else
		{
			pPresentationParameters->Windowed = 1;
			pPresentationParameters->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
			pPresentationParameters->BackBufferWidth = presentWidth;
			pPresentationParameters->BackBufferHeight = presentHeight;
		}
		g_InitWindowMode_ForceUpdate_Active = false;
	}

	// window mode stuff
	if (set.window_mode
		&& pPresentationParameters->BackBufferWidth != 0
		&& pPresentationParameters->BackBufferHeight != 0)
	{
		// title bar or not?
		if(set.window_mode_titlebar)
		{
			RECT um; // damn near killed um
			// add caption bar, etc
			SetWindowLong(pPresentationParameters->hDeviceWindow, GWL_STYLE, WS_POPUP | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE);
			// update caption bar, etc
			SetWindowPos(pPresentationParameters->hDeviceWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
			// the client area of the window must be the same size as the back buffer
			GetClientRect(pPresentationParameters->hDeviceWindow, &um);
			if(um.right == pPresentationParameters->BackBufferWidth && um.bottom == pPresentationParameters->BackBufferHeight)
				goto proxyID3DDevice9_InitWindowMode_end;
			SetWindowPos(pPresentationParameters->hDeviceWindow, HWND_NOTOPMOST, 0, 0,
			   pPresentationParameters->BackBufferWidth + (pPresentationParameters->BackBufferWidth - um.right),
			   pPresentationParameters->BackBufferHeight + (pPresentationParameters->BackBufferHeight - um.bottom),
			   SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOMOVE);
			if(pool_actor != NULL)
			{
				int x, y;
				x = GetSystemMetrics(SM_CXSCREEN);
				y = GetSystemMetrics(SM_CYSCREEN);
				SetWindowPos(pPresentationParameters->hDeviceWindow, HWND_NOTOPMOST,
					(x/2)-(pPresentationParameters->BackBufferWidth/2),
					(y/2)-(pPresentationParameters->BackBufferHeight/2),
					pPresentationParameters->BackBufferWidth + (pPresentationParameters->BackBufferWidth - um.right),
					pPresentationParameters->BackBufferHeight + (pPresentationParameters->BackBufferHeight - um.bottom),
					SWP_SHOWWINDOW);
			}

		}
		else
		{
			// center the window
			SetWindowPos(pPresentationParameters->hDeviceWindow, HWND_NOTOPMOST,
				0, 0, 0, 0,
				SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		}
	}

proxyID3DDevice9_InitWindowMode_end:;
	// always make sure our window_mode is synced with the game's
	set.window_mode = (g_RsGlobal->ps->fullscreen == 0);
}

HRESULT proxyIDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	traceLastFunc("proxyIDirect3DDevice9::Reset()");

	// new PROPER reset code
	//

	// return for the reset
	HRESULT hRes_orig_Reset;

	// this should always be done if a reset is requested
	// and we have assets initialized on the proxy device
	while (bD3DRenderInit) proxyID3DDevice9_UnInitOurShit();

	// always attempt to reset orig device if we are asked to
	// since GTA is getting the state of the original device
	hRes_orig_Reset = origIDirect3DDevice9->Reset(pPresentationParameters);

	// handle the return from original Reset()
	if (hRes_orig_Reset == D3D_OK)
	{
		// variable for checking the pointers
		short badPointerCount_g_pGTAPresent = 0;
		bool badPointerBreak_g_pGTAPresent = false;
		short badPointerCount_pPresentationParameters = 0;
		bool badPointerBreak_pPresentationParameters = false;
		while (isBadPtr_writeAny(g_pGTAPresent, sizeof(D3DPRESENT_PARAMETERS))
			&& !badPointerBreak_g_pGTAPresent)
		{
			badPointerCount_g_pGTAPresent++;
			if (badPointerCount_g_pGTAPresent < 50)
			{
				Sleep(100);
			}
			else
			{
				Log("During D3D9 Reset(), g_pGTAPresent was bad for over 5 seconds. Continuing anyways.");
				badPointerBreak_g_pGTAPresent = true;
			}
		}
		while (isBadPtr_writeAny(pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS))
			&& !badPointerBreak_pPresentationParameters)
		{
			badPointerCount_pPresentationParameters++;
			if (badPointerCount_pPresentationParameters < 50)
			{
				Sleep(100);
			}
			else
			{
				Log("During D3D9 Reset(), pPresentationParameters was bad for over 5 seconds. Continuing anyways.");
				badPointerBreak_pPresentationParameters = true;
			}
		}
		// init our shit
		while (!bD3DRenderInit)
		{
			proxyID3DDevice9_InitOurShit(pPresentationParameters);
			if (!bD3DRenderInit)
				Sleep(100);
		}
		// init our window mode
		proxyID3DDevice9_InitWindowMode(pPresentationParameters);
		// update the global Present Param struct AFTER original reset, only if it's ok
		pPresentParam = *pPresentationParameters;
	}
	// reporting problems is about all we can do here.
	// we would normally just end the application gracefully right
	// now, but GTA should be handling this on its own, so whatever.
	else if (hRes_orig_Reset == D3DERR_DEVICELOST)
	{
		Log("origIDirect3DDevice9::Reset() returned D3DERR_DEVICELOST");
	}
	else if (hRes_orig_Reset == D3DERR_DRIVERINTERNALERROR)
	{
		Log("WTF: origIDirect3DDevice9::Reset() returned D3DERR_DRIVERINTERNALERROR");
	}
	else if (hRes_orig_Reset == D3DERR_OUTOFVIDEOMEMORY)
	{
		Log("WTF: origIDirect3DDevice9::Reset() returned D3DERR_OUTOFVIDEOMEMORY");
	}
	else
	{
		Log("WTF: origIDirect3DDevice9::Reset() returned value %d", hRes_orig_Reset);
	}

	// bai
	return hRes_orig_Reset;
}

HRESULT proxyIDirect3DDevice9::Present(CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
{
	traceLastFunc("proxyIDirect3DDevice9::Present()");

	// ATi flicker fix
	// something is clearly not right here, lol
	//D3DMATRIX *projMatrix = (D3DMATRIX*)(DWORD*)__RwD3D9ActiveViewProjTransform;
	//origIDirect3DDevice9->SetTransform(D3DTS_PROJECTION, projMatrix);

	// get original function to return
	return origIDirect3DDevice9->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT proxyIDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9 **ppBackBuffer)
{
   return origIDirect3DDevice9->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT proxyIDirect3DDevice9::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS *pRasterStatus)
{
   return origIDirect3DDevice9->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT proxyIDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
   return origIDirect3DDevice9->SetDialogBoxMode(bEnableDialogs);
}

void proxyIDirect3DDevice9::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP *pRamp)
{
   origIDirect3DDevice9->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void proxyIDirect3DDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP *pRamp)
{
   origIDirect3DDevice9->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT proxyIDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9 **ppTexture, HANDLE *pSharedHandle)
{
   return origIDirect3DDevice9->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT proxyIDirect3DDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9 **ppVolumeTexture, HANDLE *pSharedHandle)
{
   return origIDirect3DDevice9->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT proxyIDirect3DDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9 **ppCubeTexture, HANDLE *pSharedHandle)
{
   return origIDirect3DDevice9->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT proxyIDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle)
{
   return origIDirect3DDevice9->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT proxyIDirect3DDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9 **ppIndexBuffer, HANDLE *pSharedHandle)
{
   return origIDirect3DDevice9->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT proxyIDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
   return origIDirect3DDevice9->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT proxyIDirect3DDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
   return origIDirect3DDevice9->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT proxyIDirect3DDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface, CONST RECT *pSourceRect, IDirect3DSurface9 *pDestinationSurface, CONST POINT *pDestPoint)
{
   return origIDirect3DDevice9->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT proxyIDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9 *pSourceTexture, IDirect3DBaseTexture9 *pDestinationTexture)
{
   return origIDirect3DDevice9->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT proxyIDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pDestSurface)
{
   return origIDirect3DDevice9->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT proxyIDirect3DDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9 *pDestSurface)
{
   return origIDirect3DDevice9->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT proxyIDirect3DDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface, CONST RECT *pSourceRect, IDirect3DSurface9 *pDestSurface, CONST RECT *pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
   return origIDirect3DDevice9->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT proxyIDirect3DDevice9::ColorFill(IDirect3DSurface9 *pSurface, CONST RECT *pRect, D3DCOLOR color)
{
   return origIDirect3DDevice9->ColorFill(pSurface, pRect, color);
}

HRESULT proxyIDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
   return origIDirect3DDevice9->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT proxyIDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget)
{
   return origIDirect3DDevice9->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT proxyIDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 **ppRenderTarget)
{
   return origIDirect3DDevice9->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT proxyIDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
   return origIDirect3DDevice9->SetDepthStencilSurface(pNewZStencil);
}

HRESULT proxyIDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
   return origIDirect3DDevice9->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT proxyIDirect3DDevice9::BeginScene(void)
{
	traceLastFunc("proxyIDirect3DDevice9::BeginScene()");

	// thanks STUNT COCK!!!  (now go away, please)
	isPornographyStuntCockReady = false;

	// return original function
	return origIDirect3DDevice9->BeginScene();
}

extern D3DXVECTOR3 vecGravColOrigin, vecGravColTarget, vecGravTargetNorm;

HRESULT proxyIDirect3DDevice9::EndScene(void)
{
	traceLastFunc("proxyIDirect3DDevice9::EndScene()");

	// set FPS limit for vsync mode
	memcpy_safe((void *)0xC1704C, &set.fps_limit, sizeof(int));

	// we should already be initialized, but what the hell why not
	while (!bD3DRenderInit) proxyID3DDevice9_InitOurShit(&pPresentParam);

	// init death type textures and HUD colors
	static int proxyIDirect3DDevice9_init;
	if(!proxyIDirect3DDevice9_init)
	{
		LoadSpriteTexture();

		*(uint8_t *)0xBAB22C = gta_hp_bar->red;
		*(uint8_t *)0xBAB22D = gta_hp_bar->green;
		*(uint8_t *)0xBAB22E = gta_hp_bar->blue;
		*(uint8_t *)0xBAB22F = gta_hp_bar->alpha;

		*(uint8_t *)0xBAB230 = gta_money_hud->red;
		*(uint8_t *)0xBAB231 = gta_money_hud->green;
		*(uint8_t *)0xBAB232 = gta_money_hud->blue;
		*(uint8_t *)0xBAB233 = gta_money_hud->alpha;

		proxyIDirect3DDevice9_init = 1;
	}

	// run all dem hacks
	cheat_hook(pPresentParam.hDeviceWindow);

	#define HUD_TEXT(x, color, text)															\
		pD3DFont->PrintShadow(																	\
			1.0f + (x), (float)(pPresentParam.BackBufferHeight - 1) - pD3DFont->DrawHeight(),	\
			(color), (text));																	\
		(x) += pD3DFont->DrawLength(text);

	#define HUD_TEXT_TGL(x, color, text)	\
		HUD_TEXT(x, color_text, "[");		\
		HUD_TEXT(x, color, text);			\
		HUD_TEXT(x, color_text, "] ")

	if(1==1)
	{
		char buf[256];
		float x = 0.0f;

		uint32_t color_text = D3DCOLOR_ARGB(191, 255, 255, 255);
		uint32_t color_enabled  = D3DCOLOR_ARGB(191, 63, 255, 63);
		uint32_t color_disabled = D3DCOLOR_ARGB(191, 255, 255, 255);

		if(SUCCEEDED(render->BeginRender()))
		{
			static int game_inited;
#ifdef M0D_DEV
			if(!set.flickering_problem)
			{
				pD3DFontFixed->PrintShadow(0.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 255, 0), NAME);
			}
			else
			{
				if(game_inited) if(!gta_menu_active()) pD3DFontFixed->PrintShadow(0.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 255, 0), NAME);
			}
			if(g_SAMP != NULL && g_SAMP->pRakNet != NULL)
			{
				uint32_t iBitsSent, iBitsRecv;
				__asm
				{
					mov ecx, g_dwSAMP_Addr
					add ecx, SAMP_INFO_OFFSET
					mov ecx, dword ptr [ecx]
					test ecx, ecx
					je pRakNet_NetTxRx_Hook_End
					mov ecx, dword ptr [ecx+0x2C]
					test ecx, ecx
					je pRakNet_NetTxRx_Hook_End
					mov edx, dword ptr [ecx]
					test edx, edx
					je pRakNet_NetTxRx_Hook_End
					mov eax, dword ptr [ecx-0x6B6]
					test eax, eax
					je pRakNet_NetTxRx_Hook_End
					call dword ptr [edx+0xCC]
					mov ecx, dword ptr [eax+0x34]
					mov iBitsSent, ecx
					mov ecx, dword ptr [eax+0x94]
					mov iBitsRecv, ecx
				} // __asm
				iBitsSent = (iBitsSent / 8);
				iBitsRecv = (iBitsRecv / 8);
				_snprintf_s(buf, sizeof(buf), "Bytes sent: %u || Bytes received: %u", iBitsSent, iBitsRecv);
				pD3DFontFixed->PrintShadow(pPresentParam.BackBufferWidth - pD3DFontFixed->DrawLength(buf) - 2,
					pPresentParam.BackBufferHeight - pD3DFontFixed->DrawHeight() - 20, D3DCOLOR_ARGB(215, 0, 255, 0), buf);
pRakNet_NetTxRx_Hook_End:;
			}
#endif // M0D_DEV

			if(set.d3dtext_hud)
			{
				if (isPornographyMasterControlRunning && set.screenshot_clean) goto no_d3dtext_hud;
				if(cheat_panic() || cheat_state->state == CHEAT_STATE_NONE)
				{
					if(set.flickering_problem) goto no_render;

					if(game_inited)
					{
						if(g_dwSAMP_Addr != NULL)
						{
							uint32_t bar_color = D3DCOLOR_ARGB(hud_bar->alpha, hud_bar->red, hud_bar->green, hud_bar->blue);
							render->D3DBoxi((int)x - 1, (int)(pPresentParam.BackBufferHeight - 1) - (int)pD3DFont->DrawHeight() - 3,
								(int)(pPresentParam.BackBufferWidth + 14), 22, bar_color, NULL);
							HUD_TEXT(x, D3DCOLOR_ARGB(127, 255, 255, 255), NAME " for " SAMP_VERSION);
						}
						else
						{
							uint32_t bar_color = D3DCOLOR_ARGB(hud_bar->alpha, hud_bar->red, hud_bar->green, hud_bar->blue);
							render->D3DBoxi((int)x - 1, (int)(pPresentParam.BackBufferHeight - 1) - (int)pD3DFont->DrawHeight() - 3,
								(int)(pPresentParam.BackBufferWidth + 14), 22, bar_color, NULL);
							HUD_TEXT(x, D3DCOLOR_ARGB(127, 255, 255, 255), NAME);
						}
					}
					else
					{
						if ((sLoadingLogo) && (tLoadingLogo) && !gta_menu_active())
						{
							sLoadingLogo->Begin(D3DXSPRITE_ALPHABLEND);
							sLoadingLogo->Draw(tLoadingLogo,NULL,NULL,&posLoadingLogo,0xFFFFFFFF);
							sLoadingLogo->End();
						}

					}

				}
				else
				{
					if (!game_inited)
					{
						game_inited = 1;
						// maybe this is the fix to the world load hang?
						cheat_state->text_time = 0;
						goto no_render;
					}

					if(set.flickering_problem) if(gta_menu_active()) goto no_render;

					if(set.left_bottom_bars_enable) (x) += 102.f;

					if(set.hud_draw_bar)
					{
						uint32_t bar_color = D3DCOLOR_ARGB(
							hud_bar->alpha, hud_bar->red, hud_bar->green, hud_bar->blue);
	
						render->D3DBoxi((int)x - 1, (int)(pPresentParam.BackBufferHeight - 1) - (int)pD3DFont->DrawHeight() - 3,
							(int)(pPresentParam.BackBufferWidth + 14), 22, bar_color, NULL);
					}

					if(set.hud_indicator_inv) { HUD_TEXT_TGL(x, cheat_state->_generic.hp_cheat ? color_enabled : color_disabled , "Inv"); }
					if(set.hud_indicator_weapon) { HUD_TEXT_TGL(x, cheat_state->_generic.weapon ? color_enabled : color_disabled , "Weapon"); }
					if(set.hud_indicator_money) { HUD_TEXT_TGL(x, cheat_state->_generic.money ? color_enabled : color_disabled , "Money"); }
					if(set.hud_indicator_freeze) { HUD_TEXT_TGL(x, cheat_state->_generic.vehicles_freeze ? color_enabled : color_disabled , "Freeze"); }

					if(set.hud_fps_draw)
					{
						uint32_t color_fps = D3DCOLOR_XRGB(200, 200, 0);
						if((int)getFPS() >= 23)
							color_fps = D3DCOLOR_XRGB(0, 200, 0);
						else if((int)getFPS() >= 13 && (int)getFPS <= 22)
							color_fps = D3DCOLOR_XRGB(200, 200, 0);
						else if((int)getFPS() <= 12)
							color_fps = D3DCOLOR_XRGB(200, 0, 0);

						_snprintf_s(buf, sizeof(buf), "%0.2f (%d)", getFPS(), *(int*)0xC1704C );
						pD3DFont->PrintShadow(pPresentParam.BackBufferWidth - pD3DFont->DrawLength(buf) - 2,
							pPresentParam.BackBufferHeight - pD3DFont->DrawHeight() - 2, color_fps, buf);
					}	

				}

				if(cheat_state->state == CHEAT_STATE_VEHICLE)
				{
					if(set.hud_indicator_inveh_airbrk) { HUD_TEXT_TGL(x, cheat_state->vehicle.air_brake ? color_enabled : color_disabled , "AirBrk"); }
					if(set.hud_indicator_inveh_stick) { HUD_TEXT_TGL(x, cheat_state->vehicle.stick ? color_enabled : color_disabled , "Stick"); }
					if(set.hud_indicator_inveh_brkdance) { HUD_TEXT_TGL(x, cheat_state->vehicle.brkdance ? color_enabled : color_disabled , "BrkDance"); }
					if(set.hud_indicator_inveh_spider) { HUD_TEXT_TGL(x, cheat_state->vehicle.spiderWheels_on ? color_enabled : color_disabled , "Spider"); }
					RenderVehicleHPBar();
#ifdef M0D_DEV
// fantastic shit
/*
render->DrawLine(vecGravColOrigin, vecGravColTarget, D3DCOLOR_ARGB(128, 255, 0, 0));
render->DrawLine(vecGravColOrigin, vecGravTargetNorm, D3DCOLOR_ARGB(128, 0, 255, 0));

struct vehicle_info *vinfo_self = vehicle_info_get(VEHICLE_SELF, 0);
_snprintf_s(buf, sizeof(buf), "gravityVector: %0.2f %0.2f %0.2f", cheat_state->vehicle.gravityVector.fX, cheat_state->vehicle.gravityVector.fY, cheat_state->vehicle.gravityVector.fZ);
pD3DFontFixed->PrintShadow(pPresentParam.BackBufferWidth - pD3DFontFixed->DrawLength(buf) - 20,
	pPresentParam.BackBufferHeight - pD3DFontFixed->DrawHeight() - 50, D3DCOLOR_ARGB(215, 0, 255, 0), buf);
*/
#endif
				}
				else if(cheat_state->state == CHEAT_STATE_ACTOR)
				{
					if(set.hud_indicator_onfoot_airbrk) { HUD_TEXT_TGL(x, cheat_state->actor.air_brake ? color_enabled : color_disabled , "AirBrk"); }
					if(set.hud_indicator_onfoot_stick) { HUD_TEXT_TGL(x, cheat_state->actor.stick ? color_enabled : color_disabled , "Stick"); }
					if(set.hud_indicator_onfoot_aim) { HUD_TEXT_TGL(x, cheat_state->actor.autoaim ? color_enabled : color_disabled , "Aim"); }
					RenderPedHPBar();
				}

				if(cheat_state->state != CHEAT_STATE_NONE)
				{
					if(set.hud_indicator_pos)
					{
						float *coord = (cheat_state->state == CHEAT_STATE_VEHICLE) ?
							cheat_state->vehicle.coords
							: cheat_state->actor.coords;

						_snprintf_s(buf, sizeof(buf), "  %.2f, %.2f, %.2f  %d", coord[0], coord[1], coord[2], gta_interior_id_get() );
						HUD_TEXT(x, color_text, buf);
					}
				}

				if(cheat_state->text_time > 0 && time_get() - cheat_state->text_time < MSEC_TO_TIME(3000))
				{
					uint32_t color, alpha = 191;

					if(time_get() - cheat_state->text_time > MSEC_TO_TIME(2000))
						alpha -= (time_get() - cheat_state->text_time - MSEC_TO_TIME(2000)) * 191 / MSEC_TO_TIME(1000);

					color = D3DCOLOR_ARGB(alpha, 255, 255, 255);

					_snprintf_s(buf, sizeof(buf), "%s <-", cheat_state->text );
					pD3DFont->PrintShadow(pPresentParam.BackBufferWidth - pD3DFont->DrawLength(buf) - 3.0f,
						1, D3DCOLOR_ARGB(alpha, 255, 255, 255), buf);
				}

			}


no_d3dtext_hud:;
			if(g_dwSAMP_Addr != NULL)
				renderSAMP(); // sure why not
			if (isPornographyMasterControlRunning && set.screenshot_clean) {}
			else
			{
				if(cheat_state->_generic.teletext)   RenderTeleportTexts();
				if(cheat_state->_generic.menu)       RenderMenu();
				if(cheat_state->debug_enabled)       RenderDebug();
			}


no_render:;
			render->EndRender();
		}
		mapMenuTeleport();

	}

	HRESULT ret = origIDirect3DDevice9->EndScene();

	// clean Pornography, thanks to STUNT COCK!!!
	if (isPornographyMasterControlRunning && !isPornographyStuntCockReady) isPornographyStuntCockReady = true;
	// sexytime
	if(isRequestingScreenshot && !isPornographyMasterControlRunning && g_lastPornographyTickCount < (GetTickCount() - 1000))
	{
		isRequestingScreenshot = false;
		if (Pornography() == false) addMessageToChatWindowSS("Could not take a screenshot.  Thread FAIL!");
	}

	// return GPU's EndScene()
	return ret;
}

HRESULT proxyIDirect3DDevice9::Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
   return origIDirect3DDevice9->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT proxyIDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
	/*
	switch (State)
	{
		case D3DTS_VIEW:
			// Copy the real view matrix.
			memcpy(&m_mViewMatrix, pMatrix, sizeof(D3DMATRIX));
			break;
		case D3DTS_PROJECTION:
			// Copy the real projection marix.
			memcpy(&m_mProjMatrix, pMatrix, sizeof(D3DMATRIX));
			break;
		case D3DTS_WORLD:
			// Copy the real world matrix.
			memcpy(&m_mWorldMatrix, pMatrix, sizeof(D3DMATRIX));
			break;
		default:
			// Do nothing.
			break;
	}
	*/
	return origIDirect3DDevice9->SetTransform(State, pMatrix);
}

HRESULT proxyIDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix)
{
	return origIDirect3DDevice9->GetTransform(State, pMatrix);
}

HRESULT proxyIDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
   return origIDirect3DDevice9->MultiplyTransform(State, pMatrix);
}

HRESULT proxyIDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9 *pViewport)
{
   return origIDirect3DDevice9->SetViewport(pViewport);
}

HRESULT proxyIDirect3DDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
   return origIDirect3DDevice9->GetViewport(pViewport);
}

HRESULT proxyIDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9 *pMaterial)
{
   return origIDirect3DDevice9->SetMaterial(pMaterial);
}

HRESULT proxyIDirect3DDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
   return origIDirect3DDevice9->GetMaterial(pMaterial);
}

HRESULT proxyIDirect3DDevice9::SetLight(DWORD Index, CONST D3DLIGHT9 *pLight)
{
   return origIDirect3DDevice9->SetLight(Index, pLight);
}

HRESULT proxyIDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9 *pLight)
{
   return origIDirect3DDevice9->GetLight(Index, pLight);
}

HRESULT proxyIDirect3DDevice9::LightEnable(DWORD Index, BOOL Enable)
{
   return origIDirect3DDevice9->LightEnable(Index, Enable);
}

HRESULT proxyIDirect3DDevice9::GetLightEnable(DWORD Index, BOOL *pEnable)
{
   return origIDirect3DDevice9->GetLightEnable(Index, pEnable);
}

HRESULT proxyIDirect3DDevice9::SetClipPlane(DWORD Index, CONST float *pPlane)
{
   return origIDirect3DDevice9->SetClipPlane(Index, pPlane);
}

HRESULT proxyIDirect3DDevice9::GetClipPlane(DWORD Index, float *pPlane)
{
   return origIDirect3DDevice9->GetClipPlane(Index, pPlane);
}

HRESULT proxyIDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
   return origIDirect3DDevice9->SetRenderState(State, Value);
}

HRESULT proxyIDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue)
{
   return origIDirect3DDevice9->GetRenderState(State, pValue);
}

HRESULT proxyIDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB)
{
   return origIDirect3DDevice9->CreateStateBlock(Type, ppSB);
}

HRESULT proxyIDirect3DDevice9::BeginStateBlock(void)
{
   return origIDirect3DDevice9->BeginStateBlock();
}

HRESULT proxyIDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{
   return origIDirect3DDevice9->EndStateBlock(ppSB);
}

HRESULT proxyIDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9 *pClipStatus)
{
   return origIDirect3DDevice9->SetClipStatus(pClipStatus);
}

HRESULT proxyIDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
   return origIDirect3DDevice9->GetClipStatus(pClipStatus);
}

HRESULT proxyIDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9 **ppTexture)
{
   return origIDirect3DDevice9->GetTexture(Stage, ppTexture);
}

HRESULT proxyIDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture)
{
   return origIDirect3DDevice9->SetTexture(Stage, pTexture);
}

HRESULT proxyIDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue)
{
   return origIDirect3DDevice9->GetTextureStageState(Stage, Type, pValue);
}

HRESULT proxyIDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
   return origIDirect3DDevice9->SetTextureStageState(Stage, Type, Value);
}

HRESULT proxyIDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD *pValue)
{
   return origIDirect3DDevice9->GetSamplerState(Sampler, Type, pValue);
}

HRESULT proxyIDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
   return origIDirect3DDevice9->SetSamplerState(Sampler, Type, Value);
}

HRESULT proxyIDirect3DDevice9::ValidateDevice(DWORD *pNumPasses)
{
   return origIDirect3DDevice9->ValidateDevice(pNumPasses);
}

HRESULT proxyIDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY *pEntries)
{
   return origIDirect3DDevice9->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT proxyIDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries)
{
   return origIDirect3DDevice9->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT proxyIDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
   return origIDirect3DDevice9->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT proxyIDirect3DDevice9::GetCurrentTexturePalette(UINT *PaletteNumber)
{
   return origIDirect3DDevice9->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT proxyIDirect3DDevice9::SetScissorRect(CONST RECT *pRect)
{
   return origIDirect3DDevice9->SetScissorRect(pRect);
}

HRESULT proxyIDirect3DDevice9::GetScissorRect(RECT *pRect)
{
   return origIDirect3DDevice9->GetScissorRect(pRect);
}

HRESULT proxyIDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
   return origIDirect3DDevice9->SetSoftwareVertexProcessing(bSoftware);
}

BOOL proxyIDirect3DDevice9::GetSoftwareVertexProcessing(void)
{
   return origIDirect3DDevice9->GetSoftwareVertexProcessing();
}

HRESULT proxyIDirect3DDevice9::SetNPatchMode(float nSegments)
{
   return origIDirect3DDevice9->SetNPatchMode(nSegments);
}

float proxyIDirect3DDevice9::GetNPatchMode(void)
{
   return origIDirect3DDevice9->GetNPatchMode();
}

HRESULT proxyIDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
   return origIDirect3DDevice9->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT proxyIDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return origIDirect3DDevice9->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT proxyIDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
   return origIDirect3DDevice9->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT proxyIDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
   return origIDirect3DDevice9->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT proxyIDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9 *pDestBuffer, IDirect3DVertexDeclaration9 *pVertexDecl, DWORD Flags)
{
   return origIDirect3DDevice9->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT proxyIDirect3DDevice9::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9 *pVertexElements, IDirect3DVertexDeclaration9 **ppDecl)
{
   return origIDirect3DDevice9->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT proxyIDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
   return origIDirect3DDevice9->SetVertexDeclaration(pDecl);
}

HRESULT proxyIDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)
{
   return origIDirect3DDevice9->GetVertexDeclaration(ppDecl);
}

HRESULT proxyIDirect3DDevice9::SetFVF(DWORD FVF)
{
   return origIDirect3DDevice9->SetFVF(FVF);
}

HRESULT proxyIDirect3DDevice9::GetFVF(DWORD *pFVF)
{
   return origIDirect3DDevice9->GetFVF(pFVF);
}

HRESULT proxyIDirect3DDevice9::CreateVertexShader(CONST DWORD *pFunction, IDirect3DVertexShader9 **ppShader)
{
   return origIDirect3DDevice9->CreateVertexShader(pFunction, ppShader);
}

HRESULT proxyIDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
   return origIDirect3DDevice9->SetVertexShader(pShader);
}

HRESULT proxyIDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
   return origIDirect3DDevice9->GetVertexShader(ppShader);
}

HRESULT proxyIDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister, CONST float *pConstantData, UINT Vector4fCount)
{
   return origIDirect3DDevice9->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT proxyIDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister, float *pConstantData, UINT Vector4fCount)
{
   return origIDirect3DDevice9->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT proxyIDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister, CONST int *pConstantData, UINT Vector4iCount)
{
   return origIDirect3DDevice9->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT proxyIDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister, int *pConstantData, UINT Vector4iCount)
{
   return origIDirect3DDevice9->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT proxyIDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT  BoolCount)
{
   return origIDirect3DDevice9->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT proxyIDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL *pConstantData, UINT BoolCount)
{
   return origIDirect3DDevice9->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT proxyIDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride)
{
   return origIDirect3DDevice9->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT proxyIDirect3DDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 **ppStreamData, UINT *OffsetInBytes, UINT *pStride)
{
   return origIDirect3DDevice9->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
}

HRESULT proxyIDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT Divider)
{
   return origIDirect3DDevice9->SetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT proxyIDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT *Divider)
{
   return origIDirect3DDevice9->GetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT proxyIDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{
   return origIDirect3DDevice9->SetIndices(pIndexData);
}

HRESULT proxyIDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData)
{
   return origIDirect3DDevice9->GetIndices(ppIndexData);
}

HRESULT proxyIDirect3DDevice9::CreatePixelShader(CONST DWORD *pFunction, IDirect3DPixelShader9 **ppShader)
{
   return origIDirect3DDevice9->CreatePixelShader(pFunction, ppShader);
}

HRESULT proxyIDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
   return origIDirect3DDevice9->SetPixelShader(pShader);
}

HRESULT proxyIDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
   return origIDirect3DDevice9->GetPixelShader(ppShader);
}

HRESULT proxyIDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister, CONST float *pConstantData, UINT Vector4fCount)
{
   return origIDirect3DDevice9->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT proxyIDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister, float *pConstantData, UINT Vector4fCount)
{
   return origIDirect3DDevice9->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT proxyIDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister, CONST int *pConstantData, UINT Vector4iCount)
{
   return origIDirect3DDevice9->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT proxyIDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister, int *pConstantData, UINT Vector4iCount)
{
   return origIDirect3DDevice9->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT proxyIDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL *pConstantData, UINT  BoolCount)
{
   return origIDirect3DDevice9->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT proxyIDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL *pConstantData, UINT BoolCount)
{
   return origIDirect3DDevice9->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT proxyIDirect3DDevice9::DrawRectPatch(UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo)
{
   return origIDirect3DDevice9->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT proxyIDirect3DDevice9::DrawTriPatch(UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo)
{
   return origIDirect3DDevice9->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT proxyIDirect3DDevice9::DeletePatch(UINT Handle)
{
   return origIDirect3DDevice9->DeletePatch(Handle);
}

HRESULT proxyIDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9 **ppQuery)
{
   return origIDirect3DDevice9->CreateQuery(Type, ppQuery);
}


void toggleWindowedMode(void)
{
	traceLastFunc("toggleWindowedMode()");

	g_isRequestingWindowModeToggle = true;
	g_isRequesting_RwD3D9ChangeVideoMode = true;
	proxyID3DDevice9_InitWindowMode(&pPresentParam);
	
}
