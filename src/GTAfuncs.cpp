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
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/GTAfuncs.cpp $
	$Id: GTAfuncs.cpp 43 2010-01-05 07:40:59Z futnucks $

*/


#include "main.h"

int GTAfunc_isModelLoaded(int iModelID)
{
	int *ModelsLoadStateArray = (int*)0x8E4CD0;
	return ModelsLoadStateArray[5*iModelID];
}

void GTAfunc_requestModelLoad(int iModelID)
{
	if(iModelID < 0) return;
	__asm
	{
		push 2
		push iModelID
		mov edx, FUNC_RequestModel
		call edx
		pop edx
		pop edx
	}
}

void GTAfunc_loadRequestedModels(void)
{
	uint32_t func_load = FUNC_LoadAllRequestedModels;
	__asm
	{
		push 0
		call func_load
		add esp, 4
	}
}

bool GTAfunc_IsUpgradeAvailable(int m_dwModelID, eVehicleUpgradePosn posn)
{
    bool bRet = false;
    DWORD ModelID = m_dwModelID;
    __asm
    {
        mov     eax, ModelID
        lea     ecx, ARRAY_ModelInfo[eax*4]

        mov     eax, posn
        mov     ecx, [ecx+0x5C]
        shl     eax, 5
        push    esi
        mov     esi, [ecx+eax+0D0h]
        xor     edx, edx
        test    esi, esi
        setnl   dl
        mov     al, dl
        pop     esi

        mov     bRet, al
    }
    return bRet;
}

void GTAfunc_requestAnyVehicleUpgrade(vehicle_info *vinfo, int iModelID)
{
	if(vinfo == NULL) return;
	DWORD dwFunction = FUNC_RequestAnyVehicleUpgrade;
	__asm
	{
		mov ecx, vinfo
		push iModelID
		call dwFunction
	}
}

void GTAfunc_removeVehicleUpgrade(vehicle_info *vinfo, int iModelID)
{
	if(vinfo == NULL) return;
	uint32_t func_upgrade = 0x6DF930;
	__asm
	{
		mov     ecx, vinfo
		push    iModelID
		call    func_upgrade
	}
}

// turn off annoying performance warning
#pragma warning ( disable : 4800 ) 

bool GTAfunc_vehicle_iModelID_IsCar(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsCarModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsBike(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsBikeModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsPlane(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsPlaneModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsHeli(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsHeliModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsBoat(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsBoatModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsQuadBike(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsQuadBikeModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsBmx(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsBmxModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsMonsterTruck(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsMonsterTruckModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsTrailer(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsTrailerModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

bool GTAfunc_vehicle_iModelID_IsTrain(int m_dwModelID)
{
	DWORD dwFunction = FUNC_IsTrainModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	__asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (bool)bReturn;
}

// re-enable annoying performance warning
#pragma warning ( default : 4800 )

// get paintjob
int GTAfunc_vehicle_getRemapIndex(vehicle_info *vinfo)
{
    DWORD dwFunc = FUNC_CVehicle__GetRemapIndex;
    int iReturn = 0;
    __asm
    {
        mov     ecx, vinfo
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}

// set paintjob
void GTAfunc_vehicle_setRemap(vehicle_info *vinfo, int iRemap)
{
	DWORD dwFunc = FUNC_CVehicle__SetRemap;
	__asm
	{
		mov     ecx, vinfo
		push    iRemap
		call    dwFunc
	}
}

int GTAfunc_getNumVideoModes(void)
{
    int uiReturn = 0;
	DWORD dwFunc = FUNC_GetNumVideoModes;
    __asm
    {
        call    dwFunc
        mov     uiReturn, eax
    }
    return uiReturn;
}

VideoMode *GTAfunc_getVideoModeInfo(VideoMode *modeInfo, int modeIndex)
{
	DWORD dwFunc = FUNC_GetVideoModeInfo;
    __asm
    {
        push    modeIndex
        push    modeInfo
        call    dwFunc
        add     esp, 8
    }
    return modeInfo;
}

int GTAfunc_getCurrentVideoMode(void)
{
    int uiReturn = 0;
	DWORD dwFunc = FUNC_GetCurrentVideoMode;
    __asm
    {
        call    dwFunc
        mov     uiReturn, eax
    }
    return uiReturn;
}

void GTAfunc_setCurrentVideoMode(int modeIndex)
{
	DWORD dwFunc = FUNC_SetCurrentVideoMode;
    __asm
    {
        push    modeIndex
        call    dwFunc
        add     esp, 4
    }
}

int GTAfunc_RwD3D9ChangeVideoMode(int modeIndex)
{
	DWORD dwFunc = FUNC_RwD3D9ChangeVideoMode;
    __asm
    {
        push    modeIndex
        call    dwFunc
        add     esp, 4
    }
}
