/// \file
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#include "GetTime.h"
#ifdef _COMPATIBILITY_1
#include "Compatibility1Includes.h" // Developers of a certain platform will know what to do here.
#elif defined(_WIN32)
#include <windows.h> // LARGE_INTEGER
#else
#include <sys/time.h>
#endif

unsigned int RakNet::GetTime( void )
{
#ifdef _WIN32
	static LARGE_INTEGER yo;
//	static LONGLONG counts;
#else
	static timeval tp, initialTime;
#endif
	
	static bool initialized = false;
	
	if ( initialized == false )
	{
#ifdef _WIN32
		QueryPerformanceFrequency( &yo );
		// The original code shifted right 10 bits
		//counts = yo.QuadPart >> 10;
		// It gives the wrong value since 2^10 is not 1000
	//	counts = yo.QuadPart;// / 1000;
#else
		gettimeofday( &initialTime, 0 );
#endif
		
		initialized = true;
	}
	
#ifdef _WIN32
	LARGE_INTEGER PerfVal;
	
	QueryPerformanceCounter( &PerfVal );
	
	return ( unsigned int ) ( PerfVal.QuadPart*1000 / yo.QuadPart );
	
#else
	
	gettimeofday( &tp, 0 );
	
	// Seconds to ms and microseconds to ms
	return ( tp.tv_sec - initialTime.tv_sec ) * 1000 + ( tp.tv_usec - initialTime.tv_usec ) / 1000;
	
#endif
}
