#pragma once
#ifndef AST_LUA_HPP
#define AST_LUA_HPP

#include <luaaa.hpp>

lua_State* _lstate;
void initLUA(lua_State* state_) {
	_lstate = state_;
}

#endif