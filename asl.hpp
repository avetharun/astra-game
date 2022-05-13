#ifndef AST_ASL_HPP
#define AST_ASL_HPP
#define ALIB_FORCE_INLINE_RUNNERS
#include "utils.hpp"
#include "LUA_INCLUDE.h"


#include <random>
lua_State* state = luaL_newstate(); // create and init lua
#include "engine.hpp"
#include "renderer.hpp"
#include "Vectors.hpp"
#include "Audio.hpp"
#include "physics.hpp"
#include "cwlib/cwlib.hpp"
#include "lua.hpp"
#include "imgui/imgui.h"

void setGlobalLuaF(lua_State* state, const char* _name, lua_CFunction _f) {
	lua_pushcfunction(state, _f);
	lua_setglobal(state, _name);
}
void setGlobalLuaNum(lua_State* state, const char* _name, lua_Number num) {
	lua_pushnumber(state, num);
	lua_setglobal(state, _name);
}
void deleteGlobalLua(lua_State* state, const char* name) {
	lua_pushnil(state);
	lua_setglobal(state, name);
}

Sprite* playersprite;
Window* windowInstance;
SDL_Renderer* __sdlRenderer;
void guiRenderTaskImpl();
void initDataLua(Sprite* _s, SDL_Renderer* renderOBJ) {
	playersprite = _s;
	__sdlRenderer = renderOBJ;
	windowInstance = Win;
}
struct lu_time {
	static long double* DeltaTime;
};
long double* lu_time::DeltaTime = nullptr;
void lu_initTime(long double* deltatime) {
	lu_time::DeltaTime = deltatime;
}
void lu_cw_loadScene(std::string sceneName) {
	if (!alib_endswith(sceneName.c_str(), ".cwl")) {
		sceneName.append(".cwl");
	}
	if (alib_file_exists(sceneName.c_str())) {
		if (Win->scene != nullptr) {
			Win->scene->Discard();
		}
		Window::WindowInstance->scene = new cwLayout(sceneName.c_str());
		return;
	} 
	cwError::sstate(cwError::CW_ERROR);
	cwError::serrof("Scene %s doesn't exist!", sceneName.c_str());
}
void lu_cw_unloadScene() {
	if (Win->scene != nullptr) {
		Win->scene->Discard();
		Win->scene = nullptr;
	}
}

void lu_cw_reloadScene() {
	if (SUCCEEDED(Win->scene)) {
		std::string _name = Win->scene->name;
		Win->scene->Discard();
		Window::WindowInstance->scene = new cwLayout(_name.c_str());
		return;
	}
}
struct lu_col {
	std::array<char, 4> cols;
	char getR() { return cols[0]; }
	char getG() { return cols[1]; }
	char getB() { return cols[2]; }
	char getA() { return cols[3]; }

	// Needs to fit AT LEAST 32 bits
	void setColorL(int32_t c) {
		cols[0] = alib_get_byte(&c, 0);
		cols[1] = alib_get_byte(&c, 1);
		cols[2] = alib_get_byte(&c, 2);
		cols[3] = alib_get_byte(&c, 3);
	}
	void setColor(lu_col _c) {
		cols = _c.cols;
	}
};
void lu_logerr(std::string str) {
	cwError::sstate(cwError::CW_ERROR);
	cwError::serror(str.c_str());
};
void lu_logsil(std::string str) {
	cwError::sstate(cwError::CW_NONE);
	cwError::serror(str.c_str());
};
void lu_logdeb(std::string str) {
	cwError::sstate(cwError::CW_DEBUG);
	cwError::serror(str.c_str());
};
void lu_logmsg(std::string str) {
	cwError::sstate(cwError::CW_MESSAGE);
	cwError::serror(str.c_str());
};
void lu_logwrn(std::string str) {
	cwError::sstate(cwError::CW_WARN);
	cwError::serror(str.c_str());
};
void lu_logvrb(std::string str) {
	cwError::sstate(cwError::CW_VERBOSE);
	cwError::serror(str.c_str());
};
#define alu_global luabridge::getGlobalNamespace(state)
void do_n(int amt, std::function<void()> __run) {
	for (int i = 0; i < amt; i++) {
		__run();
	}
}

alib_inline_run _nn([&]() {

	initLUA(state);

	luaL_openlibs(state);
	luaopen_base(state);
	deleteGlobalLua(state, "utf8");

	setGlobalLuaNum(state, "c_empty", COL_EMPTY);
	setGlobalLuaNum(state, "c_player", COL_PLAYER);
	setGlobalLuaNum(state, "c_entity", COL_ENT);
	setGlobalLuaNum(state, "c_object", COL_OBJ);
	setGlobalLuaNum(state, "c_trigger", COL_TRG);
	setGlobalLuaNum(state, "c_solid", COL_SOLID);

	alu_global.beginNamespace("cw")
		.addFunction("load", &lu_cw_loadScene)
		.addFunction("unload", &lu_cw_unloadScene)
		.addFunction("reload", &lu_cw_reloadScene)
	.endNamespace();
	alu_global.beginClass<Vector2>("Vector2")
		.addStaticFunction("new", &Vector2::lu_new) // ctor
		.addStaticFunction("distance", &Vector2::distance)
		.addData("x", &Vector2::x)
		.addData("y", &Vector2::y)
	.endClass();
	alu_global.beginNamespace("console")
		.addFunction("log", &lu_logmsg)
		.addFunction("error", &lu_logmsg)
		.addFunction("warn", &lu_logerr)
		.addFunction("debug", &lu_logdeb)
	.endNamespace();
	alu_global
		.addFunction("print", &lu_logsil) // Override print, and have it print to the debug console.
		.addFunction("sleep", std::function<void(double)>([](int ms) -> void {
			alib_sleep_millis(ms);
		}))
	;
	alu_global.beginClass<MeshLine>("Line")
		.addStaticFunction("new", &MeshLine::lu_new)
		.addData("start", &MeshLine::start)
		.addData("end", &MeshLine::end)
		.addData("layer", &MeshLine::layer)
		.addData("id", &MeshLine::coll_id)
		.addFunction("enable_collider", &MeshLine::setFreestanding)
		.addFunction("disable_collider", &MeshLine::eraseFreestanding)
	.endClass();
	alu_global.beginClass<VectorRect>("Rect")
		.addStaticFunction("new", &VectorRect::lu_new)
		.addData("x", &VectorRect::x)
		.addData("y", &VectorRect::y)
		.addData("w", &VectorRect::w)
		.addData("h", &VectorRect::h)
		.addStaticFunction("intersects", &VectorRect::lu_intersects)
	.endClass();
	alu_global.beginClass<RectCollider2d>("RectCollider")
		.addStaticFunction("new", &RectCollider2d::lu_new_fromi)
		.addStaticFunction("from_rect", &RectCollider2d::lu_new_fromr)
		.addData("layer", &RectCollider2d::layer)
		.addData("id", &RectCollider2d::coll_id)
		.addProperty("x",
			std::function <int(const RectCollider2d*)>([](const RectCollider2d* r) {return r->rect->x; }),
			std::function <void(RectCollider2d*, int)>([](RectCollider2d* r, int v) {r->rect->x = v; })
			)
		.addProperty("y",
			std::function <int(const RectCollider2d*)>([](const RectCollider2d* r) {return r->rect->y; }),
			std::function <void(RectCollider2d*, int)>([](RectCollider2d* r, int v) {r->rect->y = v; })
		)
		.addProperty("w",
			std::function <int(const RectCollider2d*)>([](const RectCollider2d* r) {return r->rect->w; }),
			std::function <void(RectCollider2d*, int)>([](RectCollider2d* r, int v) {r->rect->w = v; })
		)
		.addProperty("h",
			std::function <int(const RectCollider2d*)>([](const RectCollider2d* r) {return r->rect->h; }),
			std::function <void(RectCollider2d*, int)>([](RectCollider2d* r, int v) {r->rect->h = v; })
		)
		.addFunction("onIntersect", &RectCollider2d::setCallback)
		.addStaticFunction("dump_debug", &RectCollider2d::dump_debug)
	.endClass();
	// I despise programming like this
	alu_global.beginNamespace("Raycast")
		.addFunction("sline", &Collider::Raycast::Test) // Raycast and check for specific type of collider
	.endNamespace();


	alu_global.beginNamespace("math")
		.addFunction("lerp", std::function <Vector2(Vector2, Vector2, float percent)>([](Vector2 start, Vector2 end, float percent) -> Vector2 {
			return (start + (end - start) * percent);
		}))
	.endNamespace();
	alu_global.beginNamespace("Time")
		.addFunction("deltatime", std::function<double()>([]() -> double {return (double) * lu_time::DeltaTime; }))
	.endNamespace();
	alu_global.beginClass<AudioWAV>("Audio")
		.addFunction("name", &AudioWAV::name)
		.addStaticFunction("new", &AudioWAV::lu_new)
		.addFunction("end", &AudioWAV::operator~)
		.addFunction("play", &AudioWAV::Play)
		.addFunction("stop", &AudioWAV::Stop)
		.addFunction("set_volume", &AudioWAV::SetVolume)
		.addFunction("get_volume", &AudioWAV::GetVolume)
		.addFunction("get_error", &AudioWAV::GetError)
		//.addProperty("volume", &AudioWAV::GetVolume, &AudioWAV::SetVolume)
	.endClass();
	int status = luaL_dostring(state, R"(
		io.stderr:setvbuf "no"
		dofile("debug.lua")
	)");//collectgarbage("stop")
	if (status) {
		// since the error message includes the script which we're trying
		// to parse, the result has unbounded length; so use another
		// ostringstream to hold the error message
		std::ostringstream errmsg;
		int parseError1_pos = lua_gettop(state);
		std::cout << "Errors parsing the following script:" << std::endl;
		std::cout << "debug.lua" << std::endl << std::endl;
		std::cout << "Parser error when interpreting as an expression:" << std::endl;
		std::cout << lua_tostring(state, parseError1_pos) << std::endl << std::endl;
	}
});
void guiRenderTaskImpl() {
	




}



#endif