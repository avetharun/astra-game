#ifndef AST_LUA_HPP
#define AST_LUA_HPP


#include "LUA_INCLUDE.h"

#include "input.h"
#include "engine.hpp"

#include "utils.hpp"
struct lua_context {
	int reference;
	static lua_State* state;
};
lua_State* lua_context::state = nullptr;
lua_State* _lstate;
void initLUA(lua_State* state_) {
	_lstate = state_;
	
}

struct Time {
	float TimeSinceStart = 0;
	static double DeltaTime;
	static double DeltaTimeUnscaled;
	static double fps;
	static double DeltaTimeScale;
	struct Timer {
	private:
		std::chrono::steady_clock::time_point _start;
		std::chrono::steady_clock::time_point _end;
	public:
		Timer() {}
		double elapsed;
		void Start() {
			_start = std::chrono::high_resolution_clock::now();
		}
		void End() {
			_end = std::chrono::high_resolution_clock::now();
			elapsed = (double)(_end.time_since_epoch().count() - _start.time_since_epoch().count());
		}
	};
};
double Time::DeltaTimeScale = 1.0;
double Time::DeltaTimeUnscaled = 0.0;
double Time::DeltaTime = 0.0;
double Time::fps = 0.0f;
struct luafunc {
	luabridge::LuaRef __func_impl = 0;
	luafunc() {}
	luabridge::LuaRef operator ()(luabridge::detail::ArgList<luabridge::LuaRef, 0> _args) {
		if (__func_impl.isFunction()) { __func_impl(_args); }
	}
	luafunc operator =(luabridge::LuaRef _func_impl_flua) { __func_impl = _func_impl_flua; }
};
struct LuaComponentImpl { 


};


#define __asl_define_inputmodu(name, key) .addConstant(#name, key)
void assignInputData(luabridge::Namespace globalNS) {
	globalNS.beginNamespace("input")
		__asl_define_inputmodu(K_a, Input::K_A)
		__asl_define_inputmodu(K_b, Input::K_B)
		__asl_define_inputmodu(K_c, Input::K_C)
		__asl_define_inputmodu(K_d, Input::K_D)
		__asl_define_inputmodu(K_e, Input::K_E)
		__asl_define_inputmodu(K_f, Input::K_F)
		__asl_define_inputmodu(K_g, Input::K_G)
		__asl_define_inputmodu(K_h, Input::K_H)
		__asl_define_inputmodu(K_i, Input::K_I)
		__asl_define_inputmodu(K_j, Input::K_J)
		__asl_define_inputmodu(K_k, Input::K_K)
		__asl_define_inputmodu(K_l, Input::K_L)
		__asl_define_inputmodu(K_m, Input::K_M)
		__asl_define_inputmodu(K_n, Input::K_N)
		__asl_define_inputmodu(K_o, Input::K_O)
		__asl_define_inputmodu(K_p, Input::K_P)
		__asl_define_inputmodu(K_q, Input::K_Q)
		__asl_define_inputmodu(K_r, Input::K_R)
		__asl_define_inputmodu(K_s, Input::K_S)
		__asl_define_inputmodu(K_t, Input::K_T)
		__asl_define_inputmodu(K_u, Input::K_U)
		__asl_define_inputmodu(K_v, Input::K_V)
		__asl_define_inputmodu(K_w, Input::K_W)
		__asl_define_inputmodu(K_x, Input::K_X)
		__asl_define_inputmodu(K_y, Input::K_Y)
		__asl_define_inputmodu(K_z, Input::K_Z)

		__asl_define_inputmodu(K_A, Input::K_A)
		__asl_define_inputmodu(K_B, Input::K_B)
		__asl_define_inputmodu(K_C, Input::K_C)
		__asl_define_inputmodu(K_D, Input::K_D)
		__asl_define_inputmodu(K_E, Input::K_E)
		__asl_define_inputmodu(K_F, Input::K_F)
		__asl_define_inputmodu(K_G, Input::K_G)
		__asl_define_inputmodu(K_H, Input::K_H)
		__asl_define_inputmodu(K_I, Input::K_I)
		__asl_define_inputmodu(K_J, Input::K_J)
		__asl_define_inputmodu(K_K, Input::K_K)
		__asl_define_inputmodu(K_L, Input::K_L)
		__asl_define_inputmodu(K_M, Input::K_M)
		__asl_define_inputmodu(K_N, Input::K_N)
		__asl_define_inputmodu(K_O, Input::K_O)
		__asl_define_inputmodu(K_P, Input::K_P)
		__asl_define_inputmodu(K_Q, Input::K_Q)
		__asl_define_inputmodu(K_R, Input::K_R)
		__asl_define_inputmodu(K_S, Input::K_S)
		__asl_define_inputmodu(K_T, Input::K_T)
		__asl_define_inputmodu(K_U, Input::K_U)
		__asl_define_inputmodu(K_V, Input::K_V)
		__asl_define_inputmodu(K_W, Input::K_W)
		__asl_define_inputmodu(K_X, Input::K_X)
		__asl_define_inputmodu(K_Y, Input::K_Y)
		__asl_define_inputmodu(K_Z, Input::K_Z)

		__asl_define_inputmodu(K_1, Input::K_1)
		__asl_define_inputmodu(K_2, Input::K_2)
		__asl_define_inputmodu(K_3, Input::K_3)
		__asl_define_inputmodu(K_4, Input::K_4)
		__asl_define_inputmodu(K_5, Input::K_5)
		__asl_define_inputmodu(K_6, Input::K_6)
		__asl_define_inputmodu(K_7, Input::K_7)
		__asl_define_inputmodu(K_8, Input::K_8)
		__asl_define_inputmodu(K_9, Input::K_9)
		__asl_define_inputmodu(K_0, Input::K_0)

		__asl_define_inputmodu(K_return,		Input::K_return)
		__asl_define_inputmodu(K_escape,		Input::K_escape)
		__asl_define_inputmodu(K_backspace,		Input::K_backspace)
		__asl_define_inputmodu(K_tab,			Input::K_tab)
		__asl_define_inputmodu(K_space,			Input::K_space)
		__asl_define_inputmodu(K_minus,			Input::K_minus)
		__asl_define_inputmodu(K_equals,		Input::K_equals)
		__asl_define_inputmodu(K_leftbracket,	Input::K_leftbracket)
		__asl_define_inputmodu(K_rightbracket,	Input::K_rightbracket)
		__asl_define_inputmodu(K_backslash,		Input::K_backslash)
		__asl_define_inputmodu(K_nonushash,		Input::K_nonushash)
		__asl_define_inputmodu(K_semicolon,		Input::K_semicolon)
		__asl_define_inputmodu(K_apostrophe,	Input::K_apostrophe)
		__asl_define_inputmodu(K_grave,			Input::K_grave)
		__asl_define_inputmodu(K_tild,			Input::K_tild)
		__asl_define_inputmodu(K_comma,			Input::K_comma)
		__asl_define_inputmodu(K_period,		Input::K_period)
		__asl_define_inputmodu(K_slash,			Input::K_slash)
		__asl_define_inputmodu(K_capslock,		Input::K_capslock)
		__asl_define_inputmodu(K_F1,	 Input::K_F1)
		__asl_define_inputmodu(K_F2,	 Input::K_F2)
		__asl_define_inputmodu(K_F3,	 Input::K_F3)
		__asl_define_inputmodu(K_F4,	 Input::K_F4)
		__asl_define_inputmodu(K_F5,	 Input::K_F5)
		__asl_define_inputmodu(K_F6,	 Input::K_F6)
		__asl_define_inputmodu(K_F7,	 Input::K_F7)
		__asl_define_inputmodu(K_F8,	 Input::K_F8)
		__asl_define_inputmodu(K_F9,	 Input::K_F9)
		__asl_define_inputmodu(K_F10,	 Input::K_F10)
		__asl_define_inputmodu(K_F11,	 Input::K_F11)
		__asl_define_inputmodu(K_F12,	 Input::K_F12)
		__asl_define_inputmodu(K_printscreen,		Input::K_printscreen)
		__asl_define_inputmodu(K_scrolllock,		Input::K_scrolllock)
		__asl_define_inputmodu(K_pause,				Input::K_pause)
		__asl_define_inputmodu(K_insert,			Input::K_insert)
		__asl_define_inputmodu(K_home,				Input::K_home)
		__asl_define_inputmodu(K_pageup,			Input::K_pageup)
		__asl_define_inputmodu(K_delete,			Input::K_delete)
		__asl_define_inputmodu(K_end,				Input::K_end)
		__asl_define_inputmodu(K_pagedown,			Input::K_pagedown)
		__asl_define_inputmodu(K_right,				Input::K_right)
		__asl_define_inputmodu(K_left,				Input::K_left)
		__asl_define_inputmodu(K_down,				Input::K_down)
		__asl_define_inputmodu(K_up,				Input::K_up)
		__asl_define_inputmodu(K_lctrl,				Input::K_lctrl)
		__asl_define_inputmodu(K_lshift,			Input::K_lshift)
		__asl_define_inputmodu(K_lalt,				Input::K_lalt)
		__asl_define_inputmodu(K_lgui,				Input::K_lgui)
		__asl_define_inputmodu(K_rctrl,				Input::K_rctrl)
		__asl_define_inputmodu(K_rshift,			Input::K_rshift)
		__asl_define_inputmodu(K_ralt,				Input::K_ralt)
		__asl_define_inputmodu(K_rgui,				Input::K_rgui)
		__asl_define_inputmodu(K_unknown,			Input::K_UNKNOWN)
	.endNamespace();
}

#endif