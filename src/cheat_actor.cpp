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
CEntitySAInterface* cheat_actor_GetCEntitySAInterface(actor_info *ainfo)
{
	return (CEntitySAInterface*)ainfo;
}
//*p_CEntitySAInterface = (CEntitySAInterface*)ainfo->base;



void cheat_actor_teleport(struct actor_info *info, const float pos[3], int interior_id)
{
   if(info == NULL)
      return;
   vect3_copy(pos, &info->base.matrix[4*3]);
   gta_interior_id_set(interior_id);
}

static struct patch_set patch_gta_auto_aim = {
	  "GTA: Autoaim", 0, 0, {
      { 1,  (void *)0x00B6EC2E, (uint8_t *)"\x01", (uint8_t *)"\x00", NULL },
      { 1,  (void *)0x00BA6818, (uint8_t *)"\x00", (uint8_t *)"\x01", NULL },
      { 5,  (void *)0x00524013, (uint8_t *)"\xA0\x2E\xEC\xB6\x00", NULL, NULL },
      { 5,  (void *)0x00523F3E, (uint8_t *)"\xA0\x2E\xEC\xB6\x00", NULL, NULL },
      { 5,  (void *)0x00525615, (uint8_t *)"\xA0\x2E\xEC\xB6\x00", NULL, NULL },
      { 5,  (void *)0x005221FC, (uint8_t *)"\xA0\x2E\xEC\xB6\x00", NULL, NULL }
	  }
};

void cheat_handle_actor_autoaim(struct actor_info *info, float time_diff)
{
   if(KEY_PRESSED(set.key_autoaim))
   {
	   if(set.use_gta_autoaim)
	   {
		   if(cheat_state->actor.autoaim == 0)
		   {
			   patcher_install(&patch_gta_auto_aim);
			   cheat_state->actor.autoaim ^= 1;
		   }
		   else
		   {
			   patcher_remove(&patch_gta_auto_aim);
			   cheat_state->actor.autoaim ^= 1;
		   }
	   }
	   else
	   {
		   cheat_state->actor.autoaim ^= 1;

		   static int prev_id;
		   prev_id = -1;

		   if(cheat_state->actor.autoaim)
		   {
			   static float adj_rx, adj_rz, prev_rx, prev_rz;
			   float rx = *(float *)0x00B6F248;
			   float rz = *(float *)0x00B6F258;
			   
			   int nearest_id = actor_find_nearest(ACTOR_ALIVE);
			   struct actor_info *nearest;
			   float vect[3], ax, az;
			   
			   if(nearest_id == -1)
			   {
				   cheat_state_text("No players found; auto aim disabled.");
				   cheat_state->actor.autoaim = 0;
				   return;
			   }
			   
			   if(nearest_id == prev_id)
			   {
				   adj_rx += rx - prev_rx;
				   adj_rz += rz - prev_rz;
			   }
			   prev_id = nearest_id;
			   
			   if((nearest = actor_info_get(nearest_id, ACTOR_ALIVE)) == NULL)
				   return;  /* won't happen */
			   
			   /*cheat_state_text("%.3f %.3f %d %d", adj_rx, adj_rz, nearest->state, nearest->state_running);*/
			   
			   /* calculate distance vector */
			   vect3_vect3_sub(&nearest->base.matrix[4*3], &info->base.matrix[4*3], vect);
			   
			   /* z angle */
			   az = atan2f(vect[0], vect[1]);
			   
			   /* rotate around z axis */
			   vect[1] = sinf(az) * vect[0] + cosf(az) * vect[1];
			   
			   /* x angle */
			   ax = atan2f(vect[1], vect[2]);
			   
			   ax = -ax + M_PI / 2.0f + adj_rx;
			   az = -az - M_PI / 2.0f + adj_rz;
			   
			   if(ax < -M_PI)
				   ax = -M_PI;
			   else if(ax > M_PI)
				   ax = M_PI;
			   
			   /* XXX make function */
			   prev_rx = *(float *)0x00B6F248 = ax;
			   prev_rz = *(float *)0x00B6F258 = az;
		   }
	   }
   }

}

void cheat_handle_actor_air_brake(struct actor_info *info, float time_diff)
{
   static float orig_pos[3];
   static float fall_speed_mult;
   static int was_enabled;

   if(set.air_brake_toggle)
   {
	   if(KEY_PRESSED(set.key_air_brake_foot_mod))
		   cheat_state->actor.air_brake ^= 1;

      if(KEY_PRESSED(set.key_air_brake_mod2) && cheat_state->actor.air_brake)
         cheat_state->actor.air_brake_slowmo ^= 1;
   }
   else
   {
      if(KEY_PRESSED(set.key_air_brake_foot_mod))
         cheat_state->actor.air_brake = 1;
      else if(KEY_RELEASED(set.key_air_brake_foot_mod))
         cheat_state->actor.air_brake = 0;

      if(KEY_PRESSED(set.key_air_brake_mod2) && cheat_state->actor.air_brake)
         cheat_state->actor.air_brake_slowmo = 1;
      else if(KEY_RELEASED(set.key_air_brake_mod2) && cheat_state->actor.air_brake)
         cheat_state->actor.air_brake_slowmo = 0;
   }

   if(!was_enabled && cheat_state->actor.air_brake)
   {
      vect3_copy(&info->base.matrix[4*3], orig_pos);
      fall_speed_mult = 1.0f;
      was_enabled = 1;
   }

   if(!cheat_state->actor.air_brake)
      was_enabled = 0;

   if(cheat_state->actor.air_brake)
   {
	    #define PARACHUTE_ACTIVE (info->weapon_slot == 11 && info->weapon[11].id == 46)

      /* prevent movement */
      if(!PARACHUTE_ACTIVE)
      {
         vect3_copy(orig_pos, &info->base.matrix[4*3]);
         vect3_zero(info->speed);

		 // new pedFlags
		 info->pedFlags.bIsStanding = true;
		 info->pedFlags.bWasStanding = true;
		 info->pedFlags.bStayInSamePlace = true;
      }
   }
   else
   {
      cheat_state->actor.air_brake = 0;
      cheat_state->actor.air_brake_slowmo = 0;
   }

   if(cheat_state->actor.air_brake)
   {
      float *matrix = info->base.matrix;

      if(!PARACHUTE_ACTIVE)
      {
         static uint32_t time_start;
         float d[4] = { 0.0f, 0.0f, 0.0f, time_diff * set.air_brake_speed };

         if(cheat_state->actor.air_brake_slowmo)
            d[3] /= 10.0f;

         if(KEY_DOWN(set.key_air_brake_forward))   d[0] += 1.0f;
         if(KEY_DOWN(set.key_air_brake_backward))  d[0] -= 1.0f;
         if(KEY_DOWN(set.key_air_brake_left))      d[1] += 1.0f;
         if(KEY_DOWN(set.key_air_brake_right))     d[1] -= 1.0f;
         if(KEY_DOWN(set.key_air_brake_up))        d[2] += 1.0f;
         if(KEY_DOWN(set.key_air_brake_down))      d[2] -= 1.0f;

         if(!near_zero(set.air_brake_accel_time))
         {
            if(!vect3_near_zero(d))
               time_start = (time_start == 0) ? time_get() : time_start;
            else
               time_start = 0;   /* no keys pressed */

            /* acceleration */
            if(time_start != 0)
            {
               float t = TIME_TO_FLOAT(time_get() - time_start);
               if(t < set.air_brake_accel_time)
                  d[3] *= t / set.air_brake_accel_time;
            }
         }

         if(!vect3_near_zero(d))
         {
            float vect[4] = { -d[1], d[0], d[2], 0.0f };
            float out[4];

            /* out = matrix * norm(d) */
            vect3_normalize(vect, vect);
            matrix_vect4_mult(matrix, vect, out);

            matrix[4*3+0] += out[0] * d[3];
            matrix[4*3+1] += out[1] * d[3];
            matrix[4*3+2] += out[2] * d[3];
         }
      }
      else /* PARACHUTE_ACTIVE */
      {
         if(KEY_DOWN(set.key_air_brake_up))   fall_speed_mult += time_diff / 2.0f;
         if(KEY_DOWN(set.key_air_brake_down)) fall_speed_mult -= time_diff / 2.0f;

         if(fall_speed_mult < 0.0f)
            fall_speed_mult = 0.0f;
         else if(fall_speed_mult > 2.0f)
            fall_speed_mult = 2.0f;

         matrix[4*3+0] += info->speed[0] * time_diff * set.air_brake_speed;
         matrix[4*3+1] += info->speed[1] * time_diff * set.air_brake_speed;
         matrix[4*3+2] -= (matrix[4*3+2] - orig_pos[2]) * fall_speed_mult;
      }

      vect3_copy(&matrix[4*3], orig_pos);
   }
}






































#ifdef M0D_DEV
D3DXVECTOR3 vecGravColOrigin, vecGravColTarget, vecGravTargetNorm;
CVector temp_vecGravTargetNorm;
#endif

CVector cheat_actor_getPositionUnder(actor_info *ainfo)
{
	traceLastFunc("cheat_vehicle_getPositionUnder()");
	CVector offsetVector;
	float *matrix = ainfo->base.matrix;
	offsetVector.fX = 0 * matrix[0] + 0 * matrix[4] - 1 * matrix[8];
    offsetVector.fY = 0 * matrix[1] + 0 * matrix[5] - 1 * matrix[9];
    offsetVector.fZ = 0 * matrix[2] + 0 * matrix[6] - 1 * matrix[10];
	return offsetVector;
}

void cheat_actor_setGravity(actor_info *ainfo, CVector pvecGravity)
{
	traceLastFunc("cheat_actor_setGravity()");

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


void cheat_handle_spiderFeet(struct actor_info *ainfo, float time_diff)
{
	traceLastFunc("cheat_handle_spiderFeet()");

	if(KEY_PRESSED(set.key_spiderfeet))
	{
		// toggle the d-dang spiderz
		cheat_state->actor.spiderFeet_on ^= 1;
	}

	if (1==1)//cheat_state->actor.spiderFeet_on)
	{
		// get "down" facing vector
		CVector offsetVector = cheat_actor_getPositionUnder(ainfo);

		// setup variables
		CVector vecOrigin, vecTarget;
		CColPoint *pCollision = NULL;
		CEntitySAInterface *pCollisionEntity = NULL;
		int checkDistanceMeters = 20;

		// get CEntitySAInterface pointer
		CEntitySAInterface *p_CEntitySAInterface = cheat_actor_GetCEntitySAInterface(ainfo);

		// origin = our actor
		vecOrigin = p_CEntitySAInterface->Placeable.matrix->vPos;
		// target = vecOrigin + offsetVector * checkDistanceMeters
		vecTarget = offsetVector * checkDistanceMeters;
		vecTarget = vecTarget + vecOrigin;

		// check for collision
		bool bCollision = GTAfunc_ProcessLineOfSight(&vecOrigin, &vecTarget, &pCollision, &pCollisionEntity, 1, 0, 0, 1, 1, 0, 0, 0);

		if (bCollision)
		{
			// set altered gravity vector
			float fTimeStep = *(float *)0xB7CB5C;
			CVector colGravTemp = -pCollision->GetInterface()->Normal;
			CVector vehGravTemp = cheat_state->actor.gravityVector;
			CVector newRotVector;
			newRotVector = colGravTemp - vehGravTemp;
			newRotVector *= 0.05f * fTimeStep;
			offsetVector = vehGravTemp + newRotVector;
#ifdef M0D_DEV
/**/
			vecGravColOrigin = D3DXVECTOR3(vecOrigin.fX, vecOrigin.fY, vecOrigin.fZ);
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
			float fTimeStep = *(float *)0xB7CB5C;
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
			vecGravColOrigin = D3DXVECTOR3(vecOrigin.fX, vecOrigin.fY, vecOrigin.fZ);
			vecGravColTarget = D3DXVECTOR3(vecTarget.fX, vecTarget.fY, vecTarget.fZ);
			temp_vecGravTargetNorm = vecOrigin + offsetVector * checkDistanceMeters;
			vecGravTargetNorm.x = temp_vecGravTargetNorm.fX;
			vecGravTargetNorm.y = temp_vecGravTargetNorm.fY;
			vecGravTargetNorm.z = temp_vecGravTargetNorm.fZ;

#endif
		}

		// set the gravity/camera
		cheat_actor_setGravity(ainfo, offsetVector);
	}
	else if (cheat_state->actor.spiderFeet_Enabled)
	{
		CVector offsetVector;
		// disable spider wheels with normal gravity vector
		offsetVector.fX = 0.0f;
		offsetVector.fY = 0.0f;
		offsetVector.fZ = -1.0f;

		// set the gravity/camera
		cheat_actor_setGravity(ainfo, offsetVector);
		// set spider wheels disabled
		cheat_state->actor.spiderFeet_Enabled = false;
	}
}
