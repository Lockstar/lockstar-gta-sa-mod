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


/////////////////////
// RUNTIME PATCHES //
/////////////////////

static struct patch_set patch_EnableResolutions =
{
	"Enable All Supported Resolutions",
	0,
	0,
	{
		{ 2, (void *)0x00745BC9, NULL, NULL, (uint8_t *)"\x7A\x6D" },
		{ 2, (void *)0x007459E1, NULL, NULL, (uint8_t *)"\x7A\x71" }
	}
};



//////////////////////
// FUNCTION PATCHES //
//////////////////////

static struct patch_set patch_vehicle_inf_NOS =
{
	"Vehicle Infinite NOS",
	0,
	0,
	{
		{ 1, (void *)0x006A3FFA, (uint8_t *)"\x7A", (uint8_t *)"\xEB", (uint8_t *)"\x7A" },
		{ 2, (void *)0x006A3FB9, (uint8_t *)"\xFE\xC8", (uint8_t *)"\x90\x90", (uint8_t *)"\xFE\xC8" }
	}
};

static struct patch_set patch_NotAPlane =
{
	"Its Not A Plane", //aka deactivate flying for planes
	0,
	0,
	{ 17, (void *)0x006CC46F, (uint8_t *)"\x51\x8B\x4C\x24\x20\x52\x50\x51\x6A\x03\x8B\xCE\xE8\x70\xC1\x00\x00", NULL, NULL }
};

static struct patch_set patch_NoColsWalls =
{
	"No collision walls", // Disable the wall-jump (jumping into air infront of walls)
	0,
	0,
	{ 1, (void *)((uint8_t *)0x679D3A), (uint8_t *)"\x74", (uint8_t *)"\xEB", (uint8_t *)"\x74" }
};


// function declarations
void cheat_patches_installRuntimePatches(void);
