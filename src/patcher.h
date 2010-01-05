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
	$HeadURL: https://m0d-s0beit-sa.googlecode.com/svn/trunk/src/patcher.h $
	$Id: patcher.h 31 2009-12-14 07:12:56Z futnucks $

*/

#define PATCHER_CHUNKS_MAX 16

struct patch_chunk
{
   uint32_t len;        /* length of data @ ptr */
   void *ptr;           /* ptr to data */
   uint8_t *data_cmp;   /* compare data against this */
   uint8_t *data_rep;   /* replacement data @ ptr, if NULL NOPs are used */
   uint8_t *data_orig;  /* original data @ ptr */
   int is_volatile;
};

struct patch_set
{
   const char *name;
   int initialized;
   int installed;
   struct patch_chunk chunk[PATCHER_CHUNKS_MAX];
   int failed;       /* set if patcher_installed() failed to initialize; reset on patcher_remove() */
   int has_volatile;
   /* these don't really belong here... but oh well. */
   int ini_auto;     /* automatically enable patch; only used by the ini parser */
   int ini_hotkey;   /* hotkey for this patch; only used by the ini parser */
};

char *dumb_hex_dump(void *data, uint32_t len);
int patcher_install(struct patch_set *set);
int patcher_remove(struct patch_set *set);
int is_patch_installed(struct patch_set *patch);
void patcher_free(struct patch_set *set);
