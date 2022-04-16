#pragma once

#include "_cwincludes.h"


struct cwError {
	// Note: CW_NONE does NOT mean it won't send! Use CW_SILENT for that!!
	enum cwWarnings : uint32_t {
		CW_NONE = B32(00000000, 00000000, 00000000, 00000000),
		CW_ERROR = B32(10000000, 00000000, 00000000, 00000000),
		CW_WARN = B32(01000000, 00000000, 00000000, 00000000),
		CW_MESSAGE = B32(00100000, 00000000, 00000000, 00000000),
		CW_VERBOSE = B32(00010000, 00000000, 00000000, 00000000),
		CW_SILENT = B32(00001000, 00000000, 00000000, 00000000),
		CW_DEBUG = B32(00000100, 00000000, 00000000, 00000000),
	};
	// warning ID to human readable
	static const char* wtoh(uint32_t w) {
		switch (w) {
		default: return "UNKNOWN";
		case CW_NONE: return "NONE";
		case CW_ERROR: return "ERROR";
		case CW_WARN: return "WARN";
		case CW_MESSAGE: return "MESSAGE";
		case CW_VERBOSE: return "VERBOSE";
		case CW_SILENT: return "SILENT";
		case CW_DEBUG: return "DEBUG";
		}
	}
	static inline cwWarnings warningState{};
	static inline const char* errorStr{};
	cwError() {
		alib_set_byte(const_cast<char*>(errorStr), '\0');
	}
	static const char* geterror() {
		return errorStr;
	}
	static inline void_1pc_1i_f onError = [](const char* errv, uint32_t errs) { printf("%s|%s", errv, wtoh(errs)); };

	// Return the current error state, or change if argument 0 is not CW_NONE
	static uint32_t sstate(cwWarnings state = CW_NONE) {
		if (state == CW_NONE) { return warningState; }
		warningState = state;
		return state;
	}
	static void serror(const char* err) {
		errorStr = (char*)err;
		onError(err, sstate());
	}
	static void serrof(const char* fmt, va_list args) {
		free(const_cast<char*>(errorStr));
		alib_va_arg_parse(const_cast<char*>(errorStr), fmt, args);
		onError(errorStr, sstate());
	}
	static void serrof(const char* fmt, ...) {
		free(const_cast<char*>(errorStr));
		va_list args;
		va_start(args, fmt);
		size_t bufsz = snprintf(NULL, 0, fmt, args);
		errorStr = (const char*)malloc(bufsz);
		vsprintf((char*)errorStr, fmt, args);
		va_end(args);
		onError(errorStr, sstate());
	}
};
