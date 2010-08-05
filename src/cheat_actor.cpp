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
		if ( !isBadPtr_GTA_pVehicleInfo(veh) )
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
		bool isAimKeyDown = false;
		CControllerConfigManager *pPadConfig = pGameInterface->GetControllerConfigManager();
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
		if ( cheat_state->actor.autoaim )//&& isAimKeyDown )
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
				return; /* won't happen */

			/*cheat_state_text("%.3f %.3f %d %d", adj_rx, adj_rz, nearest->state, nearest->state_running);*/
			/* calculate distance vector */
			vect3_vect3_sub( &nearest->base.matrix[4 * 3], &info->base.matrix[4 * 3], vect );

			/* z angle */
			az = atan2f( vect[0], vect[1] );

			/* rotate around z axis */
			vect[1] = sinf( az ) * vect[0] + cosf( az ) * vect[1];

			/* x angle */
			ax = atan2f( vect[1], vect[2] );

			ax = -ax + M_PI / 2.0f + adj_rx;
			az = -az - M_PI / 2.0f + adj_rz;

			if ( ax < -M_PI )
				ax = -M_PI;
			else if ( ax > M_PI )
				ax = M_PI;

			/* XXX make function */
			prev_rx = *(float *)0x00B6F248 = ax;
			prev_rz = *(float *)0x00B6F258 = az;
		}
	}




/*
	// let's aim, shall we?
	if ( cheat_state->actor.autoaim )//&& isAimKeyDown )
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


int lineSpace = 0;
char buf[128];


		// main aim vectors
		CVector vecSelfAimPos, vecAimFromPos, vecAimToPos;

		// reused rotation variables
		float theta;
		CVector vecRightBlank = CVector(1.0f, 1.0f, 0.0f);
		CVector vecUpBlank = CVector(1.0f, 0.0f, 1.0f);

		// get our position
		pPedSelf->GetBonePosition(BONE_RIGHTHAND, &vecSelfAimPos);
		CVector vecSelfPos = pPedSelf->GetInterface()->Placeable.matrix->vPos;




		// get the weapon's cam object/s
		//float rx = *(float *)0x00B6F248; // CCamSAInterface::m_fTargetBeta
		//float rz = *(float *)0x00B6F258; // CCamSAInterface::m_fTrueAlpha
		CCamSA *pSelfWeaponCam1 = new CCamSA((CCamSAInterface *)0x00B6F19C);
		//CCamSA *pSelfWeaponCam2 = new CCamSA((CCamSAInterface *)0x00B6F3D4);
		//CCamSA *pSelfWeaponCam3 = new CCamSA((CCamSAInterface *)0x00B6F60C);

		// weapon cam vector
		CVector *pSelfWeapCamFront = pSelfWeaponCam1->GetFront();
		CVector SelfWeapCamFront;
		SelfWeapCamFront.fX = pSelfWeapCamFront->fX;
		SelfWeapCamFront.fY = pSelfWeapCamFront->fY;
		SelfWeapCamFront.fZ = pSelfWeapCamFront->fZ;
*/
/*
		SelfWeapCamFront.fX = 1.0f;
		SelfWeapCamFront.fY = 0.0f;
		SelfWeapCamFront.fZ = pSelfWeaponCam1->GetInterface()->m_fTrueAlpha;
		// rotate vec toward aim
		CMatrix weapAimRotate;
		theta = M_PI / ( pSelfWeaponCam1->GetInterface()->m_fTrueBeta / 2 );
		weapAimRotate.vFront = SelfWeapCamFront;
		weapAimRotate = weapAimRotate.Rotate( &vecUpBlank, theta );

		SelfWeapCamFront = weapAimRotate.vFront;


sprintf( buf, "m_fTrueAlpha: %.4f", pSelfWeaponCam1->GetInterface()->m_fTrueAlpha );
pD3DFontFixed->PrintShadow(450, 50 + lineSpace, D3DCOLOR_XRGB(0, 200, 0), buf);
lineSpace += 12;
sprintf( buf, "m_fTrueBeta: %.4f", pSelfWeaponCam1->GetInterface()->m_fTrueBeta );
pD3DFontFixed->PrintShadow(450, 50 + lineSpace, D3DCOLOR_XRGB(0, 200, 0), buf);
lineSpace += 12;
*/
/*
		CVector ffFront = SelfWeapCamFront;

		// weapon fire offset, switch of X and Y so it points north
		CWeaponInfo *pPedSelfWeapInfo = pPedSelf->GetWeapon(pPedSelf->GetCurrentWeaponSlot())->GetInfo();
		CVector *pSelfWeapFireOffset = pPedSelfWeapInfo->GetFireOffset();
		CVector SelfWeapFireOffset;
		SelfWeapFireOffset.fX = pSelfWeapFireOffset->fX;
		SelfWeapFireOffset.fY = pSelfWeapFireOffset->fY;
		SelfWeapFireOffset.fZ = pSelfWeapFireOffset->fZ;

		// save this so we can restore proper length after offset rotation
		float weapOffsetLength = SelfWeapFireOffset.Length();

		// weapon offset rotation variables
		CMatrix weapOffsetRotate;

		// rotate offset toward north
		theta = M_PI / 4.0f;
		weapOffsetRotate.vFront = SelfWeapFireOffset;
		weapOffsetRotate = weapOffsetRotate.Rotate( &vecUpBlank, -theta );

		// normalize some values for offset rotation
		SelfWeapCamFront.Normalize();
		SelfWeapFireOffset.Normalize();

		// rotate fire offset to aim
		theta = acos( SelfWeapFireOffset.DotProduct(&SelfWeapCamFront) );
		if ( theta > FLOAT_EPSILON )
		{
			SelfWeapFireOffset.CrossProduct( &SelfWeapCamFront );
			SelfWeapFireOffset.Normalize();
			SelfWeapFireOffset.ZeroNearZero();
			weapOffsetRotate = weapOffsetRotate.Rotate( &SelfWeapFireOffset, -theta );
		}
		SelfWeapFireOffset = weapOffsetRotate.vFront * weapOffsetLength;


sprintf( buf, "weapOffsetLength: %.4f", weapOffsetLength );
pD3DFontFixed->PrintShadow(450, 50 + lineSpace, D3DCOLOR_XRGB(0, 200, 0), buf);
lineSpace += 12;
sprintf( buf, "theta: %.4f", theta );
pD3DFontFixed->PrintShadow(450, 50 + lineSpace, D3DCOLOR_XRGB(0, 200, 0), buf);
lineSpace += 12;


		//CMatrix *pSelfWeapMatrix1 = (CMatrix *)0xB6FE40;
		//CVector *pSelfWeapVec1 = (CVector *)0xB6FDA4;
		//CVector *pSelfWeapVec2 = (CVector *)0xB6FDB8;
		//CVector SelfWeapBoneOffset = pPedSelf->GetPedInterface()->pPlayerData->m_vecTargetBoneOffset;







		// set the final aim variables
		vecAimFromPos = vecSelfAimPos + SelfWeapFireOffset;
		//vecAimToPos = vecSelfPos + (ffFront * 2);
		vecAimToPos = vecAimFromPos + (ffFront * 10);
		render->DrawLine(CVecToD3DXVEC(vecAimFromPos), CVecToD3DXVEC(vecAimToPos), D3DCOLOR_ARGB(128, 0, 255, 0));





		// potentially great stuff tho
		//pPedSelf->GetPedInterface()->pPlayerData->m_vecTargetBoneOffset;

		//vecWeaponOffset = pPedSelf->GetWeapon(selfSlot)->GetInfo()->GetFireOffset();

		//vecWeaponOffset = pPedSelf->GetPedInterface()->vecUnk5;
		//vecWeaponOffset = pPedSelf->GetPedInterface()->vecUnk6;
		//vecWeaponPos.fX = vecWeaponOffset->fX;
		//vecWeaponPos.fY = vecWeaponOffset->fY;
		//vecWeaponPos.fZ = vecWeaponOffset->fZ;

		//vecWeaponPos = 








		// setup iterator
		CPedSA		*iterPed = NULL;
		CVector		iterPedPos;


		CPoolsSA	*pPools = reinterpret_cast < CPoolsSA * > ( pGameInterface->GetPools() );
		CPoolsSA::pedPool_t::mapType::iterator iter = pPools->m_pedPool.map.begin();

		// get variables for peds streamed in
		while ( iter.pos < iter.end )
		{
			// map iterator pointer to our pointer
			iterPed = iter.pos->second;

			// advance to next item for next pass
			iter.pos++;
			if ( !iterPed )
				continue;

			// get CPed position
			//iterPedPos = iterPed->GetInterface()->Placeable.matrix->vPos;
			iterPed->GetBonePosition(BONE_SPINE1, &iterPedPos);
			//iterPed->GetPedInterface()->pPlayerData->m_fLookPitch;
			//iterPed->GetPedInterface()->vecUnk5;
			//iterPed->GetPedInterface()->vecUnk6;

			render->DrawLine(CVecToD3DXVEC(vecAimFromPos), CVecToD3DXVEC(iterPedPos), D3DCOLOR_ARGB(128, 255, 0, 0));

			//render->DrawLine(vecGravColOrigin, vecGravTargetNorm, D3DCOLOR_ARGB(128, 0, 255, 0));


			//pPedSelf->GetWeapon(WEAPONSLOT_TYPE_RIFLE)->GetInfo()->GetFireOffset()

		}

	}
*/

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

#ifdef M0D_DEV
D3DXVECTOR3 vecGravColOrigin, vecGravColTarget, vecGravTargetNorm;
CVector		temp_vecGravTargetNorm;
#endif
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

	ainfo->base.m_CMatrix->vFront.fZ = pvecGravity.fX;
	ainfo->base.m_CMatrix->vRight.fZ = pvecGravity.fY;

	ainfo->base.m_CMatrix->vUp.fZ = -pvecGravity.fZ;

	// surf on yourself, lawlz
	//ainfo->vehicle_contact = (vehicle_info*)ainfo;
	//ainfo->pedFlags.bResetWalkAnims = false;
	//ainfo->pedFlags.bStillOnValidPoly = false;
	//ainfo->pedFlags.bUpdateMatricesRequired = true;
	//ainfo->pedFlags.bUpdateAnimHeading = true;
	//ainfo->pedFlags.bWasStanding = false;
	//ainfo->pedFlags.bDonePositionOutOfCollision = true;
	//ainfo->pedFlags.bIsNearCar = true;
	//ainfo->base.m_CMatrix->vUp = -pvecGravity;
	//if (ainfo->base.m_CMatrixPre != NULL)
	//	ainfo->base.m_CMatrixPre->vUp = pvecGravity;
}

void cheat_handle_spiderFeet ( struct actor_info *ainfo, float time_diff )
{
	traceLastFunc( "cheat_handle_spiderFeet()" );

	if ( KEY_PRESSED(set.key_spiderfeet) )
	{
		// toggle the d-dang spiderz
		cheat_state->actor.spiderFeet_on ^= 1;
	}

	if ( 1 == 1 )	//cheat_state->actor.spiderFeet_on)
	{
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
#ifdef M0D_DEV
			/**/
			vecGravColOrigin = D3DXVECTOR3( vecOrigin.fX, vecOrigin.fY, vecOrigin.fZ );
			vecGravColTarget.x = pCollision->GetInterface()->Position.fX;
			vecGravColTarget.y = pCollision->GetInterface()->Position.fY;
			vecGravColTarget.z = pCollision->GetInterface()->Position.fZ;
			temp_vecGravTargetNorm = vecOrigin + offsetVector * checkDistanceMeters;
			vecGravTargetNorm.x = temp_vecGravTargetNorm.fX;
			vecGravTargetNorm.y = temp_vecGravTargetNorm.fY;
			vecGravTargetNorm.z = temp_vecGravTargetNorm.fZ;
#endif
			pCollision->Destroy();
		}
		else
		{
			// set normal gravity vector
			float	fTimeStep = *(float *)0xB7CB5C;
			CVector colGravTemp;
			colGravTemp.fX = 0.0f;
			colGravTemp.fY = 0.0f;
			colGravTemp.fZ = -1.0f;

			CVector vehGravTemp = cheat_state->actor.gravityVector;
			CVector newRotVector;
			newRotVector = colGravTemp - vehGravTemp;
			newRotVector *= 0.05f * fTimeStep;
			offsetVector = vehGravTemp + newRotVector;
#ifdef M0D_DEV
			/**/
			vecGravColOrigin = D3DXVECTOR3( vecOrigin.fX, vecOrigin.fY, vecOrigin.fZ );
			vecGravColTarget = D3DXVECTOR3( vecTarget.fX, vecTarget.fY, vecTarget.fZ );
			temp_vecGravTargetNorm = vecOrigin + offsetVector * checkDistanceMeters;
			vecGravTargetNorm.x = temp_vecGravTargetNorm.fX;
			vecGravTargetNorm.y = temp_vecGravTargetNorm.fY;
			vecGravTargetNorm.z = temp_vecGravTargetNorm.fZ;
#endif
		}

		// set the gravity/camera
		cheat_actor_setGravity( ainfo, offsetVector );
	}
	else if ( cheat_state->actor.spiderFeet_Enabled )
	{
		CVector offsetVector;

		// disable spider wheels with normal gravity vector
		offsetVector.fX = 0.0f;
		offsetVector.fY = 0.0f;
		offsetVector.fZ = -1.0f;

		// set the gravity/camera
		cheat_actor_setGravity( ainfo, offsetVector );

		// set spider wheels disabled
		cheat_state->actor.spiderFeet_Enabled = false;
	}
}
