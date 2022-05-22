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
struct lu_SDL_Window_impl {
	static inline SDL_Window* __win{};
	static inline SDL_Renderer* __rend{};
	std::string __name = "Unnamed Game";
	bool debug = true;
	bool debug_window_open = true;
	void Update() {
		SDL_SetWindowTitle(__win, __name.c_str());
		Window::WindowInstance->debug = this->debug;
		Window::WindowInstance->debug_window_open = this->debug_window_open;
	}
	//std::string __get_name() { return __name; }
	//void __set_name(std::string ___name) { this->__name = ___name; }
	static inline lu_SDL_Window_impl* __INSTANCE = nullptr;
};
Window* windowInstance;
SDL_Renderer* __sdlRenderer;
SDL_Window* __sdlWindow;
double* _lu_deltaTime;
double* _lu_deltaTimeUS;
double* _lu_deltaTimeSCALE;
double* _lu_deltaTimefps;
void guiRenderTaskImpl();
void initDataLua(SDL_Renderer* renderOBJ, SDL_Window* __winObj, double* _ludt, double* _ludtsc, double* _ludtus, double* _ludt_fps) {
	__sdlWindow = __winObj;
	__sdlRenderer = renderOBJ;
	windowInstance = Win;
	lu_SDL_Window_impl::__INSTANCE = new lu_SDL_Window_impl();
	lu_SDL_Window_impl::__win = __winObj;
	lu_SDL_Window_impl::__rend = renderOBJ;
	_lu_deltaTime = _ludt;
	_lu_deltaTimeSCALE = _ludtsc;
	_lu_deltaTimeUS = _ludtus;
	_lu_deltaTimefps = _ludt_fps;
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
std::map<std::string, const luabridge::LuaRef> __lu_preupdate__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_update__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_postupdate__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_start__funcs;
void lu_cw_add_preu(std::string __name, const luabridge::LuaRef __preufunc) {
	__lu_preupdate__funcs.insert({ __name, __preufunc});
}
void lu_cw_add_update(std::string __name, const luabridge::LuaRef __ufunc) {
	__lu_update__funcs.insert({__name, __ufunc});
}
void lu_cw_add_postu(std::string __name, const luabridge::LuaRef __postufunc) {
	__lu_postupdate__funcs.insert({ __name, __postufunc});
}
void lu_cw_add_start(std::string __name, const luabridge::LuaRef __sfunc) {
	__lu_start__funcs.insert({ __name, __sfunc});
}


void lu_cw_del_preu(std::string __name) {
	__lu_preupdate__funcs.erase(__name);
}
void lu_cw_del_update(std::string __name) {
	__lu_update__funcs.erase(__name);
}
void lu_cw_del_postu(std::string __name) {
	__lu_postupdate__funcs.erase( __name);
}
void lu_cw_del_start(std::string __name) {
	__lu_start__funcs.erase(__name);
}

Window::Component __lu_component_impl;
void lu_cw_start_func_impl() {
	for (int i = 0; i < __lu_start__funcs.size(); i++) {
		luabridge::LuaRef _ref = __lu_start__funcs.begin()++->second;
		if (_ref.isFunction()) { _ref(); }
	}
}
void lu_cw____update_func_impl() {
	for (int i = 0; i < __lu_update__funcs.size(); i++) {
		luabridge::LuaRef _ref = __lu_update__funcs.begin()++->second;
		if (_ref.isFunction()) { _ref(); }
	}
}
void lu_cw_posupdate_func_impl() {
	for (int i = 0; i < __lu_postupdate__funcs.size(); i++) {
		luabridge::LuaRef _ref = __lu_postupdate__funcs.begin()++->second;
		if (_ref.isFunction()) { _ref(); }
	}
	lu_SDL_Window_impl::__INSTANCE->Update();
}
void lu_cw_preupdate_func_impl() {
	for (int i = 0; i < __lu_preupdate__funcs.size(); i++) {
		luabridge::LuaRef _ref = __lu_preupdate__funcs.begin()++->second;
		if (_ref.isFunction()) { _ref(); }
	}
}

luabridge::LuaRef __lu_on_move = 0;
luabridge::LuaRef __lu_on_click = 0;
luabridge::LuaRef __lu_on_keypress = 0;
luabridge::LuaRef __lu_on_keyrelease = 0;
luabridge::LuaRef __lu_on_keyhold = 0;

void lu_do_on(const std::string& do_s, luabridge::LuaRef __func) {
	printf("\na\n");
	if (!__func.isFunction()) {
		luaL_error(state, "function not found");
	}
	if (alib_costr(do_s, "key_press")) {
		__lu_on_keypress = __func;
		Window::WindowInstance->keyboard.onKeyPress = __lu_on_keypress;
	}
	if (alib_costr(do_s, "key_release")) {
		__lu_on_keyrelease = __func;
		Window::WindowInstance->keyboard.onKeyRelease = __lu_on_keyrelease;
	}
	if (alib_costr(do_s, "key_hold")) {
		__lu_on_keyhold = __func;
		Window::WindowInstance->keyboard.onKeyHold = __lu_on_keyhold;
	}
	if (alib_costr(do_s, "click")) {
		__lu_on_click = __func;
		Window::WindowInstance->mouse.onClick = __lu_on_click;
	}
	if (alib_costr(do_s, "move")) {
		__lu_on_move = __func;
		Window::WindowInstance->mouse.onMove = __lu_on_move;
	}
}


lu_SDL_Window_impl* lu_cw_get_window() {
	return lu_SDL_Window_impl::__INSTANCE;
}
struct lu_SDL_Rect_impl : public SDL_Rect{
	void set_bulk(int x, int y, int w, int h) { *this= { x,y,w,h }; }
	std::array<int, 4> get_bulk() { return {this->x,this->y, this->w, this->h}; }
};
bool lu_cw_keydownf(int key) { return Window::WindowInstance->keyboard.GetKeyDown(key); }
bool lu_cw_keyf(int key) { return Window::WindowInstance->keyboard.GetKey(key); }
bool lu_cw_keyupf(int key) { return !Window::WindowInstance->keyboard.GetKey(key); }

Vector lu_cw_vector__get(Vector const& _v) { return _v; }
void lu_cw_vector__set(Vector & _vec, double _vec_val) { _vec = _vec_val; }
double lu_get_dt_scaled() { return *_lu_deltaTime; }
double lu_get_dt_scale_val() { return *_lu_deltaTimeSCALE; }
double lu_get_dt_unscaled() { return *_lu_deltaTimeUS; }
double lu_get_dt_fps() { return *_lu_deltaTimefps; }
alib_inline_run _nn([&]() {
	__lu_component_impl.Start = &lu_cw_start_func_impl;
	__lu_component_impl.Update = &lu_cw____update_func_impl;
	__lu_component_impl.PreUpdate = &lu_cw_preupdate_func_impl;
	__lu_component_impl.PostUpdate = &lu_cw_posupdate_func_impl;
	initLUA(state);

	luaL_openlibs(state);
	luaopen_base(state);
	deleteGlobalLua(state, "utf8");
	assignInputData(alu_global);
	alu_global.beginNamespace("input")
		
		.addFunction("key_pressed", &lu_cw_keydownf)
		.addFunction("key_up", &lu_cw_keyupf)
		.addFunction("key_down", &lu_cw_keyf)
	.endNamespace();
	setGlobalLuaNum(state, "c_empty", COL_EMPTY);
	setGlobalLuaNum(state, "c_player", COL_PLAYER);
	setGlobalLuaNum(state, "c_entity", COL_ENT);
	setGlobalLuaNum(state, "c_object", COL_OBJ);
	setGlobalLuaNum(state, "c_trigger", COL_TRG);
	setGlobalLuaNum(state, "c_solid", COL_SOLID);
	alu_global.beginClass<lu_SDL_Window_impl>("WindowImpl")
		.addData("name", &lu_SDL_Window_impl::__name)
		.addData("debug", &lu_SDL_Window_impl::debug)
		.addData("debug_window", &lu_SDL_Window_impl::debug_window_open)
	.endClass();
//	alu_global.beginClass<Vector>("Vector")
//		.addFunction("new", &Vector::lu_new)
//		.addFunction("__index", &Vector::lu_get)
//		.addFunction("__newindex", &Vector::lu_set)
//		.addFunction("__concat", &Vector::lu_concat)
//		.addStaticFunction("distance", &Vector::distance)
//		.endClass();
	alu_global.beginClass<Vector2>("Vector2")
		.addStaticFunction("new", &Vector2::lu_new) // ctor
		.addConstructor<Vector2(*)(int, int)>()
		.addStaticFunction("distance", &Vector2::distance)
		.addData("x", &Vector2::x)
		.addData("y", &Vector2::y)
		.addStaticFunction("invert", &Vector2::lu_invert)
		.addStaticFunction("abs", &Vector2::lu_abs)
		.endClass();
	alu_global.beginClass<MeshLine>("Line")
		.addConstructor<void(*)(double, double, double, double)>()
		.addConstructor<void(*)(Vector2, Vector2)>()
		.addStaticFunction("new", &MeshLine::lu_new)
		.addData("start", &MeshLine::start)
		.addData("end", &MeshLine::end)
		.addData("layer", &MeshLine::layer)
		.addData("id", &MeshLine::coll_id)
		.addFunction("set_freestanding", &MeshLine::setFreestanding)
		.addFunction("erase_freestanding", &MeshLine::eraseFreestanding)
		.endClass();
	alu_global.beginClass<VectorRect>("Rect")
		.addStaticFunction("new", &VectorRect::lu_new)
		.addConstructor<VectorRect(*)(int, int, int, int)>()
		.addData("x", &VectorRect::x)
		.addData("y", &VectorRect::y)
		.addData("w", &VectorRect::w)
		.addData("h", &VectorRect::h)
		.addStaticFunction("intersects", &VectorRect::lu_intersects)
		.endClass();
	alu_global.beginNamespace("ctypes")

		.beginClass<SDL_Rect>("SDL_Rect")
			.addData("x", &SDL_Rect::x)
			.addData("y", &SDL_Rect::y)
			.addData("w", &SDL_Rect::w)
			.addData("h", &SDL_Rect::h)
			.addConstructor<SDL_Rect*(*)(int,int,int,int)>()
		.endClass()
	.endNamespace();
	alu_global.beginClass<Transform>("Transform")
		.addData("angle", &Transform::angle)
		.addData("origin", &Transform::origin)
		.addData("position", &Transform::position)
		.addData("scale", &Transform::scale)
	.endClass();
	alu_global.beginClass<Sprite>("Sprite")
		.addStaticFunction("new", Sprite::luaNew)
		.addConstructor<Sprite(*)(const char*)>()
		.addData("centered", &Sprite::center)
		.addData("enabled", &Sprite::enabled)
		.addData("name", &Sprite::name)
		.addData("id", &Sprite::identifier)
		.addData("is_rendering", &Sprite::isRendering)
		.addData("center_position", &Sprite::center_position)
		.addData("layer", &Sprite::layer)
		.addData("uv", &Sprite::uv)
		.addData("transform", &Sprite::transform)
		.addData("tile", &Sprite::uv_tile)
		.addData("uv_final", &Sprite::uv_final, false)
		.addFunction("set_uv", &Sprite::lua_set_uv)
		.addFunction("lock", &Sprite::lockCamera)
		.addFunction("unlock", &Sprite::unlockCamera)
		.addData("pixel_sz", &Sprite::pixel_size, false)
	.endClass();
	
	alu_global.beginNamespace("cw")
		.addFunction("load", &lu_cw_loadScene)
		.addFunction("unload", &lu_cw_unloadScene)
		.addFunction("reload", &lu_cw_reloadScene)
		.addFunction("add_start", &lu_cw_add_start)
		.addFunction("add_pre_update", &lu_cw_add_preu)
		.addFunction("add_update", &lu_cw_add_update)
		.addFunction("add_post_update", &lu_cw_add_postu)
		.addFunction("delete_start", &lu_cw_del_start)
		.addFunction("delete_pre_update", &lu_cw_del_preu)
		.addFunction("delete_update", &lu_cw_del_update)
		.addFunction("delete_post_update", &lu_cw_del_postu)
		.addFunction("get_window", &lu_cw_get_window)
		.addFunction("on", &lu_do_on)
		.beginNamespace("Camera")
			.addProperty("position", Camera::lu_GetPosition, Camera::lu_SetPosition)
			.addFunction("target", Camera::lu_SetTarget)
		.endNamespace()
	.endNamespace();

	alu_global.beginNamespace("Time")
		.addProperty("DeltaTime", &lu_get_dt_scaled)
		.addVariable("DeltaTimeUnscaled", &Time::DeltaTimeUnscaled, false)
		.addVariable("fps", &Time::fps, false)
		.addProperty("DeltaTimeScale", &lu_get_dt_scale_val)
		.beginClass<Time::Timer>("Timer")
			.addConstructor<Time::Timer(*)()>()
			.addData("elapsed", &Time::Timer::elapsed, false)
			.addFunction("start", &Time::Timer::Start)
			.addFunction("end", &Time::Timer::End)
		.endClass()
	.endNamespace();
	alu_global.beginNamespace("console")
		.addFunction("log", &lu_logmsg)
		.addFunction("error", &lu_logmsg)
		.addFunction("warn", &lu_logerr)
		.addFunction("debug", &lu_logdeb)
	.endNamespace();
	alu_global
		.addFunction("print", &lu_logsil) // Override print, and have it print to the debug console.
	;
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
		.addStaticFunction("dump_debug", &RectCollider2d::dump_debug)
	.endClass();
	// I despise programming like this
	alu_global.beginNamespace("Raycast")
		.addFunction("line", &Collider::Raycast::Test) // Raycast and check for specific type of collider
		.addFunction("circle", &Collider::Raycast::TestCircle)
		.addFunction("cone", &Collider::Raycast::TestCone)
		.addFunction("any_line", &Collider::Raycast::TestAnyLine)
		.addFunction("any_circle", &Collider::Raycast::TestAnyCircle)
		.addFunction("any_cone", &Collider::Raycast::TestAnyCone)
	.endNamespace();


	alu_global.beginNamespace("math")
		.addFunction("lerp", std::function <Vector2(Vector2, Vector2, float percent)>([](Vector2 start, Vector2 end, float percent) -> Vector2 {
			return (start + (end - start) * percent);
		}))
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
		printf = function(s,...)
			return print(s:format(...))
        end -- function
		io.stderr:setvbuf "no"
		--collectgarbage("stop")
		dofile("lib/debug.lua")
	)");//
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