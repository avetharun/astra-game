#pragma once

class WinConsoleClr {
	static const char* clrRed;
	static const char* clrYellow;
	static const char* clrGreen;
	static const char* clrCyan;
	static const char* clrReset;
};



const char* WinConsoleClr::clrRed = "\u001b[31m",          //Error
const char* WinConsoleClr::clrYellow = "\u001b[33m",       //Trying
const char* WinConsoleClr::clrGreen = "\u001b[32m",        //Success
const char* WinConsoleClr::clrCyan = "\u001b[36m",         //Files
const char* WinConsoleClr::clrReset = "\u001b[0m";         //Reset all colours
