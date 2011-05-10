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

extern TwBar *twBar_Main;

extern TwBar *twBar_SPCheats;
extern TwBar *twBar_SPCarUpgrades;
extern TwBar *twBar_SPCarColorPJ;
extern TwBar *twBar_SPWeapons;
extern TwBar *twBar_SPVehicles;
extern TwBar *twBar_SPTeleports;
extern TwBar *twBar_SPMisc;
extern TwBar *twBar_SPPatches;

extern TwBar *twBar_SAMPPlayers;
extern TwBar *twBar_SAMPFavServers;
extern TwBar *twBar_SAMPMisc;
extern TwBar *twBar_SAMPObjects;
extern TwBar *twBar_SAMPPickups;
extern TwBar *twBar_SAMPPatches;

extern struct gui *menu_background;

void runNewMenu();
void initializeBarsMenu();
DWORD WINAPI threadMenuUpdate(PVOID);
void toggleCursor(bool bToggle);
