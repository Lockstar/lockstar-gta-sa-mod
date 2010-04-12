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

GAME_SCRIPT_THREAD	*gst;
FARPROC				ProcessOneCommand = ( FARPROC ) 0x469EB0;
BYTE				ScriptBuf[255];
DWORD				*pdwParamVars[18];

int __declspec ( naked ) ExecuteScriptBuf ()
{
	__asm
	{
		mov eax, OFFSET ScriptBuf	// Move our script buffer base into eax.
		mov ecx, gst			// Move GAME_SCRIPT_THREAD structure into ecx.
		mov[ecx + 0x14], eax	// Move eax into the gst->dwScriptIP.
		call ProcessOneCommand	// Call the game's script opcode processor.
		mov ecx, gst			// Move game script thread into ecx again.
		mov eax, [ecx + 0xC5]	// Move the dwIfFlag into eax (return value).
		ret // return.
	}
}

int ScriptCommand ( const SCRIPT_COMMAND *pScriptCommand, ... )
{
	traceLastFunc( "ScriptCommand()" );

	va_list		ap;
	const char	*p = pScriptCommand->Params;	// Get parameter string.
	va_start( ap, pScriptCommand ); // Initialize varargs.
	memcpy( &ScriptBuf, &pScriptCommand->OpCode, 2 );	// Copy opcode to script buf.
	int		buf_pos = 2;	// Position in buffer after opcode.
	WORD	var_pos = 0;	// Init var_pos.

	// (aru) Reset all vars before proceeding (To fix some nasty 0x00465CB4 crashes)
	// May cause problems with functions that use local vars returned from previous calls
	for ( int i = 0; i < 18; i++ )
		gst->dwLocalVar[i] = 0;

	while ( *p )			// While we've not ran out of parameters...
	{
		switch ( *p )		// Switch current parameter.
		{
		case 'i':			// If integer...
			{
				int i = va_arg( ap, int );	// Grab an int off the stack.
				ScriptBuf[buf_pos] = 0x01;	// Variable data type = 0x01 (4b int).
				buf_pos++;;

				// Increment buffer position.
				memcpy( &ScriptBuf[buf_pos], &i, 4 );	// Insert the int.
				buf_pos += 4;	// Increment buffer by 4b.
				break;
			}

		case 'f':	// If float...
			{
				float	f = (float)va_arg( ap, double );	// Get float off the stack.
				ScriptBuf[buf_pos] = 0x06;	// Variable data type = 0x06 (float).
				buf_pos++;	// Increment buffer position.
				memcpy( &ScriptBuf[buf_pos], &f, 4 );	// Copy float into script buf.
				buf_pos += 4;	// Increment buffer by 4b.
				break;
			}

		case 'v':	// If variable...
			{
				DWORD	*v = va_arg( ap, DWORD * ); // Get the pointer to the passed variable.
				ScriptBuf[buf_pos] = 0x03;			// Variable data type = 0x03 (Local Var).
				buf_pos++;	// Increment buffer position.
				pdwParamVars[var_pos] = v;		// Save pointer to passed variable.
				gst->dwLocalVar[var_pos] = *v;	// Put value of passed variable into local.
				memcpy( &ScriptBuf[buf_pos], &var_pos, 2 ); // Copy the offset into the script.
				buf_pos += 2;	// Increment buffer by 2b.
				var_pos++;		// Increment the variable position.
				break;
			}

		default:
			{
				return 0;
			}
		}

		++p;		// Next parameter
	}

	va_end( ap );	// End varargs.

	// Execute script stub.
	int result = 0;
	result = ExecuteScriptBuf();

	if ( var_pos )	// if we've used a variable...
	{
		for ( int i = 0; i < var_pos; i++ )			// For every passed variable...
		{
			*pdwParamVars[i] = gst->dwLocalVar[i];	// Retrieve variable from local var.
		}
	}

	return result;
}

void InitScripting ()
{
	gst = new GAME_SCRIPT_THREAD;
	ZeroMemory( gst, sizeof(GAME_SCRIPT_THREAD) );
}
