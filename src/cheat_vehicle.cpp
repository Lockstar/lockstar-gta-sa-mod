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

	$LastChangedDate: 2009-12-14 01:12:56 -0600 (Mon, 14 Dec 2009) $
	$LastChangedBy: futnucks $
	$Revision: 31 $
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/cheat_vehicle.cpp $
	$Id: cheat_vehicle.cpp 31 2009-12-14 07:12:56Z futnucks $

*/

#include "main.h"

void cheat_vehicle_teleport(struct vehicle_info *info, const float pos[3], int interior_id)
{
   if(info == NULL)
      return;

   float diff[3];
   float new_pos[3];
   struct vehicle_info *temp;

   vect3_vect3_sub(pos, &info->base.matrix[4*3], diff);

   for(temp = info; temp != NULL; temp = temp->trailer)
   {
      vect3_vect3_add(&temp->base.matrix[4*3], diff, new_pos);

      vehicle_detachables_teleport(temp, &temp->base.matrix[4*3], new_pos);
      vect3_copy(new_pos, &temp->base.matrix[4*3]);

      vect3_zero(temp->speed);
      vect3_zero(temp->spin);

      gta_interior_id_set(interior_id);
      temp->base.interior_id = (uint8_t)interior_id;

      if(!set.trailer_support)
         break;
   }
}


void cheat_handle_vehicle_unflip(struct vehicle_info *info, float time_diff)
{
	static float unflip_rotation;

	/* Unflip */
	if(KEY_PRESSED(set.key_unflip))
	{
		/* get the vehicle's yaw angle (z-axis) */
		unflip_rotation = atan2f(info->base.matrix[4*1+0], info->base.matrix[4*1+1]);
	}

	/* Rotate vehicle while the unflip key is held down */
	if(KEY_DOWN(set.key_unflip))
	{
		float a = unflip_rotation;
		float *m = info->base.matrix;
		float matrix[16] = {
			cosf(a),		-sinf(a),		0.0f,		0.0f,		// right
			sinf(a),		cosf(a),		0.0f,		0.0f,		// attitude
			0.0f,			0.0f,			1.0f,		0.0f,		// up
			m[4*3+0],		m[4*3+1],		m[4*3+2],	1.0f		// position
		};

		matrix_copy(matrix, info->base.matrix);
		vect3_zero(info->spin);
		unflip_rotation += time_diff * M_PI;
	}
}


void cheat_vehicle_air_brake_set(int enabled)
{
   cheat_state->vehicle.air_brake = enabled;
   cheat_handle_vehicle_air_brake(NULL, 0.0f);
}

void cheat_handle_vehicle_air_brake(struct vehicle_info *info, float time_diff)
{
   static float orig_matrix[16];
   static int orig_matrix_set;
   struct vehicle_info *temp;

   if(info == NULL)
   {
      orig_matrix_set = 0;
      return;
   }

   if(set.air_brake_toggle)
   {
	   if(KEY_PRESSED(set.key_air_brake_mod))
         cheat_state->vehicle.air_brake ^= 1;

      if(KEY_PRESSED(set.key_air_brake_mod2) && cheat_state->vehicle.air_brake)
         cheat_state->vehicle.air_brake_slowmo ^= 1;
   }
   else
   {
      if(KEY_PRESSED(set.key_air_brake_mod))
         cheat_state->vehicle.air_brake = 1;
      else if(KEY_RELEASED(set.key_air_brake_mod))
         cheat_state->vehicle.air_brake = 0;

      if(KEY_PRESSED(set.key_air_brake_mod2) && cheat_state->vehicle.air_brake)
         cheat_state->vehicle.air_brake_slowmo = 1;
      else if(KEY_RELEASED(set.key_air_brake_mod2) && cheat_state->vehicle.air_brake)
         cheat_state->vehicle.air_brake_slowmo = 0;
   }

   if(cheat_state->vehicle.air_brake && !orig_matrix_set)
   {
      matrix_copy(info->base.matrix, orig_matrix);
      orig_matrix_set = 1;
   }

   if(!cheat_state->vehicle.air_brake)
      orig_matrix_set = 0;

   if(cheat_state->vehicle.air_brake)
   {
      if(!KEY_DOWN(set.key_unflip))    /* allow the unflip rotation feature to work */
         matrix_copy(orig_matrix, info->base.matrix);

      /* XXX allow some movement */
      for(temp = info; temp != NULL; temp = temp->trailer)
      {
         /* prevent all kinds of movement */
         vect3_zero(temp->speed);
         vect3_zero(temp->spin);

         if(!set.trailer_support)
            break;
      }
   }

   if(cheat_state->vehicle.air_brake)
   {
      static uint32_t time_start;
      float *matrix = info->base.matrix;
      float d[4] = { 0.0f, 0.0f, 0.0f, time_diff * set.air_brake_speed };
      float xyvect[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; /* rotation vector */
      float zvect[4]  = { 0.0f, 0.0f, 0.0f, 0.0f }; /* rotation vector */
      float theta = set.air_brake_rot_speed * time_diff * M_PI * 2.0f;


      if(cheat_state->vehicle.air_brake_slowmo)
      {
         d[3] /= 10.0f;
         theta /= 2.0f;
      }

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

      /* pitch (x-axis) */
      if(KEY_DOWN(set.key_air_brake_rot_pitch1))   xyvect[0] += 1.0f;
      if(KEY_DOWN(set.key_air_brake_rot_pitch2))   xyvect[0] -= 1.0f;
      /* roll (y-axis) */
      if(KEY_DOWN(set.key_air_brake_rot_roll1))    xyvect[1] += 1.0f;
      if(KEY_DOWN(set.key_air_brake_rot_roll2))    xyvect[1] -= 1.0f;
      /* yaw (z-axis) */
      if(KEY_DOWN(set.key_air_brake_rot_yaw1))     zvect[2] -= 1.0f;
      if(KEY_DOWN(set.key_air_brake_rot_yaw2))     zvect[2] += 1.0f;

      if(!vect3_near_zero(xyvect))
      {
         vect3_normalize(xyvect, xyvect);

         matrix_vect4_mult(matrix, xyvect, xyvect);
         matrix_vect3_rotate(matrix, xyvect, theta, matrix);
      }

      if(!vect3_near_zero(zvect))
      {
         vect3_normalize(zvect, zvect);

         matrix_vect3_rotate(matrix, zvect, theta, matrix);
      }

      switch(set.air_brake_behaviour)
      {
      case 0:
         matrix[4*1+0] += d[0] * d[3];
         matrix[4*1+1] += d[1] * d[3];
         matrix[4*1+2] += d[2] * d[3];
         break;

      case 1:  /* mixed 2d/3d mode */
         {
            /* convert the vehicle's 3d rotation vector to a 2d angle */
            float a = atan2f(matrix[4*1+0], matrix[4*1+1]);

            /* calculate new position */
            matrix[4*3+0] += (d[0] * sinf(a) - d[1] * cosf(a)) * d[3];
            matrix[4*3+1] += (d[0] * cosf(a) + d[1] * sinf(a)) * d[3];
            matrix[4*3+2] += d[2] * d[3];
         }
         break;

      case 2:  /* full 3d movement */
         if(!vect3_near_zero(d))
         {
            float vect[4] = { -d[1], d[0], d[2], 0.0f };  /* swap x & y + invert y */
            float out[4];

            /* out = matrix * norm(d) */
            vect3_normalize(vect, vect);
            matrix_vect4_mult(matrix, vect, out);

            matrix[4*3+0] += out[0] * d[3];
            matrix[4*3+1] += out[1] * d[3];
            matrix[4*3+2] += out[2] * d[3];
         }
         break;
      }

      matrix_copy(matrix, orig_matrix);
   }
}


void cheat_handle_vehicle_warp(struct vehicle_info *info, float time_diff)
{
   static struct vehicle_state state;
   static uint32_t warp_time = 0;
   static int warping = 0;


   if(warp_time != 0)
   {
      if(time_get() - warp_time > MSEC_TO_TIME(100))
      {
         warp_time = 0;
      }
      else
      {
         vehicle_state_restore(info, &state);
      }
   }

   /* warp factor: fuck you */
   if(KEY_PRESSED(set.key_warp_mod))
   {
      struct vehicle_info *temp;
      float dir[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
      float vect[4];

      if(set.warp_use_speed &&
         !vect3_near_zero(info->speed) &&
         vect3_length(info->speed) >= 0.01f)
      {
         /* use normalized speed vector */
         vect3_copy(info->speed, dir);
         vect3_normalize(dir, vect);
      }
      else
      {
         /* use vehicle direction matrix */
         matrix_vect4_mult(info->base.matrix, dir, vect);
         if(vect3_near_zero(vect))
            return;  /* shouldn't happen */
      }

      vehicle_state_store(info, &state);
      warp_time = 0;
      for(temp = info; temp != NULL; temp = temp->trailer)
      {
         vect3_mult(vect, set.warp_speed, temp->speed);

         if(!set.trailer_support)
            break;
      }
   }

   if(KEY_RELEASED(set.key_warp_mod))
   {
      struct vehicle_info *temp;

      warp_time = time_get();
      vehicle_state_restore(info, &state);
      for(temp = info; temp != NULL; temp = temp->trailer)
      {
         temp->collision_something = 0.0f;

         if(!set.trailer_support)
            break;
      }
   }
}


void cheat_handle_vehicle_hop(struct vehicle_info *info, float time_diff)
{
   struct vehicle_info *temp;

   if(KEY_DOWN(set.key_vehicle_hop))
      for(temp = info; temp != NULL; temp = temp->trailer)
      {
         temp->speed[2] = set.vehicle_hop_speed;

         if(!set.trailer_support)
            break;
      }
}


void cheat_handle_vehicle_brake(struct vehicle_info *info, float time_diff)
{
   float speed;
   struct vehicle_info *temp;

   if(KEY_DOWN(set.key_brake_mod))
   {
      for(temp = info; temp != NULL; temp = temp->trailer)
      {
         speed = vect3_length(temp->speed);
         vect3_normalize(temp->speed, temp->speed);

         speed -= time_diff * set.brake_mult;
         if(speed < 0.0f)
            speed = 0.0f;

         if(vect3_near_zero(temp->speed))
            vect3_zero(temp->speed);
         else
            vect3_mult(temp->speed, speed, temp->speed);

         if(!set.trailer_support)
            break;
      }
   }
}

void cheat_handle_vehicle_nitro(struct vehicle_info *info, float time_diff)
{
	static uint32_t timer;
	static float speed_off;
	float pre_speed[3];
	struct vehicle_info *temp;


	if(KEY_DOWN(set.key_warp_mod))
		vect3_copy(info->speed, pre_speed);

	if(KEY_PRESSED(set.key_nitro_mod))
	{
		speed_off = vect3_length(info->speed);
		timer = time_get();
	}

	/* "nitro" acceleration mod */
	if(KEY_DOWN(set.key_nitro_mod) && !vect3_near_zero(info->speed))
	{
		float etime = TIME_TO_FLOAT(time_get() - timer) / set.nitro_accel_time;
		float speed = set.nitro_high;

		if(!near_zero(set.nitro_accel_time))
			etime += 1.0f - (set.nitro_high - speed_off) / set.nitro_high;

		if(etime < 1.0f && !near_zero(set.nitro_accel_time))
			speed = set.nitro_high * etime;

		for(temp = info; temp != NULL; temp = temp->trailer)
		{
			if(!vect3_near_zero(temp->speed))
			{
				vect3_normalize(temp->speed, temp->speed);
				vect3_mult(temp->speed, speed, temp->speed);
				if(vect3_near_zero(temp->speed))
					vect3_zero(temp->speed);
			}

			if(!set.trailer_support)
				break;
		}
	}

	/* actual NOS of the game, toggle infinite NOS, w00t! */
	if(KEY_PRESSED(set.key_nitro))
	{
		if (!cheat_state->vehicle.infNOS_toggle_on)
		{
			cheat_state->vehicle.infNOS_toggle_on = true;
			patcher_install(&patch_vehicle_inf_NOS);
			vehicle_addUpgrade(info, 1010);
		}
		else
		{
			cheat_state->vehicle.infNOS_toggle_on = false;
			patcher_remove(&patch_vehicle_inf_NOS);
			GTAfunc_removeVehicleUpgrade(info, 1010);
		}
	}
}

void cheat_handle_vehicle_quick_turn_180(struct vehicle_info *info, float time_diff)
{
   if(KEY_PRESSED(set.key_quick_turn_180))
   {
      /* simply invert the X and Y axis.. */
      vect3_invert(&info->base.matrix[4*0], &info->base.matrix[4*0]);
      vect3_invert(&info->base.matrix[4*1], &info->base.matrix[4*1]);
      vect3_invert(info->speed, info->speed);
   }
}

//MATRIX4X4
//	VECTOR right
//		float X [4*0+0]
//		float Y [4*0+1]
//		float Z [4*0+2] Roll
// DWORD  flags;
// VECTOR up;
//		float X [4*1+0]
//		float Y [4*1+1]
//		float Z [4*1+2] Pitch
// float  pad_u;
// VECTOR at;
//		float X [4*2+0]
//		float Y [4*2+1]
//		float Z [4*2+2]
// float  pad_a;
// VECTOR pos;
//		float X [4*3+0]
//		float Y [4*3+1]
//		float Z [4*3+2]
// float  pad_p;
void cheat_handle_vehicle_quick_turn_left(struct vehicle_info *vinfo, float time_diff)
{
	if(KEY_PRESSED(set.key_quick_turn_left))
	{
		// do new heading
		float *heading_matrix = vinfo->base.matrix;
		MATRIX4X4 *heading_matrix4x4 = vinfo->base.matrixStruct;
		// rotate on z axis
		float heading_vector_zrotate[4] = { 0.0f, 0.0f, -90.0f, 0.0f };
		float heading_theta = M_PI / 2.0f;
		vect3_normalize(heading_vector_zrotate, heading_vector_zrotate);
		matrix_vect3_rotate(heading_matrix, heading_vector_zrotate, heading_theta, heading_matrix);
		// compensate pitch & yaw
		float pitch = heading_matrix4x4->up.Z;
		float roll = heading_matrix4x4->right.Z;
		heading_matrix4x4->up.Z = -roll;
		heading_matrix4x4->right.Z = pitch;

		// do new speed
		float *speed_vector = vinfo->speed;
		if(!vect3_near_zero(speed_vector))
		{
			float speed_vector_return[4];
			float speed_theta = 1.0f;
			//
			vect3_copy(speed_vector, speed_vector_return);
			speed_vector_return[1] = speed_vector[0];
			speed_vector_return[0] = -speed_vector[1];
			vect3_copy(speed_vector_return, speed_vector);
		}
	}
}

void cheat_handle_vehicle_quick_turn_right(struct vehicle_info *vinfo, float time_diff)
{
	if(KEY_PRESSED(set.key_quick_turn_right))
	{
		// do new heading
		float *heading_matrix = vinfo->base.matrix;
		MATRIX4X4 *heading_matrix4x4 = vinfo->base.matrixStruct;
		// rotate on z axis
		float heading_zvectrotate[4] = { 0.0f, 0.0f, 90.0f, 0.0f };
		float heading_theta = M_PI / 2.0f;
		vect3_normalize(heading_zvectrotate, heading_zvectrotate);
		matrix_vect3_rotate(heading_matrix, heading_zvectrotate, heading_theta, heading_matrix);
		// compensate pitch & yaw
		float pitch = heading_matrix4x4->up.Z;
		float roll = heading_matrix4x4->right.Z;
		heading_matrix4x4->up.Z = roll;
		heading_matrix4x4->right.Z = -pitch;

		// do new speed
		float *speed_vector = vinfo->speed;
		if(!vect3_near_zero(speed_vector))
		{
			float speed_vector_return[4];
			float speed_theta = 1.0f;
			//
			vect3_copy(speed_vector, speed_vector_return);
			speed_vector_return[1] = -speed_vector[0];
			speed_vector_return[0] = speed_vector[1];
			vect3_copy(speed_vector_return, speed_vector);
		}
	}
}


void cheat_handle_vehicle_protection(struct vehicle_info *info, float time_diff)
{
   static float last_spin[3];
   struct vehicle_info *temp;

   if(KEY_PRESSED(set.key_protection))
      cheat_state->vehicle.protection ^= 1;

   if(KEY_DOWN(set.key_warp_mod))
      return;

   if(cheat_state->vehicle.protection)
   {
      for(temp = info; temp != NULL; temp = temp->trailer)
      {
         if(!vect3_near_zero(temp->spin) && vect3_length(temp->spin) > set.protection_spin_cap)
         {
            vect3_normalize(temp->spin, temp->spin);
            vect3_mult(temp->spin, set.protection_spin_cap, temp->spin);
         }

         if(!vect3_near_zero(temp->speed) && vect3_length(temp->speed) > set.protection_speed_cap)
         {
            vect3_normalize(temp->speed, temp->speed);
            vect3_mult(temp->speed, set.protection_speed_cap, temp->speed);
         }

         if(temp->base.matrix[4*2+2] < 0.0f)
         {
            if(!vect3_near_zero(last_spin) && !vect3_near_zero(temp->spin) && vect3_length(temp->spin) < 0.10f)
            {
               vect3_normalize(temp->spin, temp->spin);
               vect3_mult(temp->spin, 0.10f, temp->spin);
            }
         }
         else if(temp == info) /* only copy the first vehicle's spin info */
         {
            vect3_copy(temp->spin, last_spin);
         }

         vehicle_prevent_below_height(temp, set.protection_min_height);

         if(!set.trailer_support)
            break;
      }
   }
}

void cheat_handle_vehicle_engine(struct vehicle_info *vehicle_info, float time_diff)
{
	if(vehicle_info == NULL) return;
	struct vehicle_info *veh_self = vehicle_info_get(VEHICLE_SELF, 0);
	if(veh_self == NULL) return;

	if(KEY_PRESSED(set.key_engine))
	{
		if(veh_self->m_nVehicleFlags.bEngineOn)
		{
			veh_self->m_nVehicleFlags.bEngineOn = 0;
			cheat_state->vehicle.is_engine_on = 0;
		} else {
			veh_self->m_nVehicleFlags.bEngineOn = 1;
			cheat_state->vehicle.is_engine_on = 1;
		}
	}
}

void cheat_handle_vehicle_breakdance(struct vehicle_info *vehicle_info, float time_diff)
{
	if(cheat_state->vehicle.air_brake) return;
	if(cheat_state->vehicle.stick) return;

	static float velpos, velneg;
	
	if(KEY_PRESSED(set.key_brkd_toggle))
	{
		cheat_state->vehicle.brkdance ^= 1;

		velpos = set.brkdance_velocity;
		velneg = -set.brkdance_velocity;
   }

   if(cheat_state->vehicle.brkdance)
   {
	   // we should probably actually be driving the vehicle
	   struct actor_info *actor = actor_info_get(ACTOR_SELF, 0);
	   if (actor == NULL) return; // we're not an actor? lulz
	   if (actor->state != ACTOR_STATE_DRIVING) return; // we're not driving?
	   if (actor->vehicle->passengers[1] == actor) return; // we're not passenger in an airplane?

	   int iVehicleID = getPlayerVehicleGTAScriptingID(ACTOR_SELF);

	   if(KEY_DOWN(set.key_brkd_forward))
	   {
		   ScriptCommand(&apply_momentum_in_direction_XYZ,      iVehicleID, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		   ScriptCommand(&apply_rotory_pulse_about_an_axis_XYZ, iVehicleID, velneg, 0.0f, 0.0f);
	   }
	   else if(KEY_DOWN(set.key_brkd_backward))
	   {
		   ScriptCommand(&apply_momentum_in_direction_XYZ,      iVehicleID, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		   ScriptCommand(&apply_rotory_pulse_about_an_axis_XYZ, iVehicleID, velpos, 0.0f, 0.0f);
	   }
	   else if(KEY_DOWN(set.key_brkd_right))
	   {
		   ScriptCommand(&apply_momentum_in_direction_XYZ,      iVehicleID, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		   ScriptCommand(&apply_rotory_pulse_about_an_axis_XYZ, iVehicleID, 0.0f, velpos, 0.0f);
	   }
	   else if(KEY_DOWN(set.key_brkd_left))
	   {
		   ScriptCommand(&apply_momentum_in_direction_XYZ,      iVehicleID, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		   ScriptCommand(&apply_rotory_pulse_about_an_axis_XYZ, iVehicleID, 0.0f, velneg, 0.0f);
	   }
	   else if(KEY_DOWN(set.key_brkd_rightward))
	   {
		   ScriptCommand(&apply_momentum_in_direction_XYZ,      iVehicleID, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		   ScriptCommand(&apply_rotory_pulse_about_an_axis_XYZ, iVehicleID, 0.0f, 0.0f, velneg);
	   }
	   else if(KEY_DOWN(set.key_brkd_leftward))
	   {
		   ScriptCommand(&apply_momentum_in_direction_XYZ,      iVehicleID, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		   ScriptCommand(&apply_rotory_pulse_about_an_axis_XYZ, iVehicleID, 0.0f, 0.0f, velpos);
	   }
   }
}

void cheat_handle_fast_exit(struct vehicle_info *vehicle_info, float time_diff)
{
	if(KEY_PRESSED(set.key_fast_exit))
	{
		float *coord = (cheat_state->state == CHEAT_STATE_VEHICLE) ? cheat_state->vehicle.coords : cheat_state->actor.coords;
		ScriptCommand(&remove_actor_from_car_and_put_at,1,coord[0],coord[1] + 2.0f,coord[2]);
	}
}

void cheat_handle_repair_car(struct vehicle_info *vehicle_info, float time_diff)
{
	if(KEY_PRESSED(set.key_repair_car))
	{
		// get info
		struct vehicle_info *veh_self = vehicle_info_get(VEHICLE_SELF, 0);
		if(veh_self == NULL) return;
		struct actor_info *self = actor_info_get(ACTOR_SELF, 0);
		if(self == NULL) return;
		// make sure we are driving our own vehicle
		if(self->state == ACTOR_STATE_DRIVING
			&& self->vehicle->passengers[0] == self)
		{
			// fix the vehicle
			int iVehicleID = getPlayerVehicleGTAScriptingID(ACTOR_SELF);
			ScriptCommand(&repair_car, iVehicleID);
		}
	}
}



































int g_cheatVehicleMagnetWheels_Enabled = false;

float CVectorGetLength(VECTOR vecGravity)
{
	traceLastFunc("CVectorGetLength()");
	return sqrt( (vecGravity.X*vecGravity.X) + (vecGravity.Y*vecGravity.Y) + (vecGravity.Z*vecGravity.Z) );
}

VECTOR CVectorNormalize(VECTOR vecGravity) 
{ 
	traceLastFunc("CVectorNormalize()");
	double t = CVectorGetLength(vecGravity);
	if (t > FLOAT_EPSILON)
	{
		double fX2 = vecGravity.X / t;
		double fY2 = vecGravity.Y / t;
		double fZ2 = vecGravity.Z / t;
		vecGravity.X = (float)fX2;
		vecGravity.Y = (float)fY2;
		vecGravity.Z = (float)fZ2;
	}
	else
	{
		vecGravity.X = 0;
		vecGravity.Y = 0;
		vecGravity.Z = 0;
	}
	return vecGravity;
}

VECTOR VectorCrossProduct(VECTOR inputVector, VECTOR param)
{
	traceLastFunc("VectorCrossProduct()");
	float _fX = inputVector.X, _fY = inputVector.Y, _fZ = inputVector.Z;
	inputVector.X = _fY * param.Z - param.Y * _fZ;
	inputVector.Y = _fZ * param.X - param.Z * _fX;
	inputVector.Z = _fX * param.Y - param.X * _fY;
	return inputVector;
}

MATRIX4X4 GetMatrixForGravity(VECTOR vecGravity)
{
	traceLastFunc("GetMatrixForGravity()");
	MATRIX4X4 mat;
    // Calculates a basis where the z axis is the inverse of the gravity
    if ( CVectorGetLength(vecGravity) > 0.0001f )
    {
		mat.at.X = -vecGravity.X;
		mat.at.Y = -vecGravity.Y;
		mat.at.Z = -vecGravity.Z;
        mat.at = CVectorNormalize(mat.at);
		if ( fabs(mat.at.X) > 0.0001f || fabs(mat.at.Z) > 0.0001f )
        {
            VECTOR y_XProdTemp;// = (0.0f, 1.0f, 0.0f);
			y_XProdTemp.X = 0.0f;
			y_XProdTemp.Y = 1.0f;
			y_XProdTemp.Z = 0.0f;
            mat.up = vecGravity;
            mat.up = VectorCrossProduct(mat.up, y_XProdTemp);
            mat.up = VectorCrossProduct(mat.up, vecGravity);
            mat.up = CVectorNormalize(mat.up);
        }
        else
        {
			mat.up.X = 0.0f;
			mat.up.Y = 0.0f;
			mat.up.Z = vecGravity.Y;
        }
		mat.right = mat.up;
        mat.right = VectorCrossProduct(mat.right, mat.up);
    }
    else
    {
		// No gravity, use default axes
		mat.right.X = 1.0f; // = VECTOR ( 1.0f, 0.0f, 0.0f );
		mat.right.Y = 0.0f;
		mat.right.Z = 0.0f;
		mat.up.X = 0.0f; //    = VECTOR ( 0.0f, 1.0f, 0.0f );
		mat.up.Y = 1.0f;
		mat.up.Z = 0.0f;
		mat.at.X = 0.0f; //    = VECTOR ( 0.0f, 0.0f, 1.0f );
		mat.at.Y = 0.0f;
		mat.at.X = 1.0f;
    }
	return mat;
}

MATRIX4X4 MatrixInvert(MATRIX4X4 inputMatrix)
{
	traceLastFunc("MatrixInvert()");
	// Transpose the rotation matrix and negate the position
	VECTOR vOldRight = inputMatrix.right, vOldFront = inputMatrix.up, vOldUp = inputMatrix.at;
	inputMatrix.right.X = vOldRight.X; // vRight = (VECTOR) ( vOldRight.X, vOldFront.X, vOldUp.X );
	inputMatrix.right.Y = vOldFront.X;
	inputMatrix.right.Z = vOldUp.X;
	inputMatrix.up.X = vOldRight.Y; // vFront = VECTOR ( vOldRight.Y, vOldFront.Y, vOldUp.Y );
	inputMatrix.up.Y = vOldFront.Y; 
	inputMatrix.up.Z = vOldUp.Y; 
	inputMatrix.at.X = vOldRight.Z; // vUp = VECTOR ( vOldRight.Z, vOldFront.Z, vOldUp.Z );
	inputMatrix.at.Y = vOldFront.Z;
	inputMatrix.at.Z = vOldUp.Z;
	inputMatrix.pos.X *= -1.0f;
	inputMatrix.pos.Y *= -1.0f;
	inputMatrix.pos.Z *= -1.0f;
	return inputMatrix;
}

VECTOR VectorMuliplyMatrix(VECTOR vec, MATRIX4X4 mat)
{
	traceLastFunc("VectorMuliplyMatrix()");
	vec.X = mat.right.X*vec.X + mat.up.X*vec.Y + mat.at.X*vec.Z;
	vec.Y = mat.right.Y*vec.X + mat.up.Y*vec.Y + mat.at.Y*vec.Z;
	vec.Z = mat.right.Z*vec.X + mat.up.Z*vec.Y + mat.at.Z*vec.Z;
	return vec;
}

VECTOR VectorMultiplyFloat(VECTOR vec, float flt)
{
	traceLastFunc("VectorMultiplyFloat()");
	vec.X *= flt;
	vec.Y *= flt;
	vec.Z *= flt;
	return vec;
}

VECTOR VectorSubtractVector(VECTOR vecInOut, VECTOR vecToSubtract)
{
	traceLastFunc("VectorSubtractVector()");
	vecInOut.X -= vecToSubtract.X;
	vecInOut.Y -= vecToSubtract.Y;
	vecInOut.Z -= vecToSubtract.X;
	return vecInOut;
}

VECTOR VectorAddVector(VECTOR vecInOut, VECTOR vecToAdd)
{
	traceLastFunc("VectorAddVector()");
	vecInOut.X += vecToAdd.X;
	vecInOut.Y += vecToAdd.Y;
	vecInOut.Z += vecToAdd.X;
	return vecInOut;
}

VECTOR vecForDisp;
void CPhysical_ApplyGravity(DWORD dwThis)
{
	traceLastFunc("CPhysical_ApplyGravity()");
	// dwThis should be coming from HOOK_CPhysical_ApplyGravity
    DWORD dwType;
    __asm
    {
        mov ecx, dwThis
        mov eax, 0x46A2C0       // CEntity::GetType
        call eax
        mov dwType, eax
    }

    float fTimeStep = *(float *)0xB7CB5C;
    float fGravity  = *(float *)0x863984;

    if (dwType == 2)
    {
		// get our vehicle_info pointer to see if it's dwThis
		struct vehicle_info *vinfo_self = vehicle_info_get(VEHICLE_SELF, 0);
		struct actor_info *ainfo_self = actor_info_get(ACTOR_SELF, 0);
		if(vinfo_self != NULL
			&& vinfo_self == (vehicle_info*)dwThis
			&& vinfo_self->passengers[0] == ainfo_self)
		{
			// it's our vehicle, and we're driving, use the gravity vector

			VECTOR vecGravity = cheat_state->vehicle.gravityVector;
			VECTOR vecMoveSpeed = GTAfunc_GetMoveSpeed(vinfo_self);

			float gravityZcompensationMultiplier = vecGravity.Z - vecGravity.X;
			vecMoveSpeed.Z += (fTimeStep * fGravity) * gravityZcompensationMultiplier;

			//vecMoveSpeed += vecGravity * fTimeStep * fGravity;
			vecGravity = VectorMultiplyFloat(vecGravity, fTimeStep * fGravity);

			vecForDisp = vecGravity;

			vecMoveSpeed = VectorAddVector(vecMoveSpeed, vecGravity);
			GTAfunc_SetMoveSpeed(vinfo_self, vecMoveSpeed);
		}
		else
		{
			// it's not our vehicle, return regular gravity
			*(float *)(dwThis + 0x4C) -= fTimeStep * fGravity;
		}
    }
    else
    {
        // It's something else, apply regular downward gravity (+0x4C == m_vecMoveSpeed.fZ)
        *(float *)(dwThis + 0x4C) -= fTimeStep * fGravity;
    }
}

/*
CMatrix
	vRight = CVector ( 1.0f, 0.0f, 0.0f );
	vFront = CVector ( 0.0f, 1.0f, 0.0f );
	vUp    = CVector ( 0.0f, 0.0f, 1.0f );
	vPos   = CVector ( 0.0f, 0.0f, 0.0f );
MATRIX4X4
	VECTOR right;	// vRight
	VECTOR up;		// vFront
	VECTOR at;		// vUp
	VECTOR pos;		// vPos
*/

VECTOR cheat_vehicle_getPositionUnder(vehicle_info *vinfo)
{
	traceLastFunc("cheat_vehicle_getPositionUnder()");
	VECTOR offsetVector;
	float *matrix = vinfo->base.matrix;
	offsetVector.X = 0 * matrix[0] + 0 * matrix[4] - 1 * matrix[8];
    offsetVector.Y = 0 * matrix[1] + 0 * matrix[5] - 1 * matrix[9];
    offsetVector.Z = 0 * matrix[2] + 0 * matrix[6] - 1 * matrix[10];
	return offsetVector;
}

void cheat_vehicle_setGravity(vehicle_info *vinfo, VECTOR pvecGravity)
{
	traceLastFunc("cheat_vehicle_setGravity()");
	// 3:07:16 PM how you going
/*
// this crap is too annoying until we get the rotation setup properly
	//CCam* pCam = pGame->GetCamera ()->GetCam ( pGame->GetCamera ()->GetActiveCam () );
	CCamSA *pCam = &g_CCamera.Cams[g_CCamera.internalInterface->ActiveCam];

	//CMatrix matOld, matNew;
	MATRIX4X4 matOld = GetMatrixForGravity(cheat_state->vehicle.gravityVector);
	MATRIX4X4 matNew = GetMatrixForGravity(pvecGravity);

	//CVector* pvecPosition = &m_pInterface->Placeable.matrix->vPos;
	VECTOR pvecPosition = vinfo->base.matrixStruct->pos;

	//matOld.Invert ();
	matOld = MatrixInvert(matOld);

	//5:08:18 PM lol cool

	//pCam->GetTargetHistoryPos () [ 0 ] = matOld * (pCam->GetTargetHistoryPos () [ 0 ] - *pvecPosition);
	pCam->m_pInterface->m_aTargetHistoryPos[0] = VectorMuliplyMatrix( VectorSubtractVector(pCam->m_pInterface->m_aTargetHistoryPos[0],pvecPosition), matOld );
	//pCam->GetTargetHistoryPos () [ 0 ] = matNew * pCam->GetTargetHistoryPos () [ 0 ] + *pvecPosition;
	pCam->m_pInterface->m_aTargetHistoryPos[0] = VectorAddVector( VectorMuliplyMatrix(pCam->m_pInterface->m_aTargetHistoryPos[0],matNew), pvecPosition );

	//pCam->GetTargetHistoryPos () [ 1 ] = matOld * (pCam->GetTargetHistoryPos () [ 1 ] - *pvecPosition);
	pCam->m_pInterface->m_aTargetHistoryPos[1] = VectorMuliplyMatrix( VectorSubtractVector(pCam->m_pInterface->m_aTargetHistoryPos[1],pvecPosition), matOld );
	//pCam->GetTargetHistoryPos () [ 1 ] = matNew * pCam->GetTargetHistoryPos () [ 1 ] + *pvecPosition;
	pCam->m_pInterface->m_aTargetHistoryPos[1] = VectorAddVector( VectorMuliplyMatrix(pCam->m_pInterface->m_aTargetHistoryPos[1],matNew), pvecPosition );
*/

	cheat_state->vehicle.gravityVector = pvecGravity;
}

void cheat_vehicle_magnetWheels(vehicle_info *vinfo, int magnetWheels_on)
{
	traceLastFunc("cheat_vehicle_magnetWheels()");
	if (magnetWheels_on)
	{
		// update magnet wheels
		VECTOR offsetVector = cheat_vehicle_getPositionUnder(vinfo);

		// set the gravity
		cheat_vehicle_setGravity(vinfo, offsetVector);

		// set enabled
		g_cheatVehicleMagnetWheels_Enabled = true;
	}
	else
	{
		if (g_cheatVehicleMagnetWheels_Enabled)
		{
			// disable magnet wheels
			cheat_state->vehicle.gravityVector.X = 0.0f;
			cheat_state->vehicle.gravityVector.Y = 0.0f;
			cheat_state->vehicle.gravityVector.Z = -1.0f;
			// set disabled
			g_cheatVehicleMagnetWheels_Enabled = false;
		}
	}
}

void cheat_handle_magnetWheels(struct vehicle_info *vinfo, float time_diff)
{
	traceLastFunc("cheat_handle_magnetWheels()");
	// condition for setting/resetting magnetWheels should be here
	// in addition to the call to it, if needed

	// why in the fuck is this not working?!?
	if(KEY_PRESSED(set.key_magnetwheels))
	{
		cheat_state->vehicle.magnetWheels_on ^= 1;
		Log("magnetWheels set to: %d", cheat_state->vehicle.magnetWheels_on);
	}

	// temp on
	//cheat_vehicle_magnetWheels(vinfo, 1);
	
	// temp off
	cheat_vehicle_magnetWheels(vinfo, cheat_state->vehicle.magnetWheels_on);
}
