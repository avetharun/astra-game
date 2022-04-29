#pragma once
#ifndef cwlib_cwa_interpreter_hpp
#define cwlib_cwa_interpreter_hpp

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
	// Bitmask used as flags via the cws script or in-code
	assetFlags : 4
	{
	  0: enabled/disabled
	}
	// Length of ABT data
	assetABTLen : 4
	assetABT : N


}
*/
#include "../utils.hpp"
#include <functional>
#include "_cwincludes.h"

struct cwAsset {
	struct cwAssetVersion {
		uint16_t min;
		uint16_t max;
	};
	cwAssetVersion version;
	uint32_t assetID;
	uint64_t updateFID;
	uint64_t startFID;
	uint32_t flags;
};




#endif