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
#include "main.h"

// ---------------------------------------------------
CVector *GravityNormal = new CVector( 0.0, 0.0, -1.0 );

// ---------------------------------------------------
// new gravity hook
#define HOOKPOS_CPhysical_ApplyGravity	0x543081
DWORD	RETURN_CPhysical_ApplyGravity = 0x543093;

void CPhysical_ApplyGravity ( DWORD dwThis )
{
	traceLastFunc( "CPhysical_ApplyGravity()" );

	// dwThis should be coming from HOOK_CPhysical_ApplyGravity
	DWORD	dwType;
	__asm
	{
		mov ecx, dwThis
		mov eax, 0x46A2C0	// CEntity::GetType
		call eax
		mov dwType, eax
	}

	float	fTimeStep = *(float *)0xB7CB5C;
	float	fGravity = *(float *)0x863984;

	if ( dwType == 2 )
	{
		// It's a vehicle
		CVehicle	*pVehicle = pPools->GetVehicle( (DWORD *)dwThis );
		if ( !pVehicle )
		{
			// apply regular downward gravity
			*(float *)( dwThis + 0x4C ) -= fTimeStep * fGravity;
			return;
		}

		CPed	*pPedSelf = getSelfCPed();
		if ( pVehicle->GetDriver() == pPedSelf || pVehicle->IsPassenger(pPedSelf) )
		{
			// we're in the vehicle
			if ( !cheat_state->vehicle.air_brake )
			{
				// use our gravity vector
				CVector vecGravity, vecMoveSpeed;
				pVehicle->GetGravity( &vecGravity );
				pVehicle->GetMoveSpeed( &vecMoveSpeed );
				vecMoveSpeed += vecGravity * fTimeStep * fGravity;
				pVehicle->SetMoveSpeed( &vecMoveSpeed );
			}
		}
		else if ( pVehicle->GetTowedByVehicle() )
		{
			if ( pVehicle->GetTowedByVehicle()->GetDriver() == pPedSelf
			 ||	 pVehicle->GetTowedByVehicle()->IsPassenger(pPedSelf) )
			{
				// It's our trailer, use our gravity vector
				CVector vecGravity, vecMoveSpeed;
				pVehicle->GetGravity( &vecGravity );
				pVehicle->GetMoveSpeed( &vecMoveSpeed );
				vecMoveSpeed += vecGravity * fTimeStep * fGravity;
				pVehicle->SetMoveSpeed( &vecMoveSpeed );
			}
			else
			{
				// apply regular downward gravity
				*(float *)( dwThis + 0x4C ) -= fTimeStep * fGravity;
			}
		}
		else
		{
			// apply regular downward gravity
			*(float *)( dwThis + 0x4C ) -= fTimeStep * fGravity;
		}
	}
	else if ( dwType == 3 )
	{
		// It's a ped
		CPed	*pPed = pPools->GetPed( (DWORD *)dwThis );
		CPed	*pPedSelf = getSelfCPed();

		if ( pPed == pPedSelf )
		{
			if ( cheat_state->actor.air_brake )
			{
				// don't apply gravity
			}
			else
			{
				// apply regular downward gravity
				*(float *)( dwThis + 0x4C ) -= fTimeStep * fGravity;
			}

			/*
			// old SpiderFeet not-very-working code
			CVector vecGravity = cheat_state->actor.gravityVector;
			CVector vecMoveSpeed = GTAfunc_GetMoveSpeed(&ainfo_self->base);
			vecMoveSpeed += vecGravity * fTimeStep * fGravity;
			GTAfunc_SetMoveSpeed(&ainfo_self->base, vecMoveSpeed);
			*/
		}
		else
		{
			// apply regular downward gravity
			*(float *)( dwThis + 0x4C ) -= fTimeStep * fGravity;
		}
	}
	else
	{
		// apply regular downward gravity (+0x4C == m_vecMoveSpeed.fZ)
		*(float *)( dwThis + 0x4C ) -= fTimeStep * fGravity;
	}
}

uint8_t _declspec ( naked ) HOOK_CPhysical_ApplyGravity ( void )
{
	__asm
	{
		push esi
		call CPhysical_ApplyGravity
		add esp, 4
		jmp RETURN_CPhysical_ApplyGravity
	}
}

// ---------------------------------------------------
CMatrix gravcam_matGravity;
CMatrix gravcam_matInvertGravity;
CMatrix gravcam_matVehicleTransform;
CVector gravcam_vecVehicleVelocity;
#define HOOKPOS_VehicleCamStart 0x5245ED
DWORD	RETURN_VehicleCamStart_success = 0x5245F3;
DWORD	RETURN_VehicleCamStart_failure = 0x525E43;
#define HOOKPOS_VehicleCamTargetZTweak	0x524A68
DWORD	RETURN_VehicleCamTargetZTweak = 0x524AA4;
#define HOOKPOS_VehicleCamLookDir1	0x524DF1
DWORD	RETURN_VehicleCamLookDir1 = 0x524DF6;
#define HOOKPOS_VehicleCamLookDir2	0x525B0E
DWORD	RETURN_VehicleCamLookDir2 = 0x525B73;
#define HOOKPOS_VehicleCamHistory	0x525C56
DWORD	RETURN_VehicleCamHistory = 0x525D4A;
#define HOOKPOS_VehicleCamColDetect 0x525D8D
DWORD	RETURN_VehicleCamColDetect = 0x525D92;
#define CALL_VehicleCamUp		0x525E1F
#define HOOKPOS_VehicleCamEnd	0x525E3C
DWORD	RETURN_VehicleCamEnd = 0x525E42;
#define HOOKPOS_VehicleLookBehind	0x5207E3
DWORD	RETURN_VehicleLookBehind = 0x520891;
#define CALL_VehicleLookBehindUp	0x520E2A
#define HOOKPOS_VehicleLookAside	0x520F70
DWORD	RETURN_VehicleLookAside = 0x520FDC;
#define CALL_VehicleLookAsideUp 0x5211E0

bool _cdecl VehicleCamStart ( DWORD dwCam, DWORD pVehicleInterface )
{
	traceLastFunc( "VehicleCamStart()" );
	// Inverse transform some things so that they match a downward pointing gravity.
	// This way SA's gravity-goes-downward assumptive code can calculate the camera
	// spherical coords correctly. Of course we restore these after the camera function
	// completes.
	CVehicle	*pVehicle = pPools->GetVehicle( (DWORD *)pVehicleInterface );

	if ( !pVehicle )
		return false;

	CVector vecGravity;
	pVehicle->GetGravity( &vecGravity );

	GetMatrixForGravity( vecGravity, gravcam_matGravity );
	gravcam_matInvertGravity = gravcam_matGravity;
	gravcam_matInvertGravity.Invert();

	pVehicle->GetMatrix( &gravcam_matVehicleTransform );

	CMatrix matVehicleInverted = gravcam_matInvertGravity * gravcam_matVehicleTransform;
	matVehicleInverted.vPos = gravcam_matVehicleTransform.vPos;
	pVehicle->SetMatrix( &matVehicleInverted );

	pVehicle->GetMoveSpeed( &gravcam_vecVehicleVelocity );

	CVector vecVelocityInverted = gravcam_matInvertGravity * gravcam_vecVehicleVelocity;
	pVehicle->SetMoveSpeed( &vecVelocityInverted );
	return true;
}

void _declspec ( naked ) HOOK_VehicleCamStart ()
{
	_asm
	{
		push edi
		push esi
		call VehicleCamStart
		add esp, 8

		test eax, eax
		jz fail
		mov eax, [edi + 0x460]
		jmp RETURN_VehicleCamStart_success

		fail :
		add esp, 4
		jmp RETURN_VehicleCamStart_failure
	}
}

// ---------------------------------------------------
void _cdecl VehicleCamTargetZTweak ( CVector *pvecCamTarget, float fTargetZTweak )
{
	traceLastFunc( "VehicleCamTargetZTweak()" );
	// Replacement for "vecCamTarget = vecCarPosition + (0, 0, 1)*fZTweak"
	*pvecCamTarget += gravcam_matGravity.vUp * fTargetZTweak;
}

void _declspec ( naked ) HOOK_VehicleCamTargetZTweak ()
{
	_asm
	{
		fstp st

		lea eax, [esp + 0x48]
		push[esp + 0x30]
		push eax
		call VehicleCamTargetZTweak
		add esp, 8

		fld[esp + 0x30]
		fadd[esp + 0x7C]
		fstp[esp + 0x7C]
		fld ds : [0x8CCEDC]
		fdiv[esp + 0x7C]
		fmul[esp + 0x30]
		fadd[esp + 0x1C]
		fstp[esp + 0x1C]

		mov eax, ds : [0xB6F0DC]
		cmp eax, 1
		jmp RETURN_VehicleCamTargetZTweak
	}
}

// ---------------------------------------------------
void _cdecl VehicleCamLookDir1 ( DWORD dwCam, DWORD pVehicleInterface )
{
	traceLastFunc( "VehicleCamLookDir1()" );
	// For the same reason as in VehicleCamStart, inverse transform the camera's lookdir
	// at this point
	CVector *pvecLookDir = ( CVector * ) ( dwCam + 0x190 );
	*pvecLookDir = gravcam_matInvertGravity * ( *pvecLookDir );
}

void _declspec ( naked ) HOOK_VehicleCamLookDir1 ()
{
	_asm
	{
		mov eax, 0x59C910	// CVector::Normalise
		call eax

		push edi
		push esi
		call VehicleCamLookDir1
		add esp, 8

		jmp RETURN_VehicleCamLookDir1
	}
}

// ---------------------------------------------------
bool _cdecl VehicleCamLookDir2 ( DWORD dwCam )
{
	traceLastFunc( "VehicleCamLookDir2()" );
	// Calculates the look direction vector for the vehicle camera. This vector
	// is later multiplied by a factor and added to the vehicle position by SA
	// to obtain the final camera position.
	float	fPhi = *(float *)( dwCam + 0xBC );
	float	fTheta = *(float *)( dwCam + 0xAC );

	*( CVector * ) ( dwCam + 0x190 ) = -gravcam_matGravity.vRight *
		cos( fPhi ) *
		cos( fTheta ) -
		gravcam_matGravity.vFront *
		sin( fPhi ) *
		cos( fTheta ) +
		gravcam_matGravity.vUp *
		sin( fTheta );

	*(float *)0x8CCEA8 = fPhi;
	return true;
}

void _declspec ( naked ) HOOK_VehicleCamLookDir2 ()
{
	_asm
	{
		push esi
		call VehicleCamLookDir2
		add esp, 4

		lea ebx, [esi + 0x190]
		lea ebp, [esi + 0x19C]
		push 4
		jmp RETURN_VehicleCamLookDir2
	}
}

// ---------------------------------------------------
void _cdecl VehicleCamHistory ( DWORD dwCam, CVector *pvecTarget, float fTargetTheta, float fRadius, float fZoom )
{
	traceLastFunc( "VehicleCamHistory()" );
	float	fPhi = *(float *)( dwCam + 0xBC );
	CVector vecDir = -gravcam_matGravity.vRight * cos( fPhi ) * cos( fTargetTheta ) - gravcam_matGravity.vFront * sin( fPhi ) * cos( fTargetTheta ) + gravcam_matGravity.vUp * sin( fTargetTheta );
	( (CVector *) (dwCam + 0x1D8) )[0] = *pvecTarget - vecDir * fRadius;
	( (CVector *) (dwCam + 0x1D8) )[1] = *pvecTarget - vecDir * fZoom;
}

void _declspec ( naked ) HOOK_VehicleCamHistory ()
{
	_asm
	{
		push[esp + 0x0 + 0x7C]	// zoom
		push[esp + 0x4 + 0x2C]	// radius
		push[esp + 0x8 + 0x14]	// targetTheta
		lea eax, [esp + 0xC + 0x48]
		push eax	// pvecTarget
		push esi	// pCam
		call VehicleCamHistory
		add esp, 0x14

		mov eax, [esp + 0x24]
		jmp RETURN_VehicleCamHistory
	}
}

// ---------------------------------------------------
void _cdecl VehicleCamUp ( DWORD dwCam )
{
	traceLastFunc( "VehicleCamUp()" );
	// Calculates the up vector for the vehicle camera.
	CVector *pvecUp = ( CVector * ) ( dwCam + 0x1B4 );
	CVector *pvecLookDir = ( CVector * ) ( dwCam + 0x190 );

	pvecLookDir->Normalize();
	*pvecUp = *pvecLookDir;
	pvecUp->CrossProduct( &gravcam_matGravity.vUp );
	pvecUp->CrossProduct( pvecLookDir );
}

void _declspec ( naked ) HOOK_VehicleCamUp ()
{
	_asm
	{
		mov edx, ecx
		mov ecx, [ecx + 0x21C]	// CCam::pTargetEntity
		mov eax, 0x46A2C0		// CEntity::GetType
		call eax

		cmp al, 2			// Is it a vehicle?
		jz docustom

		mov ecx, edx
		mov eax, 0x509CE0	// CCam::GetVectorsReadyForRW
		jmp eax

		docustom :
		push edx
		call VehicleCamUp
		add esp, 4
		ret
	}
}

// ---------------------------------------------------
void _cdecl VehicleCamEnd ( DWORD pVehicleInterface )
{
	traceLastFunc( "VehicleCamEnd()" );
	// Restore the things that we inverse transformed in VehicleCamStart
	CVehicle	*pVehicle = pPools->GetVehicle( (DWORD *)pVehicleInterface );
	if ( !pVehicle )
		return;
	traceLastFunc( "VehicleCamEnd() has CVehicle*" );
	pVehicle->SetMatrix( &gravcam_matVehicleTransform );
	traceLastFunc( "VehicleCamEnd() matrix has been set" );
	pVehicle->SetMoveSpeed( &gravcam_vecVehicleVelocity );
	traceLastFunc( "VehicleCamEnd() move speed has been set, returning" );
}

void _declspec ( naked ) HOOK_VehicleCamEnd ()
{
	_asm
	{
		mov ds : [0xB6F020], edx

		push edi
		call VehicleCamEnd
		add esp, 4

		jmp RETURN_VehicleCamEnd
	}
}

// ---------------------------------------------------
void _cdecl VehicleLookBehind ( DWORD dwCam, CVector *pvecEntityPos, float fDistance )
{
	traceLastFunc( "VehicleLookBehind()" );
	// Custom calculation of the camera position when looking behind while in
	// vehicle cam mode, taking in account custom gravity
	*( CVector * ) ( dwCam + 0x19C ) = *pvecEntityPos + ( gravcam_matVehicleTransform.vFront + gravcam_matGravity.vUp * 0.2f ) * fDistance;
}

void _declspec ( naked ) HOOK_VehicleLookBehind ()
{
	_asm
	{
		push[esp + 0x14]
		lea eax, [esp + 4 + 0x1C]
		push eax
		push edi
		call VehicleLookBehind
		add esp, 0xC

		mov eax, [esp + 0x1C]		// vecTemp = vecEntityPos
		mov ecx, [esp + 0x1C + 4]
		mov edx, [esp + 0x1C + 8]
		mov[esp + 0x34], eax
		mov[esp + 0x34 + 4], ecx
		mov[esp + 0x34 + 8], edx

		mov ds : [0xB7CD68], ebx	// IgnoreEntity
		mov ds : [0xB6FC70], 0		// NumExtraIgnoreEntities
		mov eax, ebx	// pEntity
		jmp RETURN_VehicleLookBehind
	}
}

// ---------------------------------------------------
void _cdecl VehicleLookAside ( DWORD dwCam, CVector *pvecEntityPos, float fDirectionFactor, float fDistance )
{
	traceLastFunc( "VehicleLookAside" );
	// Custom calculation of the camera position when looking left/right while in
	// vehicle cam mode, taking in account custom gravity
	*( CVector * ) ( dwCam + 0x19C ) = *pvecEntityPos + ( -gravcam_matVehicleTransform.vRight * fDirectionFactor + gravcam_matGravity.vUp * 0.2f ) * fDistance;
}

void _declspec ( naked ) HOOK_VehicleLookAside ()
{
	_asm
	{
		push[esp + 0x14]
		push[esp + 4 + 0x1C]
		lea eax, [esp + 8 + 0x20]
		push eax
		push esi
		call VehicleLookAside
		add esp, 0x10

		lea ebp, [esi + 0x19C]
		mov ecx, [esi + 0x21C]
		jmp RETURN_VehicleLookAside
	}
}

// ---------------------------------------------------
void _cdecl CVehicle_constructor_hook ( CVehicleSAInterface *vehicle )
{
	// create & add new CVehicle to CPools
	CVehicle	*CVeh = pGameInterface->GetPools()->AddVehicle( (DWORD *)vehicle );
	CVeh->SetGravity( GravityNormal );
}

#define HOOKPOS_CVehicle_constructor	0x6D6259
DWORD	RETURN_CVehicle_constructor = 0x6D6263;
void _declspec ( naked ) HOOK_CVehicle_constructor ()
{
	__asm
	{
		// perform over-written asm
		mov fs : 0, ecx
		add esp, 0x10

		// call handler
		push eax
		call CVehicle_constructor_hook
		add esp, 0x4

		// return
		jmp RETURN_CVehicle_constructor
	}
}

void _cdecl CVehicle_destructor_hook ( CVehicleSAInterface *vehicle )
{
	// remove CVehicle from CPools and delete
	CVehicle	*CVeh_toDelete = pGameInterface->GetPools()->GetVehicle( (DWORD *)vehicle );
	pGameInterface->GetPools()->RemoveVehicle( CVeh_toDelete, false );
}

#define HOOKPOS_CVehicle_destructor 0x6E2B40
DWORD	RETURN_CVehicle_destructor = 0x6E2B47;
void _declspec ( naked ) HOOK_CVehicle_destructor ()
{
	__asm
	{
		// call handler
		push ecx
		call CVehicle_destructor_hook
		pop ecx

		// perform over-written asm
		push 0x0FFFFFFFF
		push 0x848826

		// return
		jmp RETURN_CVehicle_destructor
	}
}

// ---------------------------------------------------
void _cdecl CPed_constructor_hook ( CPedSAInterface *ped )
{
	// create & add new CPed to CPools
	pGameInterface->GetPools()->AddPed( (DWORD *)ped );
}

#define HOOKPOS_CPed_constructor	0x5E8606
DWORD	RETURN_CPed_constructor = 0x5E8610;
void _declspec ( naked ) HOOK_CPed_constructor ()
{
	__asm
	{
		// perform over-written asm
		mov fs : 0, ecx
		add esp, 0x28

		// call handler
		push eax
		call CPed_constructor_hook

		//pop eax
		add esp, 0x4

		// return
		jmp RETURN_CPed_constructor
	}
}

void _cdecl CPed_destructor_hook ( CPedSAInterface *ped )
{
	// remove CPed from CPools and delete
	CPed	*CPed_toDelete = pGameInterface->GetPools()->GetPed( (DWORD *)ped );
	pGameInterface->GetPools()->RemovePed( CPed_toDelete, false );
}

#define HOOKPOS_CPed_destructor 0x5E8620
DWORD	RETURN_CPed_destructor = 0x5E8627;
void _declspec ( naked ) HOOK_CPed_destructor ()
{
	__asm
	{
		// call handler
		push ecx
		call CPed_destructor_hook
		pop ecx

		// perform over-written asm
		push 0x0FFFFFFFF
		push 0x83DA5A

		// return
		jmp RETURN_CPed_destructor
	}
}

// ---------------------------------------------------
// ---------------------------------------------------
// ---------------------------------------------------

// hook installers
void cheat_hookers_installhooks ( void )
{
	// hooks
	HookInstall( HOOKPOS_VehicleCamStart, (DWORD) HOOK_VehicleCamStart, 6 );
	HookInstall( HOOKPOS_VehicleCamTargetZTweak, (DWORD) HOOK_VehicleCamTargetZTweak, 8 );
	HookInstall( HOOKPOS_VehicleCamLookDir1, (DWORD) HOOK_VehicleCamLookDir1, 5 );
	HookInstall( HOOKPOS_VehicleCamLookDir2, (DWORD) HOOK_VehicleCamLookDir2, 6 );
	HookInstall( HOOKPOS_VehicleCamHistory, (DWORD) HOOK_VehicleCamHistory, 6 );
	HookInstall( HOOKPOS_VehicleCamEnd, (DWORD) HOOK_VehicleCamEnd, 6 );
	HookInstall( HOOKPOS_VehicleLookBehind, (DWORD) HOOK_VehicleLookBehind, 6 );
	HookInstall( HOOKPOS_VehicleLookAside, (DWORD) HOOK_VehicleLookAside, 6 );
	HookInstall( HOOKPOS_CPhysical_ApplyGravity, (DWORD) HOOK_CPhysical_ApplyGravity, 6 );
	HookInstall( HOOKPOS_CVehicle_constructor, (DWORD) HOOK_CVehicle_constructor, 6 );
	HookInstall( HOOKPOS_CVehicle_destructor, (DWORD) HOOK_CVehicle_destructor, 6 );
	HookInstall( HOOKPOS_CPed_constructor, (DWORD) HOOK_CPed_constructor, 6 );
	HookInstall( HOOKPOS_CPed_destructor, (DWORD) HOOK_CPed_destructor, 6 );

	// calls
	HookInstallCall( CALL_VehicleCamUp, (DWORD) HOOK_VehicleCamUp );
	HookInstallCall( CALL_VehicleLookBehindUp, (DWORD) HOOK_VehicleCamUp );
	HookInstallCall( CALL_VehicleLookAsideUp, (DWORD) HOOK_VehicleCamUp );
}
