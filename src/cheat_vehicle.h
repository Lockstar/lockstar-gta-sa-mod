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

void cheat_vehicle_teleport(struct vehicle_info *info, const float pos[3], int interior_id);
void cheat_handle_vehicle_unflip(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_air_brake(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_warp(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_nitro(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_stick(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_quick_turn_180(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_quick_turn_left(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_quick_turn_right(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_protection(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_brake(struct vehicle_info *info, float time_diff);
void cheat_handle_vehicle_hop(struct vehicle_info *info, float time_diff);
void cheat_vehicle_air_brake_set(int enabled);
void cheat_handle_vehicle_engine(struct vehicle_info *vehicle_info, float time_diff);
void cheat_handle_vehicle_brakedance(struct vehicle_info *vehicle_info, float time_diff);
void cheat_handle_fast_exit(struct vehicle_info *vehicle_info, float time_diff);
void cheat_handle_repair_car(struct vehicle_info *vehicle_info, float time_diff);
void cheat_handle_spiderWheels(vehicle_info *vinfo, float time_diff);
void CPhysical_ApplyGravity(DWORD dwThis);
CVector cheat_vehicle_getPositionUnder(vehicle_info *vinfo);

CEntitySAInterface* cheat_vehicle_GetCEntitySAInterface(vehicle_info *vinfo);
