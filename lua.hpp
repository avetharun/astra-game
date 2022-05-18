#ifndef AST_LUA_HPP
#define AST_LUA_HPP


#include "LUA_INCLUDE.h"

#include "e_keybindings.h"
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


#define __asl_define_inputmodu(name, key) InputModu.addConstant(#name, key)
void assignInputData(auto InputModu) {
	__asl_define_inputmodu(K_a, Input::K_A);
	__asl_define_inputmodu(K_b, Input::K_B);
	__asl_define_inputmodu(K_c, Input::K_C);
	__asl_define_inputmodu(K_d, Input::K_D);
	__asl_define_inputmodu(K_e, Input::K_E);
	__asl_define_inputmodu(K_f, Input::K_F);
	__asl_define_inputmodu(K_g, Input::K_G);
	__asl_define_inputmodu(K_h, Input::K_H);
	__asl_define_inputmodu(K_i, Input::K_I);
	__asl_define_inputmodu(K_j, Input::K_J);
	__asl_define_inputmodu(K_k, Input::K_K);
	__asl_define_inputmodu(K_l, Input::K_L);
	__asl_define_inputmodu(K_m, Input::K_M);
	__asl_define_inputmodu(K_n, Input::K_N);
	__asl_define_inputmodu(K_o, Input::K_O);
	__asl_define_inputmodu(K_p, Input::K_P);
	__asl_define_inputmodu(K_q, Input::K_Q);
	__asl_define_inputmodu(K_r, Input::K_R);
	__asl_define_inputmodu(K_s, Input::K_S);
	__asl_define_inputmodu(K_t, Input::K_T);
	__asl_define_inputmodu(K_u, Input::K_U);
	__asl_define_inputmodu(K_v, Input::K_V);
	__asl_define_inputmodu(K_w, Input::K_W);
	__asl_define_inputmodu(K_x, Input::K_X);
	__asl_define_inputmodu(K_y, Input::K_Y);
	__asl_define_inputmodu(K_z, Input::K_Z);

	__asl_define_inputmodu(K_A, Input::K_A);
	__asl_define_inputmodu(K_B, Input::K_B);
	__asl_define_inputmodu(K_C, Input::K_C);
	__asl_define_inputmodu(K_D, Input::K_D);
	__asl_define_inputmodu(K_E, Input::K_E);
	__asl_define_inputmodu(K_F, Input::K_F);
	__asl_define_inputmodu(K_G, Input::K_G);
	__asl_define_inputmodu(K_H, Input::K_H);
	__asl_define_inputmodu(K_I, Input::K_I);
	__asl_define_inputmodu(K_J, Input::K_J);
	__asl_define_inputmodu(K_K, Input::K_K);
	__asl_define_inputmodu(K_L, Input::K_L);
	__asl_define_inputmodu(K_M, Input::K_M);
	__asl_define_inputmodu(K_N, Input::K_N);
	__asl_define_inputmodu(K_O, Input::K_O);
	__asl_define_inputmodu(K_P, Input::K_P);
	__asl_define_inputmodu(K_Q, Input::K_Q);
	__asl_define_inputmodu(K_R, Input::K_R);
	__asl_define_inputmodu(K_S, Input::K_S);
	__asl_define_inputmodu(K_T, Input::K_T);
	__asl_define_inputmodu(K_U, Input::K_U);
	__asl_define_inputmodu(K_V, Input::K_V);
	__asl_define_inputmodu(K_W, Input::K_W);
	__asl_define_inputmodu(K_X, Input::K_X);
	__asl_define_inputmodu(K_Y, Input::K_Y);
	__asl_define_inputmodu(K_Z, Input::K_Z);

	__asl_define_inputmodu(K_1, Input::K_1);
	__asl_define_inputmodu(K_2, Input::K_2);
	__asl_define_inputmodu(K_3, Input::K_3);
	__asl_define_inputmodu(K_4, Input::K_4);
	__asl_define_inputmodu(K_5, Input::K_5);
	__asl_define_inputmodu(K_6, Input::K_6);
	__asl_define_inputmodu(K_7, Input::K_7);
	__asl_define_inputmodu(K_8, Input::K_8);
	__asl_define_inputmodu(K_9, Input::K_9);
	__asl_define_inputmodu(K_0, Input::K_0);

	__asl_define_inputmodu(K_RETURN,		Input::K_RETURN);
	__asl_define_inputmodu(K_ESCAPE,		Input::K_ESCAPE);
	__asl_define_inputmodu(K_BACKSPACE,		Input::K_BACKSPACE);
	__asl_define_inputmodu(K_TAB,			Input::K_TAB);
	__asl_define_inputmodu(K_SPACE,			Input::K_SPACE);
	__asl_define_inputmodu(K_MINUS,			Input::K_MINUS);
	__asl_define_inputmodu(K_EQUALS,		Input::K_EQUALS);
	__asl_define_inputmodu(K_LEFTBRACKET,	Input::K_LEFTBRACKET);
	__asl_define_inputmodu(K_RIGHTBRACKET,	Input::K_RIGHTBRACKET);
	__asl_define_inputmodu(K_BACKSLASH,		Input::K_BACKSLASH);
	__asl_define_inputmodu(K_NONUSHASH,		Input::K_NONUSHASH);
	__asl_define_inputmodu(K_SEMICOLON,		Input::K_SEMICOLON);
	__asl_define_inputmodu(K_APOSTROPHE,	Input::K_APOSTROPHE);
	__asl_define_inputmodu(K_GRAVE,			Input::K_GRAVE);
	__asl_define_inputmodu(K_TILD,			Input::K_TILD);
	__asl_define_inputmodu(K_COMMA,			Input::K_COMMA);
	__asl_define_inputmodu(K_PERIOD,		Input::K_PERIOD);
	__asl_define_inputmodu(K_SLASH,			Input::K_SLASH);
	__asl_define_inputmodu(K_CAPSLOCK,		Input::K_CAPSLOCK);
	__asl_define_inputmodu(K_F1,	 Input::K_F1);
	__asl_define_inputmodu(K_F2,	 Input::K_F2);
	__asl_define_inputmodu(K_F3,	 Input::K_F3);
	__asl_define_inputmodu(K_F4,	 Input::K_F4);
	__asl_define_inputmodu(K_F5,	 Input::K_F5);
	__asl_define_inputmodu(K_F6,	 Input::K_F6);
	__asl_define_inputmodu(K_F7,	 Input::K_F7);
	__asl_define_inputmodu(K_F8,	 Input::K_F8);
	__asl_define_inputmodu(K_F9,	 Input::K_F9);
	__asl_define_inputmodu(K_F10,	 Input::K_F10);
	__asl_define_inputmodu(K_F11,	 Input::K_F11);
	__asl_define_inputmodu(K_F12,	 Input::K_F12);
	__asl_define_inputmodu(K_PRINTSCREEN,		Input::K_PRINTSCREEN);
	__asl_define_inputmodu(K_SCROLLLOCK,		Input::K_SCROLLLOCK);
	__asl_define_inputmodu(K_PAUSE,				Input::K_PAUSE);
	__asl_define_inputmodu(K_INSERT,			Input::K_INSERT);
	__asl_define_inputmodu(K_HOME,				Input::K_HOME);
	__asl_define_inputmodu(K_PAGEUP,			Input::K_PAGEUP);
	__asl_define_inputmodu(K_DELETE,			Input::K_DELETE);
	__asl_define_inputmodu(K_END,				Input::K_END);
	__asl_define_inputmodu(K_PAGEDOWN,			Input::K_PAGEDOWN);
	__asl_define_inputmodu(K_RIGHT,				Input::K_RIGHT);
	__asl_define_inputmodu(K_LEFT,				Input::K_LEFT);
	__asl_define_inputmodu(K_DOWN,				Input::K_DOWN);
	__asl_define_inputmodu(K_UP,				Input::K_UP);
	__asl_define_inputmodu(K_LCTRL,				Input::K_LCTRL);
	__asl_define_inputmodu(K_LSHIFT,			Input::K_LSHIFT);
	__asl_define_inputmodu(K_LALT,				Input::K_LALT);
	__asl_define_inputmodu(K_LGUI,				Input::K_LGUI);
	__asl_define_inputmodu(K_RCTRL,				Input::K_RCTRL);
	__asl_define_inputmodu(K_RSHIFT,			Input::K_RSHIFT);
	__asl_define_inputmodu(K_RALT,				Input::K_RALT);
	__asl_define_inputmodu(K_RGUI,				Input::K_RGUI);
	__asl_define_inputmodu(K_UNKNOWN,			Input::K_UNKNOWN);

}

#endif