#ifndef UTILS_HPP
#define UTILS_HPP
#define noop __noop

#pragma once
#define bitset(d,nbit)   ((d) |=  (1<<(nbit)))
#define bitunset(d,nbit) ((d) &= ~(1<<(nbit)))
#define bitflip(d,nbit)  ((d) ^=  (1<<(nbit)))
#define getbit(d,nbit) ((d) &   (1<<(nbit)))
#define setbit(d, nbit, val) \
    (val) ? bitset(d,nbit) : bitunset(d,nbit)
#define setbitbulk(d,start,end,val) \
	for (int i = start; i < end; i++) { \
		setbit(d, i, val); \
	}
#include <string_view>
#include <wtypes.h>
#ifndef SDL_h_
#include <sdl/SDL.h>
#endif
static bool endsWith(std::string str, std::string suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

static bool startsWith(std::string str, std::string prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

#define CONSOLE ::GetConsoleWindow()

void HideConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

bool IsConsoleVisible()
{
	return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}
void ForceHang() { while (1) {} } // Force an infinite loop
void ForceExit(bool showDebugConsoleOnExit = true) { // Force an exit that can show debug window by default.
	if (showDebugConsoleOnExit) {
		ShowConsole();
		std::cout << "\n Game ran into an error, the debug window will be shown:\nPress the [X] or ";
		system("pause");
	}
	exit(-1);
} // Force game to exit

#include <functional>
struct _RUNNABLE_ONSTARTExec {
	_RUNNABLE_ONSTARTExec(std::function<void()> initFunc) {
		initFunc();
#if defined(DEBUG_COUT)
		std::cout << "\nInitialized initFunc with struct size: " << sizeof(this) << " and size of function: " << sizeof(initFunc) << '\n';
#endif
		ZeroMemory(&initFunc, 0);
		ZeroMemory(this, 0);
	}
};
#include "e_keybindings.h" // minimal keybinding file (see SDL_SCANCODE***.h(pp?) for source)
#include "preprocessors.h" // preprocessor magic file (:
#define $f [&]()
#define $concat(a, b, c) $concat_internal(a, b, c)
#define $nn $concat(unnamed_, __LINE__, __COUNTER__)
#define $exec _RUNNABLE_ONSTARTExec $nn					/* Execute what's in brackets. */
#define $win Window::WindowInstance						/* Get window instance */
#define uint unsigned int
#define $terminate(b) ForceExit(b)
bool cmpSDL_Rect(SDL_Rect one, SDL_Rect two) {
	if (one.x == two.x &&
		one.y == two.y &&
		one.w == two.w &&
		one.h == two.h
		) 
	{
		return true;
	}
	return false;
}

typedef __int16 int2b;
typedef __int8  int1b;
#pragma pack(push, 1)
struct __int24 {
	unsigned int data : 24;
	__int24(int v) { data = v; };
	operator int() const
	{return data;}
};
#pragma pack(pop)
typedef __int24 int3b;
const __int24 INT24_MAX = 8388607;
typedef __int32 int4b;
typedef __int64 int8b;
double inline __declspec (naked) __fastcall ffsqrt(double n)
{
	_asm fld qword ptr[esp + 4]
	_asm fsqrt
	_asm ret 8
}




#endif