#pragma once

#include "_cwincludes.h"
/*
Asset file structure
*.cwa : {
	// Version of cwlib that the asset supports
	// Set to 0 to attempt to forcefully load
	assetVersion : 4
	{
	  0-2: min
	  3-4: max
	}
	// long with ID of asset
	assetID : 4
	// ID of the main functions declared either in-code or via cws script.
	// If declared via cws script, first 8 chars of function name are what this should be.
	assetUpdateF : 8
	assetStartF : 8
	// Bitmask used as flags via the cws script or in-code
	assetFlags : 4
	{
	  0: enabled/disabled
	}
}


*/

