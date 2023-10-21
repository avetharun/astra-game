#ifndef cwlib_cwl_error_handler_hpp
#define cwlib_cwl_error_handler_hpp

#include "_cwincludes.h"
#include <cstdarg>
#include "../utils.hpp"

struct cwError {
	// Note: CW_NONE does NOT mean it won't send! Use CW_SILENT for that!!
	enum cwWarnings : uint32_t {
		CW_NONE =		 B32(00000000, 00000000, 00000000, 00000000),
		CW_ERROR =		 B32(00000000, 00000000, 00000000, 00000100),
		CW_WARN =		 B32(00000000, 00000000, 00000000, 00001000),
		CW_MESSAGE =	 B32(00000000, 00000000, 00000000, 00010000),
		CW_VERBOSE =	 B32(00000000, 00000000, 00000000, 00100000),
		CW_SILENT =		 B32(00000000, 00000000, 00000000, 01000000),
		CW_DEBUG =		 B32(00000000, 00000000, 00000000, 10000000),       
		CW_RETURN__ =	 B32(10000000, 00000000, 00000000, 00000000)
	};
	// warning ID to human readable
	static const char* wtoh(uint32_t w) {
		switch (w) {
		default: return "UNKNOWN";
		case CW_NONE: return "";
		case CW_ERROR: return "ERROR";
		case CW_WARN: return "WARN";
		case CW_MESSAGE: return "Log";
		case CW_VERBOSE: return "V";
		case CW_SILENT: return "SILENT";
		case CW_DEBUG: return "DBG";
		}
	}
	static inline alib_stack<cwWarnings> WarningTypeStack = {};
	static inline cwWarnings warningState{};
	static inline std::string errorStr = "";
	static inline bool debug_enabled = true;
	static const char* geterror() {
		return errorStr.c_str();
	}
//	   ERR_STATE != CW_NONE :
//      -> ERR_STATE|ERR_MSG
//	   ERR_STATE == CW_NONE :
//		-> ERR_MSG
	static inline void_1pc_1i32_f onError = [](const char* errv, uint32_t errs) { 
		const char* __wtoh = ((errs == CW_NONE) ? "" : wtoh(errs));
		const char* __sep = ((errs == CW_NONE) ? "" : "|");
		printf("%s%s%s", __wtoh, __sep, errv); 
		
	};

	// Return the current error state, or change if argument 0 is not CW_NONE
	static uint32_t sstate(cwWarnings state = CW_RETURN__) {
		if (state == CW_RETURN__) { return warningState; }
		warningState = state;
		return state;
	}
	// Return the current or topmost error state
	static uint32_t gstate() {
		return WarningTypeStack.size() == 0 ? warningState : WarningTypeStack.top();
	}
	static void pstate(cwWarnings state) {
		WarningTypeStack.push_back(state);
	}
	static void push(cwWarnings state) {
		pstate(state);
	}
	
	static void postate() {
		WarningTypeStack.pop_back();
	}
	static void pop() {
		postate();
	}
	static void serror(const char* err) {
		// return if debugging isn't enabled, and are sending a debug message.
		if (!debug_enabled && (warningState == CW_DEBUG)) {
			return;
		}
		errorStr = (char*)err;
		onError(err, sstate());
	}
	static void serrof(const char* fmt, va_list args) {
		// return if debugging isn't enabled, and are sending a debug message.
		if (!debug_enabled && (warningState == CW_DEBUG)) {
			return;
		}
		errorStr = alib_strfmtv(fmt, args);
		onError(errorStr.c_str(), gstate());
	}
	static void serrof(const char* fmt, ...) {
		// return if debugging isn't enabled, and are sending a debug message.
		if (!debug_enabled && (warningState == CW_DEBUG)) {
			return;
		}
		errorStr.clear();
		va_list args;
		va_start(args, fmt);
		errorStr.append(alib_strfmtv(fmt, args));
		va_end(args);
		onError(errorStr.c_str(), gstate());
	}
};


#endif