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
	vect3_copy( pos, &info->base.matrix[4 * 3] );
	gta_interior_id_set( interior_id );
}

void cheat_handle_actor_nocols ( struct actor_info *info )
{
	traceLastFunc( "cheat_handle_actor_nocols()" );

	if ( !cheat_state->_generic.nocols_enabled && !cheat_state->_generic.nocols_toggled )
		return;

	if ( cheat_state->_generic.nocols_toggled && (GetTickCount() - 2000) > cheat_state->_generic.nocols_change_tick )
	{
		cheat_state->_generic.nocols_enabled = 1;
	}

	if ( !cheat_state->_generic.nocols_toggled )
	{
		cheat_state->_generic.nocols_enabled = 0;
	}

	//need to force the player into car while nocols activated (safety checks by gta)
	uint8_t *enter_exit_key = ( uint8_t * ) ( GTA_KEYS + 0x1E );
	if ( cheat_state->_generic.nocols_enabled && *enter_exit_key == 0xff )
	{
		int					id = vehicle_find_nearest( VEHICLE_ALIVE | VEHICLE_EMPTY );
		struct vehicle_info *near_veh = vehicle_info_get( id, VEHICLE_ALIVE | VEHICLE_EMPTY );
		if ( near_veh != NULL
		 &&	 gta_vehicle_get_by_id(near_veh->base.model_alt_id)->class_id != VEHICLE_CLASS_TRAILER
		 &&	 near_veh->hitpoints > 250.0f
		 &&	 vect3_dist(&info->base.matrix[4 * 3], &near_veh->base.matrix[4 * 3]) <= 20.0f )
		{
			ScriptCommand( &enter_car_as_driver, ScriptActorId(info), ScriptCarId(near_veh), 1999 );	//actor,car,maximum time (ms)
		}
	}

	if ( cheat_state->_generic.nocols_enabled && (DWORD) info->animation->active_animation_task != NULL )
	{
		//check if task enter car as * is currently executed (task_info + 0xC = vehicle pointer)
		struct vehicle_info *veh;
		uint32_t			vehiclePointer = ( (uint32_t) info->animation->active_animation_task + 0xC );
		veh = ( vehicle_info * ) ( *(uint32_t *) (vehiclePointer) );
		if ( !isBadPtr_GTA_pVehicle(veh) )
		{
			cheat_state->_generic.nocols_enabled = 0;
			cheat_state->_generic.nocols_change_tick = GetTickCount();
			return;
		}
	}
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

// used for cheat_handle_actor_fly()
enum playerFly_keyStates
{
	none,
	accelerate,
	decelerate
};
playerFly_keyStates playerFly_lastKeyState = none;
float playerFly_rotateTiltGoo = 0.0f;

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

		// standing detection
		if ( ainfo->pedFlags.bIsStanding
			&& cheat_state->actor.fly_active )
		{
			cheat_state->actor.fly_active = false;
			// copy camera rotation to player
			//ainfo->fCurrentRotation = -pGame->GetCamera()->GetCameraRotation();
			// stop fly animation
			playerFly_lastKeyState = none;
			Animation_Releaser("SWIM");
			ScriptCommand( &disembark_instantly_actor, ScriptActorId(ainfo) );
		}
		else if ( ainfo->pedFlags.bIsStanding )
		{
			// do nothing
		}
		// I believe I can fly...
		else
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
					Animation_Loader("SWIM");
					Animation_Perform("SWIM", "Swim_Breast", 1.0f, 1, 1, -2);
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
						//Animation_Loader("SWIM");
						Animation_Perform("SWIM", "Swim_Breast", 1.0f, 1, 1, -2);
					}
				case accelerate:
					{
						//Animation_Loader("SWIM");
						Animation_Perform("SWIM", "SWIM_crawl", 1.0f, 1, 1, -2);
					}
				case decelerate:
					{
						//Animation_Loader("SWIM");
						Animation_Perform("SWIM", "Swim_Tread", 1.0f, 1, 1, -2);
					}
				}
			}




			// get player speed
			CVector vecSpeed;
			pPedSelf->GetMoveSpeed(&vecSpeed);

			// I got my googoo all over it.
			// this is to tilt the player based on rotation only
			float rotateDiff = ainfo->fCurrentRotation - -pGame->GetCamera()->GetCameraRotation();
			// more than half turn in one frame won't happen unless
			// it's passing through 360 degrees, so let's correct for it
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
			float theta = ( matPed.vFront.DotProduct( &vecSpeed ) / vecSpeed.Length() );// + playerFly_rotateTiltGoo;
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





			// rotate the speed vector slowly to face the player direction
			CMatrix matSpeedVecRotate = CMatrix();
			matSpeedVecRotate.vFront = vecSpeed;
			matSpeedVecRotate.vFront.Normalize();
			// get rotation divisor
			float rotationDivisor = 1.0f + vecSpeed.Length() * time_diff * 5000.0f;
			// rotate it
			rotationAxis = matPed.vFront;
			rotationAxis.CrossProduct( &vecSpeed );
			theta = matPed.vFront.DotProduct( &vecSpeed ) / rotationDivisor;
			if ( !near_zero(theta) )
			{
				matSpeedVecRotate = matSpeedVecRotate.Rotate( &rotationAxis, sin(theta) );
			}
			// set new speed vector
			matSpeedVecRotate.vFront.Normalize();
			ainfo->m_SpeedVec = matSpeedVecRotate.vFront * vecSpeed.Length();





			// speed operations
			CVector vecPedSpeed = ainfo->m_SpeedVec;

			// acceleration
			if ( keyState == none
				|| keyState == accelerate )
			{
				float fly_speed = 0.1f;
				float fly_acceleration = 0.3f * time_diff;

				if ( keyState == accelerate )
				{
					fly_speed = 2.0f;
					fly_acceleration = 1.0f * time_diff;
				}

				// positive
				if ( matPed.vFront.fX >= 0.0f
					&& vecPedSpeed.fX < matPed.vFront.fX * fly_speed )
				{
					vecPedSpeed.fX += fly_acceleration;
				}
				if ( matPed.vFront.fY >= 0.0f
					&& vecPedSpeed.fY < matPed.vFront.fY * fly_speed )
				{
					vecPedSpeed.fY += fly_acceleration;
				}
				if ( matPed.vFront.fZ >= 0.0f
					&& vecPedSpeed.fZ < matPed.vFront.fZ * fly_speed * 2 )
				{
					// more upward acceleration boost & speed
					vecPedSpeed.fZ += fly_acceleration * 2;
				}
				// negative
				if ( matPed.vFront.fX < 0.0f
					&& vecPedSpeed.fX > matPed.vFront.fX * fly_speed )
				{
					vecPedSpeed.fX -= fly_acceleration;
				}
				if ( matPed.vFront.fY < 0.0f
					&& vecPedSpeed.fY > matPed.vFront.fY * fly_speed )
				{
					vecPedSpeed.fY -= fly_acceleration;
				}
				if ( matPed.vFront.fZ < 0.0f
					&& vecPedSpeed.fZ > matPed.vFront.fZ * fly_speed )
				{
					vecPedSpeed.fZ -= fly_acceleration;
				}
				// don't have NearZero speeds
				if ( !vecPedSpeed.IsNearZero() )
				{
					// set speed vector
					ainfo->m_SpeedVec = vecPedSpeed;
				}
			}
			else if ( keyState == decelerate )
			{
				float speed = vect3_length( ainfo->speed );
				vect3_normalize( ainfo->speed, ainfo->speed );
				speed -= time_diff * 1.3f;

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




/*

int lineSpace = 0;
char buf[256];
sprintf( buf, "keyState: %d", keyState );
pD3DFontFixed->PrintShadow(50, 650 + lineSpace, D3DCOLOR_XRGB(0, 200, 0), buf);
lineSpace += 12;
*/



		}
	}
	else if ( cheat_state->actor.fly_enabled )
	{
		// set fly disabled
		cheat_state->actor.fly_enabled = false;
		cheat_state->actor.fly_active = false;
		// stop animation
		playerFly_lastKeyState = none;
		Animation_Releaser("SWIM");
		ScriptCommand( &disembark_instantly_actor, ScriptActorId(ainfo) );
		// copy camera rotation to player
		ainfo->fCurrentRotation = -pGame->GetCamera()->GetCameraRotation();
	}
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
