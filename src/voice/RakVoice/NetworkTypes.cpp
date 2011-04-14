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

#include <main.h>

#include "NetworkTypes.h"

// Defaults to not in peer to peer mode for NetworkIDs.  This only sends the localSystemId portion in the BitStream class
// This is what you want for client/server, where the server assigns all NetworkIDs and it is unnecessary to transmit the full structure.
// For peer to peer, this will transmit the playerId of the system that created the object in addition to localSystemId.  This allows
// Any system to create unique ids locally.
// All systems must use the same value for this variable.
bool NetworkID::peerToPeerMode=false;

int operator==( const PlayerID& left, const PlayerID& right )
{
	return left.binaryAddress == right.binaryAddress && left.port == right.port;
}

int operator!=( const PlayerID& left, const PlayerID& right )
{
	return left.binaryAddress != right.binaryAddress || left.port != right.port;
}

int operator>( const PlayerID& left, const PlayerID& right )
{
	return ( ( left.binaryAddress > right.binaryAddress ) || ( ( left.binaryAddress == right.binaryAddress ) && ( left.port > right.port ) ) );
}

int operator<( const PlayerID& left, const PlayerID& right )
{
	return ( ( left.binaryAddress < right.binaryAddress ) || ( ( left.binaryAddress == right.binaryAddress ) && ( left.port < right.port ) ) );
}

int operator==( const NetworkID& left, const NetworkID& right )
{
	if (NetworkID::peerToPeerMode)
		return left.playerId == right.playerId && left.localSystemId == right.localSystemId;
	else
		return left.localSystemId==right.localSystemId;
}

int operator!=( const NetworkID& left, const NetworkID& right )
{
	if (NetworkID::peerToPeerMode)
		return left.playerId != right.playerId || left.localSystemId != right.localSystemId;
	else
		return left.localSystemId!=right.localSystemId;
}

int operator>( const NetworkID& left, const NetworkID& right )
{
	if (NetworkID::peerToPeerMode)
		return ( ( left.playerId > right.playerId ) || ( ( left.playerId == right.playerId ) && ( left.localSystemId > right.localSystemId ) ) );
	else
		return left.localSystemId>right.localSystemId;
}

int operator<( const NetworkID& left, const NetworkID& right )
{
	if (NetworkID::peerToPeerMode)
		return ( ( left.playerId < right.playerId ) || ( ( left.playerId == right.playerId ) && ( left.localSystemId < right.localSystemId ) ) );
	else
		return left.localSystemId<right.localSystemId;
}
