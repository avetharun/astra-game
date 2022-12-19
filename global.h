#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP
#include "imgui/imgui.h"

namespace CWLGlobals {
	// lifetime use, dont delete.
	char* SymbolFontData;
	ImFont* SymbolFont;
	ImFont* DefaultFont;
}
#endif