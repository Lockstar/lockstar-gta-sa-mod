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
void				cheat_actor_teleport ( struct actor_info *info, const float pos[3], int interior_id );
void				cheat_handle_actor_autoaim ( struct actor_info *info, double time_diff );
void				cheat_handle_actor_air_brake ( struct actor_info *info, double time_diff );
void				cheat_handle_actor_nocols ( struct actor_info *info );
void				cheat_handle_spiderFeet ( struct actor_info *ainfo, float time_diff );

CEntitySAInterface	*cheat_actor_GetCEntitySAInterface ( actor_info *ainfo );
