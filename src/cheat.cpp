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
#include <psapi.h>

/* Note that this is 100% C code, not C++. :p */
#ifndef MIN
#  define MIN(a,b)   ((a) > (b) ? (b) : (a))
#endif

// old camera thing, still used in RenderMapDot()
float *cam_matrix = (float*)0xB6F99C;

uint32_t       __time_current;
struct pool    *pool_actor;
struct pool    *pool_vehicle;

static struct cheat_state  __cheat_state;
struct cheat_state         *cheat_state = NULL;

static void cheat_main_actor(float time_diff)
{
	traceLastFunc("cheat_main_actor()");

	struct actor_info *info = actor_info_get(ACTOR_SELF, 0);
	if(info == NULL)
	{
		Log("wtf, actor_info_get() returned NULL.");
		return;
	}

	if (set.anti_carjacking)
	{
		if (cheat_state->_generic.got_vehicle_id) cheat_state->_generic.got_vehicle_id = false;
		if (cheat_state->_generic.anti_carjackTick
			&& cheat_state->_generic.anti_carjackTick < (GetTickCount() - 1000)
			&& cheat_state->_generic.car_jacked)
		{
			if (cheat_state->_generic.car_jacked_last_vehicle_id == 0)
			{
				showGameText("~r~Unable To Unjack~w~!", 1000, 5);
				cheat_state->_generic.anti_carjackTick = 0;
				cheat_state->_generic.car_jacked = false;
				return;
			}
			cheat_state->_generic.anti_carjackTick = 0;
			cheat_state->_generic.car_jacked = false;
			ScriptCommand(&put_actor_in_car, 1, cheat_state->_generic.car_jacked_last_vehicle_id);
			vect3_copy(cheat_state->_generic.car_jacked_lastPos, &actor_info_get(VEHICLE_SELF, 0)->base.matrix[4*3]);
			showGameText("~r~Car Unjacked~w~!", 1000, 5);
			return;
		}
	}

	vect3_copy(&info->base.matrix[4*3], cheat_state->actor.coords);
	cheat_handle_freeze_vehicles(NULL, info);
	cheat_handle_hp(NULL, info, time_diff);
	cheat_handle_teleport(NULL, info, time_diff);
	cheat_handle_unfreeze(NULL, info, time_diff);
	cheat_handle_emo(NULL, info, time_diff);
	// the following functions can be found in cheat_actor.cpp
	cheat_handle_actor_air_brake(info, time_diff);
	cheat_handle_stick(NULL, info, time_diff);
	cheat_handle_actor_autoaim(info, time_diff);
	cheat_handle_actor_nocols(info);
	//cheat_handle_spiderFeet(info, time_diff);
}

static void cheat_main_vehicle(float time_diff)
{
	traceLastFunc("cheat_main_vehicle()");
	
	struct vehicle_info *info = vehicle_info_get(VEHICLE_SELF, 0);
	if(info == NULL)
	{
		Log("wtf, vehicle_info_get() returned NULL.");
		return;
	}

// this should have its own handler
	if(set.anti_carjacking)
	{
		if(info->passengers[0] == actor_info_get(ACTOR_SELF, 0))
		{
			if(g_SAMP != NULL && !cheat_state->_generic.got_vehicle_id)
			{
				cheat_state->_generic.car_jacked_last_vehicle_id = getPlayerVehicleGTAScriptingID(ACTOR_SELF);
				if (cheat_state->_generic.car_jacked_last_vehicle_id > 0)
					cheat_state->_generic.got_vehicle_id = true;
			}
		}
	}
	
	//Making the vehicle instant jumping and the trailer attaching easier
	// this should be built into the appropriate handler
	if(cheat_state->_generic.nocols_toggled){
		int veh_id = vehicle_find_nearest(NULL);
		struct vehicle_info *veh = vehicle_info_get(veh_id, 0);
		if(veh == NULL){
			cheat_state->_generic.nocols_enabled = 0;
		}else{
			cheat_state->_generic.nocols_enabled = 1;
		}
	}else if(cheat_state->_generic.nocols_enabled){
		int veh_id = vehicle_find_nearest(NULL);
		struct vehicle_info *veh = vehicle_info_get(veh_id, 0);
		float dist[3] = {0.0f, 0.0f, 0.0f};
		if(veh != NULL)
			vect3_vect3_sub(&info->base.matrix[4*3], &veh->base.matrix[4*3], dist);
		if((info->trailer != NULL && (GetTickCount() - 250) > cheat_state->_generic.nocols_change_tick)
			|| vect3_length(dist)>=10.0f)
			cheat_state->_generic.nocols_enabled = 0;
	}

	vect3_copy(&info->base.matrix[4*3], cheat_state->vehicle.coords);
	cheat_handle_teleport(info, NULL, time_diff);
	cheat_handle_freeze_vehicles(info, NULL);
	cheat_handle_hp(info, NULL, time_diff);
	cheat_handle_unfreeze(info, NULL, time_diff);
	cheat_handle_emo(info, NULL, time_diff);
	// the following functions can be found in cheat_vehicle.cpp
	cheat_handle_vehicle_unflip(info, time_diff);
	cheat_handle_vehicle_nitro(info, time_diff);
	cheat_handle_vehicle_air_brake(info, time_diff);
	cheat_handle_vehicle_warp(info, time_diff);
	cheat_handle_stick(info, NULL, time_diff);
	cheat_handle_vehicle_protection(info, time_diff);
	cheat_handle_vehicle_quick_turn_180(info, time_diff);
	cheat_handle_vehicle_quick_turn_left(info, time_diff);
	cheat_handle_vehicle_quick_turn_right(info, time_diff);
	cheat_handle_vehicle_brake(info, time_diff);
	cheat_handle_vehicle_hop(info, time_diff);
	cheat_handle_vehicle_engine(info, time_diff);
	cheat_handle_vehicle_brakedance(info, time_diff);
	cheat_handle_blinking_carlights(info, time_diff);
	cheat_handle_vehicle_keepTrailer(info, time_diff);
	cheat_handle_repair_car(info, time_diff);
	cheat_handle_fast_exit(info, time_diff);
	cheat_handle_spiderWheels(info, time_diff);
}

// the main daddyo
extern bool isRequestingScreenshot;
int m_InitStages = 0;
void cheat_hook(HWND wnd)
{
   traceLastFunc("cheat_hook()");

   static uint32_t time_last;
   struct actor_info *actor_info;
   struct vehicle_info *vehicle_info;
   int i;

   /* for teh time_get() macro */
   __time_current = __time_get();

   keyhook_maybe_install(wnd);
   menu_maybe_init();

   /* initialize state */
   if(cheat_state == NULL)
   {
      cheat_state = &__cheat_state;
	  cheat_state->_generic.spoof_weapon         = -1;
      cheat_state->_generic.money                = set.money_enabled;
      cheat_state->_generic.weapon               = set.weapon_enabled;
      cheat_state->_generic.vehicles_unlock      = false;
      cheat_state->_generic.vehicles_warp_invert = true;
      cheat_state->actor.invulnerable            = true;
      cheat_state->vehicle.invulnerable          = true;
      cheat_state->vehicle.hp_tire_support       = true;
      cheat_state->vehicle.hp_minimum_on         = 1;
      cheat_state->vehicle.hp_damage_reduce_on   = 1;
      cheat_state->vehicle.hp_regen_on           = 1;
	  cheat_state->actor.hp_regen_on			 = 1;
      cheat_state->vehicle.brkdance				 = 0;
      cheat_state->game_speed                    = 1.0f;

	  // esp states
	  cheat_state->render_player_tags			= set.esp_players_defaulton;
	  cheat_state->render_vehicle_tags			= set.esp_vehicles_defaulton;
      cheat_state->_generic.teletext			= set.render_teleport_texts;
	  cheat_state->_generic.pickuptexts			= set.render_pickup_texts;
	  cheat_state->_generic.objecttexts			= set.render_object_texts;

	  cheat_state->vehicle.is_engine_on			= 1;
	  cheat_state->_generic.interior_patch_enabled = 0;

      /* install patches from the .ini file */
	   for(i=0; i<INI_PATCHES_MAX; i++)
	   {
		   if(set.patch[i].name != NULL && set.patch[i].ini_auto)
			   patcher_install(&set.patch[i]);
	   }
	   if(g_dwSAMP_Addr != NULL)
	   {
		   for(i=0; i<INI_SAMPPATCHES_MAX; i++)
		   {
			   if(set.sampPatch[i].name != NULL && set.sampPatch[i].ini_auto)
				   patcher_install(&set.sampPatch[i]);
		   }
	   }

   } /* end initialize state */


   cheat_state->state = CHEAT_STATE_NONE;

   /* setup & refresh actor pool */
   pool_actor = *(struct pool **)ACTOR_POOL_POINTER;
   if(pool_actor == NULL)          return;
   if(pool_actor->start == NULL)   return;
   if(pool_actor->size <= 0)       return;

   /* setup & refresh vehicle pool */
   pool_vehicle = *(struct pool **)VEHICLE_POOL_POINTER;
   if(pool_vehicle == NULL)          return;
   if(pool_vehicle->start == NULL)   return;
   if(pool_vehicle->size <= 0)       return;

//////////////////////////////////////////
// looks like we have pools so continue //
//////////////////////////////////////////

   actor_info = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);
   vehicle_info = vehicle_info_get(VEHICLE_SELF, 0);

	/* no vehicle, and no actor. exit. */
	if(vehicle_info == NULL && actor_info == NULL)
	{
		if(cheat_state->actor.air_brake
			|| cheat_state->actor.stick
			|| cheat_state->vehicle.air_brake
			|| cheat_state->vehicle.stick)
		{
			cheat_state->actor.air_brake = 0;
			cheat_vehicle_air_brake_set(0);
			cheat_state->actor.stick = 0;
			cheat_state->vehicle.stick = 0;
			cheat_state_text("Air brake / stick disabled");
		}
	}
	else
	{
		if(vehicle_info == NULL)
		{
			if(cheat_state->vehicle.air_brake || cheat_state->vehicle.stick)
			{
				cheat_vehicle_air_brake_set(0);
				cheat_state->vehicle.stick = 0;
				cheat_state_text("Air brake / stick disabled");
			}
			cheat_state->state = CHEAT_STATE_ACTOR;
			// reset infinite NOS toggle state
			if (cheat_state->vehicle.infNOS_toggle_on)
			{
				cheat_state->vehicle.infNOS_toggle_on = false;
				patcher_remove(&patch_vehicle_inf_NOS);
			}
		}
		else
		{
			if(cheat_state->actor.air_brake || cheat_state->actor.stick)
			{
				cheat_state->actor.air_brake = 0;
				cheat_state->actor.stick = 0;
				cheat_state_text("Air brake / stick disabled");
			}
			cheat_state->state = CHEAT_STATE_VEHICLE;
		}

		// load all the weapon models
		loadAllWeaponModels();

		if (m_InitStages == 0)
		{
			// LOOK WHAT I CAN DO!
			pGame = new CGameSA();
			pGameInterface = (CGame*)pGame;
			//pGameInterface->Initialize(); // all this does so far is disable modshops and pay&sprays
			pGameInterface->StartGame();
			// we have to add ourself to the pool first so that we are always the 1st ref
			// use CPOOLS_PED_SELF_REF to GetPedFromRef
			pGameInterface->GetPools()->AddPed((DWORD*)actor_info);

			// install all startup hooks
			cheat_hookers_installhooks();

			m_InitStages++;
		}
   }

   if(set.d3dtext_chat)
   {
	   if(g_Chat != NULL && g_Chat->iChatWindowMode)
	   {
			//Log("Disabling SA:MP chat text.");
			g_Chat->iChatWindowMode = 0;
			set.d3dtext_chat = 0;
	   }
   }
   if(set.d3dtext_kill)
   {
	   if(g_DeathList != NULL && g_DeathList->iEnabled)
	   {
			//Log("Disabling SA:MP kill list.");
			g_DeathList->iEnabled = 0;
			set.d3dtext_kill = 0;
	   }
   }

   if(rand() % 5000 == 0)
      srand(rand() + time_get());

   if(set.screenshot_enable)
   {
	   if(KEY_PRESSED(set.key_screenshot))
		   isRequestingScreenshot = true;
   }

   if(cheat_panic())
   {
      cheat_state->state = CHEAT_STATE_NONE;
      goto out;
   }

   menu_run();

   /* install volatile patches from the .ini file */
   for(i=0; i<INI_PATCHES_MAX; i++)
   {
      if(set.patch[i].name != NULL && set.patch[i].has_volatile && set.patch[i].installed)
         patcher_install(&set.patch[i]);
   }

   // generic stuff - these want to run even though we're dead 
   cheat_handle_debug(wnd);
   cheat_handle_misc();
   cheat_handle_spoof_weapon();
   gta_game_speed_set(cheat_state->game_speed);

   if(cheat_state->state != CHEAT_STATE_NONE)
   {
		// generic stuff
		cheat_handle_weapon_disable();
		cheat_handle_money();
		cheat_handle_weapon();

//////////////////////////////////////////////////////////////////
// this should be removed after reworking weapon cheat function //
//////////////////////////////////////////////////////////////////
if(set.restore_weapons_after_death) *(uint8_t *)0x5E6320 = 0xC3;

      cheat_handle_unlock();
	  cheat_handle_checkpoint();

      // do stuff :p
      if(cheat_state->state == CHEAT_STATE_VEHICLE)
         cheat_main_vehicle(TIME_TO_FLOAT(time_get() - time_last));
      else if(cheat_state->state == CHEAT_STATE_ACTOR)
         cheat_main_actor(TIME_TO_FLOAT(time_get() - time_last));

      for(i=0; i<INI_PATCHES_MAX; i++)
      {
         if(set.patch[i].name != NULL && KEY_PRESSED(set.patch[i].ini_hotkey))
         {
            if(set.patch[i].installed || set.patch[i].failed)
               patcher_remove(&set.patch[i]);
            else
               patcher_install(&set.patch[i]);
         }
      }

	  for(i=0; i<INI_SAMPPATCHES_MAX; i++)
	  {
		 if(set.sampPatch[i].name != NULL && KEY_PRESSED(set.sampPatch[i].ini_hotkey))
		 {
			if(set.sampPatch[i].installed || set.sampPatch[i].failed)
			   patcher_remove(&set.sampPatch[i]);
			else
			   patcher_install(&set.sampPatch[i]);
		 }
	  }

   }

   if(KEY_PRESSED(set.key_vehicle_jumper))
   {
	   int iVehicleID = vehicle_find_nearest(VEHICLE_ALIVE);
	   vehicleJumper(iVehicleID);
   }
   if(KEY_PRESSED(set.key_vehicle_occupied_jumper))
   {
	   int iVehicleID = vehicle_find_nearest(VEHICLE_ALIVE + VEHICLE_OCCUPIED);
	   vehicleJumper(iVehicleID);
   }

	// hack some SA:MP, shall we?
	if(g_dwSAMP_Addr != NULL && g_renderSAMP_initSAMPstructs)
		sampMainCheat();

out:;
   keyhook_run();
   time_last = time_get();
}
