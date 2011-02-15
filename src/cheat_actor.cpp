/*

	PROJECT:		mod_sa
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:		Copyright we_sux

	mod_sa is available from http://code.google.com/p/m0d-s0beit-sa/

	mod_sa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	mod_sa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with mod_sa.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "main.h"


// non-global vector pointer
CVector UpNormal ( 0.0, 0.0, 1.0 );

// new function to help converting from actor_info->base to CEntitySAInterface
CEntitySAInterface *cheat_actor_GetCEntitySAInterface ( actor_info *ainfo )
{
	return (CEntitySAInterface *)ainfo;
}

//*p_CEntitySAInterface = (CEntitySAInterface*)ainfo->base;
void cheat_actor_teleport ( struct actor_info *info, const float pos[3], int interior_id )
{
	if ( info == NULL )
		return;
	vect3_zero( info->speed );
	vect3_copy( pos, &info->base.matrix[4 * 3] );
	gta_interior_id_set( interior_id );
}

static struct patch_set patch_gta_auto_aim =
{
	"GTA: Autoaim",
	0,
	0,
	{ { 1, (void *)0x00B6EC2E, (uint8_t *)"\x01", (uint8_t *)"\x00", NULL }, { 1, (void *)0x00BA6818, (uint8_t *)"\x00",
				(uint8_t *)"\x01", NULL }, { 5, (void *)0x00524013,
					(uint8_t *)"\xA0\x2E\xEC\xB6\x00", NULL, NULL }, { 5, (void *)0x00523F3E,
						(uint8_t *)"\xA0\x2E\xEC\xB6\x00", NULL, NULL }, { 5, (void *)0x00525615,
							(uint8_t *)"\xA0\x2E\xEC\xB6\x00", NULL, NULL }, { 5, (void *)0x005221FC,
								(uint8_t *)"\xA0\x2E\xEC\xB6\x00", NULL, NULL }, { 6, (void *)0x0060E08E,
									(uint8_t *)"\x0F\x85\x79\x02\x00\x00", NULL, NULL }, { 6, (void *)0x0060E6B1,
										(uint8_t *)"\x0F\x85\xAC\x00\x00\x00", NULL, NULL }
	}
};

void cheat_handle_actor_autoaim ( struct actor_info *info, double time_diff )
{
	traceLastFunc( "cheat_handle_actor_autoaim()" );

	// toggle aimbot on/off
	if ( KEY_PRESSED(set.key_autoaim_toggle) )
	{
		cheat_state->actor.autoaim ^= 1;
		if ( set.use_gta_autoaim )
		{
			if ( cheat_state->actor.autoaim == 1 )
			{
				// set to default value, in case joypad aiming already activated
				*(char *)0x00B6EC2E = 1;
				*(char *)0x00BA6818 = 0;
				patcher_install( &patch_gta_auto_aim );
			}
			else
			{
				patcher_remove( &patch_gta_auto_aim );
			}
		}
	}

	if ( !set.use_gta_autoaim )
	{
		// should we be trying to aim or not?
		bool						isAimKeyDown = false;
		CControllerConfigManager	*pPadConfig = pGameInterface->GetControllerConfigManager();

		// doesnt seem to work in single player with pPadConfig and keyboard input?
		if ( pPadConfig->GetInputType() )
		{
			// mouse + keyboard
			if ( KEY_DOWN(pPadConfig->GetControllerKeyAssociatedWithAction(FIRE, MOUSE)) )
			{
				isAimKeyDown = true;
			}
			else if ( KEY_DOWN(pPadConfig->GetControllerKeyAssociatedWithAction(FIRE, KEYBOARD)) )
			{
				isAimKeyDown = true;
			}
		}
		else
		{
			// gamepad
			if ( KEY_DOWN(pPadConfig->GetControllerKeyAssociatedWithAction(FIRE, JOYSTICK)) )
			{
				isAimKeyDown = true;
			}
		}

		// let's aim, shall we?
		if ( cheat_state->actor.autoaim && isAimKeyDown )
		{
			// only for certain weapons
			eWeaponSlot selfSlot = pPedSelf->GetCurrentWeaponSlot();
			switch ( selfSlot )
			{
			case WEAPONSLOT_TYPE_UNARMED:
			case WEAPONSLOT_TYPE_MELEE:
			case WEAPONSLOT_TYPE_THROWN:
			case WEAPONSLOT_TYPE_SPECIAL:
			case WEAPONSLOT_TYPE_GIFT:
			case WEAPONSLOT_TYPE_PARACHUTE:
			case WEAPONSLOT_TYPE_DETONATOR:
				// we don't want to aim for these weapons
				return;
			//case WEAPONSLOT_TYPE_HANDGUN:
			//case WEAPONSLOT_TYPE_SHOTGUN:
			//case WEAPONSLOT_TYPE_SMG:
			//case WEAPONSLOT_TYPE_MG:
			//case WEAPONSLOT_TYPE_RIFLE:
			//case WEAPONSLOT_TYPE_HEAVY:
			}


/*
			// NEW OLD ASS AIM

			// settings
			float fRange = 300.0f;

			// variables
			CVector vecStart, vecTarget;

			// get the camera
			CCamera* pCamera = pGame->GetCamera ();

			//// Grab the active cam
			//CCam* pActive = pCamera->GetCam ( pCamera->GetActiveCam () );

			//// Grab the camera matrix
			//CMatrix matCamera;
			//pCamera->GetMatrix ( &matCamera );
			//ecStart = matCamera.vPos;

			//// Find the target position
			//CVector vecFront = *pActive->GetFront ();
			//vecFront.Normalize ();
			//vecTarget = *pActive->GetSource () + vecFront * fRange;

			// BONE_RIGHTHAND, BONE_RIGHTWRIST
			// Grab the gun muzzle position
			// this needs to also have the Z and left-right axis corrected and rotated
			// so the rear of the barrel is matched too
			eWeaponSlot eSlot = pPedSelf->GetCurrentWeaponSlot();
			CWeapon* pPlayerWeapon = pPedSelf->GetWeapon( eSlot );
			CWeaponInfo* pCurrentWeaponInfo = pPlayerWeapon->GetInfo();
			CVector vecGunMuzzle = *pCurrentWeaponInfo->GetFireOffset();
			pPedSelf->GetTransformedBonePosition( BONE_RIGHTWRIST, &vecGunMuzzle );

			// Grab the target point
			//pCamera->Find3rdPersonCamTargetVector( fRange, &vecGunMuzzle, &vecStart, &vecTarget );

			CVector vecRightWrist;
			pPedSelf->GetBonePosition( BONE_RIGHTWRIST, &vecRightWrist );

			CVector vecAimMult = vecGunMuzzle - vecRightWrist;
			vecAimMult.Normalize();
			CVector vecAimEnd = vecGunMuzzle + (vecAimMult * 40.0f);

			render->DrawLine( CVecToD3DXVEC(vecGunMuzzle), CVecToD3DXVEC(vecRightWrist), D3DCOLOR_ARGB(255, 0, 255, 0) );
			render->DrawLine( CVecToD3DXVEC(vecGunMuzzle), CVecToD3DXVEC(vecAimEnd), D3DCOLOR_ARGB(255, 255, 0, 0) );
*/


			// OLD ASS AIM
			static int			prev_id;
			static float		adj_rx, adj_rz, prev_rx, prev_rz;
			float				rx = *(float *)0x00B6F248;
			float				rz = *(float *)0x00B6F258;

			int					nearest_id = actor_find_nearest( ACTOR_ALIVE );
			struct actor_info	*nearest;
			float				vect[3], ax, az;

			if ( nearest_id == -1 )
			{
				cheat_state_text( "No players found; auto aim disabled." );
				cheat_state->actor.autoaim = 0;
				return;
			}

			if ( nearest_id == prev_id )
			{
				adj_rx += rx - prev_rx;
				adj_rz += rz - prev_rz;
			}

			prev_id = nearest_id;

			if ( (nearest = actor_info_get(nearest_id, ACTOR_ALIVE)) == NULL )
				return; // won't happen

			//cheat_state_text("%.3f %.3f %d %d", adj_rx, adj_rz, nearest->state, nearest->state_running);

			// calculate distance vector
			vect3_vect3_sub( &nearest->base.matrix[4 * 3], &info->base.matrix[4 * 3], vect );

			// z angle
			az = atan2f( vect[0], vect[1] );

			// rotate around z axis
			vect[1] = sinf( az ) * vect[0] + cosf( az ) * vect[1];

			// x angle
			ax = atan2f( vect[1], vect[2] );

			ax = -ax + M_PI / 2.0f + adj_rx;
			az = -az - M_PI / 2.0f + adj_rz;

			if ( ax < -M_PI )
				ax = -M_PI;
			else if ( ax > M_PI )
				ax = M_PI;

			// XXX make function
			prev_rx = *(float *)0x00B6F248 = ax;
			prev_rz = *(float *)0x00B6F258 = az;

			
			
/*
sprintf( buf, "m_fTrueAlpha: %.4f", pSelfWeaponCam1->GetInterface()->m_fTrueAlpha );
pD3DFontFixed->PrintShadow(450, 50 + lineSpace, D3DCOLOR_XRGB(0, 200, 0), buf);
lineSpace += 12;
sprintf( buf, "m_fTrueBeta: %.4f", pSelfWeaponCam1->GetInterface()->m_fTrueBeta );
pD3DFontFixed->PrintShadow(450, 50 + lineSpace, D3DCOLOR_XRGB(0, 200, 0), buf);
lineSpace += 12;
*/

		} // if ( cheat_state->actor.autoaim )
	} // if ( !set.use_gta_autoaim )
}

void cheat_handle_actor_air_brake ( struct actor_info *info, double time_diff )
{
	traceLastFunc( "cheat_handle_actor_air_brake()" );

	static float	orig_pos[3];
	static float	fall_speed_mult;
	static int		was_enabled;

	if ( set.air_brake_toggle )
	{
		if ( KEY_PRESSED(set.key_air_brake_foot_mod) )
			cheat_state->actor.air_brake ^= 1;

		if ( KEY_PRESSED(set.key_air_brake_mod2) && cheat_state->actor.air_brake )
			cheat_state->actor.air_brake_slowmo ^= 1;
	}
	else
	{
		if ( KEY_PRESSED(set.key_air_brake_foot_mod) )
			cheat_state->actor.air_brake = 1;
		else if ( KEY_RELEASED(set.key_air_brake_foot_mod) )
			cheat_state->actor.air_brake = 0;

		if ( KEY_PRESSED(set.key_air_brake_mod2) && cheat_state->actor.air_brake )
			cheat_state->actor.air_brake_slowmo = 1;
		else if ( KEY_RELEASED(set.key_air_brake_mod2) && cheat_state->actor.air_brake )
			cheat_state->actor.air_brake_slowmo = 0;
	}

	if ( !was_enabled && cheat_state->actor.air_brake )
	{
		vect3_copy( &info->base.matrix[4 * 3], orig_pos );
		fall_speed_mult = 1.0f;
		was_enabled = 1;
	}

	if ( !cheat_state->actor.air_brake )
	{
		was_enabled = 0;
		cheat_state->actor.air_brake_slowmo = 0;
	}
	else
	{
		float	*matrix = info->base.matrix;

		// if there's no parachute
		if ( !(info->weapon_slot == 11 && info->weapon[11].id == 46) )
		{
			vect3_copy( orig_pos, &info->base.matrix[4 * 3] );
			vect3_zero( info->speed );

			// new pedFlags
			info->pedFlags.bIsStanding = true;
			info->pedFlags.bWasStanding = true;
			info->pedFlags.bStayInSamePlace = true;

			static uint32_t time_start;
			float			d[4] = { 0.0f, 0.0f, 0.0f, time_diff * set.air_brake_speed };

			if ( cheat_state->actor.air_brake_slowmo )
				d[3] /= 10.0f;

			if ( KEY_DOWN(set.key_air_brake_forward) )
				d[0] += 1.0f;
			if ( KEY_DOWN(set.key_air_brake_backward) )
				d[0] -= 1.0f;
			if ( KEY_DOWN(set.key_air_brake_left) )
				d[1] += 1.0f;
			if ( KEY_DOWN(set.key_air_brake_right) )
				d[1] -= 1.0f;
			if ( KEY_DOWN(set.key_air_brake_up) )
				d[2] += 1.0f;
			if ( KEY_DOWN(set.key_air_brake_down) )
				d[2] -= 1.0f;

			if ( !near_zero(set.air_brake_accel_time) )
			{
				if ( !vect3_near_zero(d) )
					time_start = ( time_start == 0 ) ? time_get() : time_start;
				else
					time_start = 0; /* no keys pressed */

				/* acceleration */
				if ( time_start != 0 )
				{
					float	t = TIME_TO_FLOAT( time_get() - time_start );
					if ( t < set.air_brake_accel_time )
						d[3] *= t / set.air_brake_accel_time;
				}
			}

			if ( !vect3_near_zero(d) )
			{
				float	vect[4] = { -d[1], d[0], d[2], 0.0f };
				float	out[4];

				/* out = matrix * norm(d) */
				vect3_normalize( vect, vect );
				matrix_vect4_mult( matrix, vect, out );

				matrix[4 * 3 + 0] += out[0] * d[3];
				matrix[4 * 3 + 1] += out[1] * d[3];
				matrix[4 * 3 + 2] += out[2] * d[3];
			}
		}

		// parachute
		else
		{
			if ( KEY_DOWN(set.key_air_brake_up) )
				fall_speed_mult += time_diff / 2.0f;
			if ( KEY_DOWN(set.key_air_brake_down) )
				fall_speed_mult -= time_diff / 2.0f;

			if ( fall_speed_mult < 0.0f )
				fall_speed_mult = 0.0f;
			else if ( fall_speed_mult > 2.0f )
				fall_speed_mult = 2.0f;

			matrix[4 * 3 + 0] += info->speed[0] * time_diff * set.air_brake_speed;
			matrix[4 * 3 + 1] += info->speed[1] * time_diff * set.air_brake_speed;
			matrix[4 * 3 + 2] -= ( matrix[4 * 3 + 2] - orig_pos[2] ) * fall_speed_mult;
		}

		vect3_copy( &matrix[4 * 3], orig_pos );

		// heh
		int		gonadsMult = 1000;
		float	strifeMult = 0.00001f;
		int		gonads = rand() % gonadsMult;
		float	strife = (double)gonads * strifeMult;
		if ( strife < strifeMult * gonadsMult / 2 )
			strife -= strifeMult * gonadsMult;
		info->m_SpeedVec.fX = strife;
		gonads = rand() % gonadsMult;
		strife = (double)gonads * strifeMult;
		if ( strife < strifeMult * gonadsMult / 2 )
			strife -= strifeMult * gonadsMult;
		info->m_SpeedVec.fY = strife;
	}
}

CVector cheat_actor_getPositionUnder ( actor_info *ainfo )
{
	traceLastFunc( "cheat_vehicle_getPositionUnder()" );

	CVector offsetVector;
	float	*matrix = ainfo->base.matrix;
	offsetVector.fX = 0 * matrix[0] + 0 * matrix[4] - 1 * matrix[8];
	offsetVector.fY = 0 * matrix[1] + 0 * matrix[5] - 1 * matrix[9];
	offsetVector.fZ = 0 * matrix[2] + 0 * matrix[6] - 1 * matrix[10];
	return offsetVector;
}

void cheat_actor_setGravity ( actor_info *ainfo, CVector pvecGravity )
{
	traceLastFunc( "cheat_actor_setGravity()" );

	// set the d-dang gravity
	cheat_state->actor.gravityVector = pvecGravity;
}


/*
static CMatrix_Padded * mat_SpiderFeetCollisionTransform = new CMatrix_Padded();
static CMatrix_Padded * mat_SpiderFeetCollisionTransform_Original = (CMatrix_Padded*)0x968988;
uint8_t mat_SpiderFeetCollisionTransform_Offset[4] = {
	LOBYTE(LOWORD(&mat_SpiderFeetCollisionTransform)),
	HIBYTE(LOWORD(&mat_SpiderFeetCollisionTransform)),
	LOBYTE(HIWORD(&mat_SpiderFeetCollisionTransform)),
	HIBYTE(HIWORD(&mat_SpiderFeetCollisionTransform))
};
static struct patch_set patch_actor_SpiderFeetCollisionTransform =
{
	"SpiderFeet Collision Transform",
	0,
	0,
	{
		// bottom
		{ 4, (void *)0x004196D0, (uint8_t *)"\x88\x89\x96\x00", (uint8_t *)mat_SpiderFeetCollisionTransform_Offset, (uint8_t *)"\x88\x89\x96\x00" },
		{ 4, (void *)0x00419700, (uint8_t *)"\x88\x89\x96\x00", (uint8_t *)mat_SpiderFeetCollisionTransform_Offset, (uint8_t *)"\x88\x89\x96\x00" },
		{ 4, (void *)0x00418FB8, (uint8_t *)"\x88\x89\x96\x00", (uint8_t *)mat_SpiderFeetCollisionTransform_Offset, (uint8_t *)"\x88\x89\x96\x00" },
		// up 1
		{ 4, (void *)0x0041874E, (uint8_t *)"\x88\x89\x96\x00", (uint8_t *)mat_SpiderFeetCollisionTransform_Offset, (uint8_t *)"\x88\x89\x96\x00" },
		// up 2
		{ 4, (void *)0x004186AB, (uint8_t *)"\x88\x89\x96\x00", (uint8_t *)mat_SpiderFeetCollisionTransform_Offset, (uint8_t *)"\x88\x89\x96\x00" }
		//crash { 4, (void *)0x00418693 , (uint8_t *)"\x88\x89\x96\x00", (uint8_t *)mat_SpiderFeetCollisionTransform_Offset, (uint8_t *)"\x88\x89\x96\x00" },
		//crash { 4, (void *)0x00418681, (uint8_t *)"\x88\x89\x96\x00", (uint8_t *)mat_SpiderFeetCollisionTransform_Offset, (uint8_t *)"\x88\x89\x96\x00" },
		//crash { 4, (void *)0x00418672, (uint8_t *)"\x88\x89\x96\x00", (uint8_t *)mat_SpiderFeetCollisionTransform_Offset, (uint8_t *)"\x88\x89\x96\x00" }
	}
};
//
void cheat_handle_SpiderFeet ( struct actor_info *ainfo, double time_diff )
{
	traceLastFunc( "cheat_handle_SpiderFeet()" );

	if ( KEY_PRESSED(set.key_spiderfeet) )
	{
		// toggle the d-dang Ninjas
		if ( !cheat_state->actor.SpiderFeet_on )
		{
			//patcher_install( &patch_actor_SpiderFeetCollisionTransform );
		}
		cheat_state->actor.SpiderFeet_on ^= 1;
	}

	if ( cheat_state->actor.SpiderFeet_on )	//cheat_state->actor.NinjaMode_on)
	{
		// set SpiderFeet status
		cheat_state->actor.SpiderFeet_Enabled = true;

		// set NinjaMode enabler to on
		//ainfo->base.nImmunities = 0x0B;

		// get "down" facing vector
		CVector				offsetVector = cheat_actor_getPositionUnder( ainfo );

		// setup variables
		CVector				vecOrigin, vecTarget;
		CColPoint			*pCollision = NULL;
		CEntitySAInterface	*pCollisionEntity = NULL;
		int					checkDistanceMeters = 20;

		// get CEntitySAInterface pointer
		CEntitySAInterface	*p_CEntitySAInterface = cheat_actor_GetCEntitySAInterface( ainfo );

		// origin = our actor
		vecOrigin = p_CEntitySAInterface->Placeable.matrix->vPos;

		// target = vecOrigin + offsetVector * checkDistanceMeters
		vecTarget = offsetVector * checkDistanceMeters;
		vecTarget = vecTarget + vecOrigin;

		// for time/fps purposes
		float	fTimeStep = *(float *)0xB7CB5C;

		// check for collision
		bool	bCollision = GTAfunc_ProcessLineOfSight( &vecOrigin, &vecTarget, &pCollision, &pCollisionEntity, 1, 0,
														 0, 1, 1, 0, 0, 0 );

		if ( bCollision )
		{
			// set altered gravity vector
			float	fTimeStep = *(float *)0xB7CB5C;
			CVector colGravTemp = -pCollision->GetInterface()->Normal;
			CVector vehGravTemp = cheat_state->actor.gravityVector;
			CVector newRotVector;
			newRotVector = colGravTemp - vehGravTemp;
			newRotVector *= 0.05f * fTimeStep;
			offsetVector = vehGravTemp + newRotVector;

			// for collision on ground
			CMatrix colTransformer;
			//CVector colPosOriginal;
			mat_SpiderFeetCollisionTransform_Original->ConvertToMatrix( colTransformer );
			//colPosOriginal = colTransformer.vPos;
			CVector rotationAxis = colTransformer.vUp;
			rotationAxis.CrossProduct( &-colGravTemp );
			float theta = colTransformer.vUp.DotProduct( &-colGravTemp );
// add check here for theta nearzero
			colTransformer = colTransformer.Rotate( &rotationAxis, -cos(theta) );
			//colTransformer.vPos = colPosOriginal;
			mat_SpiderFeetCollisionTransform->SetFromMatrix( colTransformer );

			pCollision->Destroy();
		}
		else
		{
			// set normal gravity vector
			CVector colGravTemp;
			colGravTemp.fX = 0.0f;
			colGravTemp.fY = 0.0f;
			colGravTemp.fZ = -1.0f;

			CVector vehGravTemp = cheat_state->actor.gravityVector;
			CVector newRotVector;
			newRotVector = colGravTemp - vehGravTemp;
			newRotVector *= 0.05f * fTimeStep;
			offsetVector = vehGravTemp + newRotVector;
		}

		// set the gravity/camera
		cheat_actor_setGravity( ainfo, offsetVector );
		//pPed->SetOrientation( offsetVector.fX, offsetVector.fY, offsetVector.fZ );
		traceLastFunc( "cheat_handle_SpiderFeet()" );

		// set up vector, can make it very easy to scale walls n such
		//pPed->SetWas( &-offsetVector );

		// Ninjas know how to do awesome flips n shit
		if ( KEY_DOWN(set.key_ninjaflipfront) )
		{
			//Log("1");
			//CVector vecVelocity( pPedSelfSA->vecVelocity->fX, pPedSelfSA->vecVelocity->fY, pPedSelfSA->vecVelocity->fZ );
			//Log("2");

			//if ( vecVelocity.IsNearZero() )
			//{
			//	//ds
			//}
			//else
			//{
			//	// prepare directional vector

			//}
				//vecVelocity.Normalize();
				//vecVelocity *= 100.0f;
				//vecVelocity.ZeroNearZero();

				if ( !isBadPtr_writeAny(pPedSelfSA, sizeof(CPedSAInterface)) )
				{
					Log("good pPedSelfSA");
					if ( !isBadPtr_writeAny(pPedSelfSA->vecSpinCollision, sizeof(CVector)) )
					{
						Log("good pPedSelfSA->vecSpinCollision");
						//if ( !isBadPtr_writeAny(&pPedSelfSA->vecSpinCollision->fX, sizeof(float)) )
						//{
							//ds
						//}
						//else
						//	Log("bad pPedSelfSA->vecSpinCollision->fX");
					}
					else
						Log("bad pPedSelfSA->vecSpinCollision");
				}
				else
					Log("bad pPedSelfSA");

				//pPedSelfSA->vecSpinCollision->fX = vecVelocity.fX;

				//memcpy_safe( pPedSelfSA->vecSpinCollision, &vecVelocity, sizeof(float[3]) );
*/

			/*
			// get matrix, backup original front vector for comparison
			CMatrix matPed;
			pPed->GetMatrix( &matPed );
			CVector vecSpinOriginal;
			vecSpinOriginal.fX = matPed.vFront.fZ;
			vecSpinOriginal.fY = matPed.vRight.fZ;
			vecSpinOriginal.fZ = matPed.vUp.fZ;

			// rotate matrix on right axis
			float rotation_theta = M_PI / 2.0f;
// add check for theta nearzero
			matPed = matPed.Rotate( &matPed.vRight, rotation_theta );

			// compare
			CVector vecSpinCompare;
			vecSpinCompare.fX = matPed.vFront.fZ;
			vecSpinCompare.fY = matPed.vRight.fZ;
			vecSpinCompare.fZ = matPed.vUp.fZ;
			vecSpinCompare = (vecSpinOriginal - vecSpinCompare) * 10;

			// spin mother fucker, spin
			//pPedSA->vecSpinCollision = &(CVector)( vecSpinCompare );
			pPedSA->vecSpin = &(CVector)( vecSpinCompare );

			//pPed->SetDirection( vecSpinCompare );
			//pPed->SetWas( 
			//CVehicle blah;
			//blah.SetWas( vecSpinCompare );
			*/
/*

		}
		//key_ninjaflipfront
		//key_ninjaflipback
		//key_ninjajumpboost

		// if we're standing, rotate the CPed to match
		// TODO

	}
	else if ( cheat_state->actor.SpiderFeet_Enabled )
	{

		//patcher_remove( &patch_actor_SpiderFeetCollisionTransform );

		// set NinjaMode enabler to off
		//ainfo->base.nImmunities = 0x12;

		CVector offsetVector;

		// disable NinjaMode with normal gravity vector
		offsetVector.fX = 0.0f;
		offsetVector.fY = 0.0f;
		offsetVector.fZ = -1.0f;

		// set the gravity/camera
		cheat_actor_setGravity( ainfo, offsetVector );

		// set NinjaMode disabled
		cheat_state->actor.SpiderFeet_Enabled = false;
	}
}
*/

// used for cheat_handle_actor_fly()
enum playerFly_keyStates
{
	none,
	accelerate,
	decelerate
};
playerFly_keyStates playerFly_lastKeyState = none;
//float playerFly_rotateTiltGoo = 0.0f;

void cheat_handle_actor_fly ( struct actor_info *ainfo, double time_diff )
{
	traceLastFunc( "cheat_handle_actor_fly()" );

	// toggle
	if ( KEY_PRESSED(set.key_fly_player) )
	{
		if ( !cheat_state->actor.fly_on )
		{
			// init stuff
		}
		cheat_state->actor.fly_on ^= 1;
	}

	if ( cheat_state->actor.fly_on )
	{
		// set fly status
		cheat_state->actor.fly_enabled = true;

		// get ground Z height
		float groundZHeight = pGame->GetWorld()->FindGroundZFor3DPosition(pPedSelf->GetPosition());
		float playerZHeight = pPedSelf->GetPosition()->fZ;
		float playerFrontZOffset = abs(pPedSelfSA->Placeable.matrix->vFront.fZ);
		float playerRightZOffset = abs(pPedSelfSA->Placeable.matrix->vRight.fZ);

		// standing detection
		if ( cheat_state->actor.fly_active
				&& ainfo->pedFlags.bIsStanding
			|| cheat_state->actor.fly_active
				&& groundZHeight + 1.4f > playerZHeight
				&& groundZHeight - 1.4f < playerZHeight)
		{
			cheat_state->actor.fly_active = false;
			playerFly_lastKeyState = none;

			// copy camera rotation to player
			ainfo->fCurrentRotation = -pGame->GetCamera()->GetCameraRotation();
			ainfo->fTargetRotation = ainfo->fCurrentRotation;
			// play landing animation
			GTAfunc_PerformAnimation("SHOP", "SHP_Jump_Land ", -1, 0, 1, 0, 0, 0, 0, 0);

			// correct for angle after landing if needed
			if (playerFrontZOffset > 0.4f
				|| playerRightZOffset > 0.3f)
			{
				// get player matrix
				CMatrix matPed;
				pPedSelf->GetMatrix(&matPed);

				// tilt player upright
				CVector rotationAxis = UpNormal;
				rotationAxis.CrossProduct( &matPed.vUp );
				float theta = ( matPed.vUp.DotProduct( &UpNormal ) );
				if ( !near_zero(theta) )
				{
					matPed = matPed.Rotate( &rotationAxis, cos(theta) );
				}
				// normalize everything
				matPed.vFront.Normalize();
				matPed.vRight.Normalize();
				matPed.vUp.Normalize();
				// set player matrix
				pPedSelf->SetMatrix(&matPed);
			}
		}
		else if ( ainfo->pedFlags.bIsStanding
			|| groundZHeight + 1.6f > playerZHeight
				&& groundZHeight - 1.6f < playerZHeight )
		{
			// still standing, do nothing
		}
		else // I believe I can fly...
		{
			// keys/buttons input
			playerFly_keyStates keyState;
			if ( KEY_DOWN(set.key_fly_player_accelerate) )
			{
				keyState = accelerate;
			}
			else if ( KEY_DOWN(set.key_fly_player_decelerate) )
			{
				keyState = decelerate;
			}
			else
			{
				keyState = none;
			}

			// activate fly mode
			if ( !cheat_state->actor.fly_active )
			{
				cheat_state->actor.fly_active = true;
				if ( keyState == none )
				{
					// start fly animation
					GTAfunc_PerformAnimation("SWIM", "Swim_Breast", -1, 1, 1, 0, 0, 0, 1, 0);
				}
			}
			
			// change animation
			if ( playerFly_lastKeyState != keyState )
			{
				playerFly_lastKeyState = keyState;
				switch ( keyState )
				{
				case none:
					{
						GTAfunc_PerformAnimation("SWIM", "Swim_Breast", -1, 1, 1, 0, 0, 0, 1, 0);
						break;
					}
				case accelerate:
					{
						GTAfunc_PerformAnimation("SWIM", "SWIM_crawl", -1, 1, 1, 0, 0, 0, 1, 0);
						break;
					}
				case decelerate:
					{
						GTAfunc_PerformAnimation("SWIM", "Swim_Tread", -1, 1, 1, 0, 0, 0, 1, 0);
						break;
					}
				}
			}

//

			// setup variables used through this function
			CVector vecSpeed;
			pPedSelf->GetMoveSpeed(&vecSpeed);
			float theta;

			// copy camera rotation to player
			ainfo->fCurrentRotation = -pGame->GetCamera()->GetCameraRotation();

			// get camera matrix
			CMatrix matCamera;
			pGame->GetCamera()->GetMatrix(&matCamera);
			matCamera.vRight = -matCamera.vRight; // for some reason this is upside down
			// normalize camera
			matCamera.vFront.Normalize();
			matCamera.vRight.Normalize();
			matCamera.vUp.Normalize();

			// get player matrix
			CMatrix matPed;
			pPedSelf->GetMatrix(&matPed);
			// copy camera matrix to player
			matPed.vFront = matCamera.vFront;
			matPed.vRight = matCamera.vRight;
			matPed.vUp = matCamera.vUp;

			// tilt player based on side speed & rotation goo
			CVector rotationAxis = UpNormal;
			rotationAxis.CrossProduct( &vecSpeed );
			theta = ( matPed.vFront.DotProduct( &vecSpeed ) / vecSpeed.Length() );// + playerFly_rotateTiltGoo;
			if ( !near_zero(theta) )
			{
				matPed = matPed.Rotate( &rotationAxis, cos(-theta) );
			}
			matPed.vFront = matCamera.vFront;
			// normalize everything
			matPed.vFront.Normalize();
			matPed.vRight.Normalize();
			matPed.vUp.Normalize();
			// set player matrix
			pPedSelf->SetMatrix(&matPed);

//

			// acceleration
			pPedSelf->GetMoveSpeed(&vecSpeed);
			float fly_speed = 0.1f;
			float fly_acceleration = 0.3f * time_diff;
			switch ( keyState )
			{
			case accelerate:
				{
					fly_speed = 2.0f;
					fly_acceleration = 0.9f * time_diff;
				}
			case none:
				{
					if ( vecSpeed.Length() < fly_speed )
					{
						vecSpeed += matCamera.vFront * fly_acceleration;
					}

					// don't have NearZero speeds
					if ( !vecSpeed.IsNearZero() )
					{
						// set speed vector
						ainfo->m_SpeedVec = vecSpeed;
					}
				}
				break;
			case decelerate:
				{
					// this bit should be converted to mta-style code
					float speed = vect3_length( ainfo->speed );
					vect3_normalize( ainfo->speed, ainfo->speed );
					if (speed > 1.0f)
					{
						speed -= time_diff * 0.8f / speed;
					}
					else if (speed > 0.5f)
					{
						speed -= time_diff * 0.6f / (speed / 2);
					}
					else
					{
						speed -= time_diff * 0.3f;
					}

					if ( speed < 0.0f )
						speed = 0.0f;

					if ( vect3_near_zero(ainfo->speed) )
					{
						vect3_zero( ainfo->speed );
					}
					else
					{
						vect3_mult( ainfo->speed, speed, ainfo->speed );
					}
				}
				break;
			}

//
/*
pPedSelf->GetMoveSpeed(&vecSpeed);
CVector *position = pPedSelf->GetPosition();
D3DXVECTOR3 ainfoPos(position->fX, position->fY, position->fZ);
D3DXVECTOR3 vecSpeedD3D(vecSpeed.fX, vecSpeed.fY, vecSpeed.fZ);
render->DrawLine( ainfoPos, ainfoPos + (vecSpeedD3D * 10), D3DCOLOR_ARGB(255, 0, 255, 0) );

*/

			// rotate the speed vector slowly to face the player direction
			pPedSelf->GetMoveSpeed(&vecSpeed);
			CMatrix matSpeedVecRotate = CMatrix();
			matSpeedVecRotate.vFront = vecSpeed;
			matSpeedVecRotate.vFront.Normalize();
			// calculate rotation multiplier, time_diff * 69.0 is ideal for calculations, always time for 69
			float rotationMultiplier = (time_diff * 69.0) / ( 32.0f + (vecSpeed.Length() * 8.0f) );
			// calculate rotation
			rotationAxis = matPed.vFront;
			rotationAxis.CrossProduct( &matSpeedVecRotate.vFront );
			float thetaBase = abs(sinh(matPed.vFront.DotProduct(&matSpeedVecRotate.vFront)) - 1.175f) / 2.35f + 1.0f;
			theta = thetaBase * rotationMultiplier;
			if ( !near_zero(theta) )
			{
				// rotate
				matSpeedVecRotate = matSpeedVecRotate.Rotate( &rotationAxis, theta );

				// calculate new speed
				float speedReduction = time_diff * 0.2f * (thetaBase - 1.0f);

				// set new speed vector
				matSpeedVecRotate.vFront.Normalize();
				ainfo->m_SpeedVec = matSpeedVecRotate.vFront * ( ainfo->m_SpeedVec.Length() - speedReduction );
			}
			// calculate new speed
			float speedReduction = time_diff * 0.2f * (thetaBase - 1.0f);

//

			/*
			// I got my googoo all over it.
			// this is to tilt the player based on rotation only
			float rotateDiff = ainfo->fCurrentRotation - -pGame->GetCamera()->GetCameraRotation();
			// more than half turn in one frame won't happen unless
			// it's passing through 360 degrees, so let's correct for it
			// * this code is totally whack right now
			if ( rotateDiff > 180.0f )
			{
				rotateDiff -= 360.0f;
			}
			else if ( rotateDiff < -180.0f )
			{
				rotateDiff += 360.0f;
			}
			playerFly_rotateTiltGoo += rotateDiff / 10.0f;
			// remove some of the googoo.
			if ( playerFly_rotateTiltGoo > 2.0f )
			{
				playerFly_rotateTiltGoo -= 2.0f * time_diff;
			}
			else if ( playerFly_rotateTiltGoo < 2.0f )
			{
				playerFly_rotateTiltGoo += 2.0f * time_diff;
			}
			else
			{
				playerFly_rotateTiltGoo = 0.0f;
			}
			*/

		} // I believe I can touch the sky...
	}
	else if ( cheat_state->actor.fly_enabled )
	{
		// set fly disabled
		cheat_state->actor.fly_enabled = false;
		if (cheat_state->actor.fly_active)
		{
			cheat_state->actor.fly_active = false;
			// copy camera rotation to player
			ainfo->fCurrentRotation = -pGame->GetCamera()->GetCameraRotation();
			ainfo->fTargetRotation = ainfo->fCurrentRotation;
			// stop animation
			GTAfunc_PerformAnimation("SHOP", "SHP_Jump_Land ", -1, 0, 1, 0, 0, 0, 0, 0);
		}
		playerFly_lastKeyState = none;
	}


/*
// drawing some stuff
int lineSpace = 0;
char buf[256];
sprintf( buf, "speed: %0.1f", ainfo->m_SpeedVec.Length() );
pD3DFontFixed->PrintShadow(50, 500 + lineSpace, D3DCOLOR_XRGB(0, 200, 0), buf);
lineSpace += 12;

*/

}

void cheat_handle_actor_nitro ( struct actor_info *info, double time_diff )
{
	return;

	traceLastFunc( "cheat_handle_actor_nitro()" );

	static uint32_t		timer;
	static int			decelerating;
	static float		speed_off;

	if ( KEY_PRESSED(set.key_nitro_mod) )
	{
		speed_off = vect3_length( info->speed );
		decelerating = 0;
		timer = time_get();
	}

	/* "nitro" acceleration mod */
	if ( KEY_DOWN(set.key_nitro_mod) && !vect3_near_zero(info->speed) )
	{
		float	etime = TIME_TO_FLOAT( time_get() - timer ) / set.nitro_accel_time;
		float	speed = set.nitro_high;

		if ( !near_zero(set.nitro_accel_time) )
			etime += 1.0f - ( set.nitro_high - speed_off ) / set.nitro_high;

		if ( etime < 1.0f && !near_zero(set.nitro_accel_time) )
			speed = set.nitro_high * etime;

		if ( !vect3_near_zero(info->speed) )
		{
			// set speed into the direction we are facing
			float	dir[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
			float	vect[4];
			matrix_vect4_mult( info->base.matrix, dir, vect );
			if ( vect3_near_zero(vect) )
				return;
			vect3_normalize( info->speed, info->speed );
			vect3_mult( vect, speed, info->speed );
			if ( vect3_near_zero(info->speed) )
				vect3_zero( info->speed );
		}

		// heh
		/*
		int		gonadsMult = 1000;
		float	strifeMult = 0.0000001f;
		int		gonads = rand() % gonadsMult;
		float	strife = (double)gonads * strifeMult;
		if ( strife < strifeMult * gonadsMult / 2 )
			strife -= strifeMult * gonadsMult;
		info->m_SpeedVec.fX += strife;
		gonads = rand() % gonadsMult;
		strife = (double)gonads * strifeMult;
		if ( strife < strifeMult * gonadsMult / 2 )
			strife -= strifeMult * gonadsMult;
		info->m_SpeedVec.fY += strife;
		*/
	}

	if ( KEY_RELEASED(set.key_nitro_mod) )
	{
		if ( vect3_length(info->speed) > set.nitro_low )
		{
			speed_off = vect3_length( info->speed );
			decelerating = 1;
			timer = time_get();
		}
	}

	if ( decelerating )
	{
		float	speed = set.nitro_low;
		float	etime = TIME_TO_FLOAT( time_get() - timer );

		if ( etime < set.nitro_decel_time )
			speed = speed_off - ( speed_off - speed ) * ( etime / set.nitro_decel_time );
		else
			decelerating = 0;

		if ( vect3_length(info->speed) > speed )
		{
			vect3_normalize( info->speed, info->speed );
			vect3_mult( info->speed, speed, info->speed );
		}
	}
}
