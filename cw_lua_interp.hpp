#ifndef AST_ASL_HPP
#define AST_ASL_HPP
#define ALIB_FORCE_INLINE_RUNNERS
#include "utils.hpp"
#include "LUA_INCLUDE.h"

#include "settings.h"
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
	bool noclip = false;
	bool debug = true;
	bool debug_window_open = true;
	bool is_editor = false;
	bool debug_window_active = true;
	bool is_interaction_open = false;
	std::unordered_map<std::string, luabridge::LuaRef> events;
	std::vector < std::pair<std::pair<double, double>, luabridge::LuaRef>> asyncTasks = {};
	Sprite* player_sprite;
	void Update() {
		SDL_SetWindowTitle(__win, __name.c_str());
		Window::WindowInstance->debug = this->debug;
		Window::WindowInstance->debug_window_open = this->debug_window_open;
		this->debug_window_active = Window::WindowInstance->debug_window_active;
		this->is_editor = getbitv(Window::WindowInstance->data, 8) != 0;
	}
	//std::string __get_name() { return __name; }
	//void __set_name(std::string ___name) { this->__name = ___name; }
	static inline lu_SDL_Window_impl* __INSTANCE = nullptr;
};

void lu_cw_register_event(std::string name, luabridge::LuaRef f) {
	if (!f.isFunction()) {
		cwError::push(cwError::CW_ERROR);
		std::string t_name = lua_typename(state, f.type());
		cwError::serrof("Unable to register event %s : Event needs to be a function. Got %s", name.c_str(), t_name.c_str());
		cwError::pop();
	}
	lu_SDL_Window_impl::__INSTANCE->events.insert(std::make_pair(name, f));
}
void lu_cw_unregister_event(std::string name) {
	std::unordered_map<std::string, luabridge::LuaRef>& events_ = lu_SDL_Window_impl::__INSTANCE->events;
	if (events_.contains(name)) {
		events_.erase(name);
	}
}
Window* windowInstance;
SDL_Renderer* __sdlRenderer;
SDL_Window* __sdlWindow;
double* _lu_deltaTime;
double* _lu_deltaTimeUS;
double* _lu_deltaTimeSCALE;
double* _lu_deltaTimefps;
void __lu_RenderTaskImpl();
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
	int r, g, b, a;
	// Needs to fit AT LEAST 32 bits
	void setColorL(int32_t c) {
		r = alib_get_byte(&c, 0);
		g = alib_get_byte(&c, 1);
		b = alib_get_byte(&c, 2);
		a = alib_get_byte(&c, 3);
	}
	lu_col(int _r, int _g, int _b, int _a = 255) { 
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
};
void lu_logerr(std::string str) {
	Win->cons.pushlnf("%serr : %s" + str, ImRGB(255,128,128).tostring(), ImRGB::resetstr());
};
void lu_logsil(std::string str) {
	printf(str.c_str());
};
void lu_logdeb(std::string str) {
	Win->cons.pushlnf("%sdebug : %s" + str, ImRGB(200, 200, 128).tostring(), ImRGB::resetstr());
};
void lu_logmsg(std::string str) {
	cwError::sstate(cwError::CW_MESSAGE);
	cwError::serror(str.c_str());
};
void lu_logwrn(std::string str) {
	Win->cons.pushlnf("%swarn : %s" + str, ImRGB(255, 255, 128).tostring(), ImRGB::resetstr());
};
void lu_logvrb(std::string str) {
	Win->cons.pushlnf("%swarn : %s" + str, ImRGB(160, 160, 200).tostring(), ImRGB::resetstr());
};
#define alu_global luabridge::getGlobalNamespace(state)
void do_n(int amt, std::function<void()> __run) {
	for (int i = 0; i < amt; i++) {
		__run();
	}
}
std::map<std::string, const luabridge::LuaRef> __lu_preupdate__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_prerender__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_update__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_postupdate__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_start__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_render__funcs;
std::map<std::string, const luabridge::LuaRef> __lu_on_interact_funcs;
std::unordered_map < std::string, const luabridge::LuaRef > __lu_on_collide_funcs;
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

void lu_cw_add_prer(std::string __name, const luabridge::LuaRef __preufunc) {
	__lu_prerender__funcs.insert({ __name, __preufunc });
}
void lu_cw_add_render(std::string __name, const luabridge::LuaRef __preufunc) {
	__lu_render__funcs.insert({ __name, __preufunc });
}
void lu_cw_add_on_interact(std::string col_id, const luabridge::LuaRef __preufunc) {
	__lu_on_interact_funcs.insert({ col_id, __preufunc });
}

void lu_cw_add_on_collide(std::string source_collider, std::string on_collide_id, const luabridge::LuaRef __preufunc) {
	__lu_on_collide_funcs.insert({ (source_collider + on_collide_id) , __preufunc });
}

void lu_cw_del_preu(std::string __name) {
	__lu_preupdate__funcs.erase(__name);
}
void lu_cw_del_render(std::string __name) {
	__lu_render__funcs.erase(__name);
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
void lu_cw_del_prer(std::string __name) {
	__lu_prerender__funcs.erase(__name);
}
void lu_cw_del_on_interact(std::string __name) {
	__lu_on_interact_funcs.erase(__name);
}
void lu_cw_del_on_collide(std::string col__name, std::string on_collide_id) {
	__lu_on_collide_funcs.erase((col__name+on_collide_id));
}

std::function< void(RectCollider2d*, MeshLine*) > RectCollider2d::OnLineHitRect = [](RectCollider2d* _this, MeshLine* source) {
	std::string v = _this->id + source->id;
	if (__lu_on_collide_funcs.contains(v)) {
		__lu_on_collide_funcs.at(v)();
	}
	else if (__lu_on_collide_funcs.contains(_this->id + "any")) {
		__lu_on_collide_funcs.at(_this->id + "any")();
	}
};

std::function< void(RectCollider2d*, RectCollider2d*) > RectCollider2d::OnColliderHit = [](RectCollider2d* _this, RectCollider2d* source) {
	std::string v = _this->id + source->id;
	if (__lu_on_collide_funcs.contains(v)) {
		__lu_on_collide_funcs.at(v)();
	}
	else if (__lu_on_collide_funcs.contains(_this->id + "any")) {
		__lu_on_collide_funcs.at(_this->id + "any")();
	}
};

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
	auto& tasks = lu_SDL_Window_impl::__INSTANCE->asyncTasks;
	for (int i = 0; i < tasks.size(); i++) {
		auto& ref = tasks.at(i);
		if (ref.first.first /*delta*/ > ref.first.second /*delay*/) {
			if (!ref.second) { return; }
			ref.second();
			tasks.erase(tasks.begin() + i);
			continue;
		}
		ImGuiIO& io = ImGui::GetIO();
		ref.first.first += io.DeltaTime;
	}
	for (int i = 0; i < __lu_preupdate__funcs.size(); i++) {
		luabridge::LuaRef _ref = __lu_preupdate__funcs.begin()++->second;
		if (_ref.isFunction()) { _ref(); }
	}
}
void lu_cw_prerender_func_impl() {
	for (int i = 0; i < __lu_prerender__funcs.size(); i++) {
		luabridge::LuaRef _ref = __lu_prerender__funcs.begin()++->second;
		if (_ref.isFunction()) { _ref(); }
	}
}

void lu_cw_render_func_impl() {
	for (int i = 0; i < __lu_render__funcs.size(); i++) {
		luabridge::LuaRef _ref = __lu_render__funcs.begin()++->second;
		if (_ref.isFunction()) { _ref(); }
	}
	__lu_RenderTaskImpl();
}

luabridge::LuaRef __lu_on_move = 0;
luabridge::LuaRef __lu_on_click = 0;
luabridge::LuaRef __lu_on_keypress = 0;
luabridge::LuaRef __lu_on_keyrelease = 0;
luabridge::LuaRef __lu_on_keyhold = 0;
void lu_do_on(const std::string& do_s, luabridge::LuaRef __func) {
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
	if (alib_costr(do_s, "mouse_click")) {
		__lu_on_click = __func;
		Window::WindowInstance->mouse.onClick = __lu_on_click;
	}
	if (alib_costr(do_s, "mouse_move")) {
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
bool lu_cw_keypressedf(int key) { return Window::WindowInstance->keyboard.GetKeyPressed(key); }
bool lu_cw_keydownf(int key) { return Window::WindowInstance->keyboard.GetKey(key); }
bool lu_cw_keyupf(int key) { return !Window::WindowInstance->keyboard.GetKey(key); }
bool lu_cw_keys_down(std::vector<int> keys) {
	for (int i = 0; i < keys.size(); i++) {
		if (!Window::WindowInstance->keyboard.GetKey(keys[i])) { return false; }
	}
	return true;
}
bool lu_cw_keys_up(std::vector<int> keys) {
	for (int i = 0; i < keys.size(); i++) {
		if (Window::WindowInstance->keyboard.GetKey(keys[i])) { return false; }
	}
	return true;
}

std::vector<std::function<void()>> __renderTasks;

RectCollider2d* lu_new_froms(Sprite* sprite, int __layer) {
	RectCollider2d* r_re = new RectCollider2d();
	r_re->rect = sprite->rect;
	r_re->recalc();
	r_re->layer = __layer;
	return r_re;
}

Vector2 lu_cw_get_cursor_pos() { return {(double)Window::WindowInstance->mouse.x, (double)Window::WindowInstance->mouse.y }; }
Vector2 lu_cw_get_cursor_delta() { return { (double)Window::WindowInstance->mouse.dx, (double)Window::WindowInstance->mouse.dy }; }
bool lu_cw_get_mouse_ld() { return Window::WindowInstance->mouse.m1d; }
bool lu_cw_get_mouse_md() { return Window::WindowInstance->mouse.m2d; }
bool lu_cw_get_mouse_rd() { return Window::WindowInstance->mouse.m3d; }
bool lu_cw_get_mouse_t1d() { return Window::WindowInstance->mouse.mt1d; }
bool lu_cw_get_mouse_t2d() { return Window::WindowInstance->mouse.mt2d; }
Vector lu_cw_vector__get(Vector const& _v) { return _v; }
void lu_cw_vector__set(Vector & _vec, double _vec_val) { _vec = _vec_val; }
double lu_get_dt_scaled() { return *_lu_deltaTime; }
double lu_get_dt_scale_val() { return *_lu_deltaTimeSCALE; }
double lu_get_dt_unscaled() { return *_lu_deltaTimeUS; }
double lu_get_dt_fps() { return *_lu_deltaTimefps; }


void lu_new_textbox(std::string label, std::string m_text, luabridge::LuaRef on_complete_func) {
	new UI::GameTextBox(label, m_text, on_complete_func);
}

UI::GameOptionsBox* lu_new_optionsbox(std::string title, ABTDataStub options, luabridge::LuaRef on_complete) {
	return new UI::GameOptionsBox(title, options, on_complete);
}

UI::GameOptionsBox* lu_new_optionsbox_WITHALIGNMENT(std::string title, ABTDataStub options, luabridge::LuaRef on_complete, int alignment) {
	return new UI::GameOptionsBox(title, options, on_complete, alignment);
}



alib_inline_run _nn([&]() {
	__lu_component_impl.Start = &lu_cw_start_func_impl;
	__lu_component_impl.Update = &lu_cw____update_func_impl;
	__lu_component_impl.PreUpdate = &lu_cw_preupdate_func_impl;
	__lu_component_impl.PostUpdate = &lu_cw_posupdate_func_impl;
	__lu_component_impl.PreRender = &lu_cw_prerender_func_impl;
	__lu_component_impl.Render = &lu_cw_render_func_impl;
	initLUA(state);
	luaL_openlibs(state);
	luaopen_base(state);
	deleteGlobalLua(state, "utf8");
	assignInputData(alu_global);
	alu_global.beginNamespace("CWSettings")
		.addFunction("save", &CWSettings::Save)
		.addFunction("load", &CWSettings::Parse)
		.beginNamespace("rendering")
			.addVariable("always_show_borders", &CWSettings::Rendering::ALWAYS_SHOW_BORDERS)
		.endNamespace()
		.beginNamespace("keybinds")
			.addVariable("use", &CWSettings::KeyBinds::USE, false)
			.addVariable("up", &CWSettings::KeyBinds::UP, false)
			.addVariable("down", &CWSettings::KeyBinds::DOWN, false)
			.addVariable("left", &CWSettings::KeyBinds::LEFT, false)
			.addVariable("right", &CWSettings::KeyBinds::RIGHT, false)
			.addVariable("pause", &CWSettings::KeyBinds::PAUSE, false)
			.addVariable("inventory", &CWSettings::KeyBinds::INVENTORY, false)
		.endNamespace()
	.endNamespace();
	alu_global.beginNamespace("input")
		.addFunction("key_pressed", &lu_cw_keypressedf)
		.addFunction("key_up", &lu_cw_keyupf)
		.addFunction("key_down", &lu_cw_keydownf)
		.addFunction("keys_down", lu_cw_keys_down)
		.addFunction("keys_up", lu_cw_keys_up)
		.beginNamespace("mouse")
			.addProperty("position", &lu_cw_get_cursor_pos)
			.addProperty("delta", &lu_cw_get_cursor_delta)
			.addProperty("left", &lu_cw_get_mouse_ld)
			.addProperty("right", &lu_cw_get_mouse_rd)
			.addProperty("middle", &lu_cw_get_mouse_md)
		.endNamespace()
	.endNamespace();
	
	setGlobalLuaNum(state, "c_empty", COL_EMPTY);
	setGlobalLuaNum(state, "c_player", COL_PLAYER);
	setGlobalLuaNum(state, "c_entity", COL_ENT);
	setGlobalLuaNum(state, "c_object", COL_OBJ);
	setGlobalLuaNum(state, "c_trigger", COL_TRG);
	setGlobalLuaNum(state, "c_solid", COL_SOLID);
	setGlobalLuaNum(state, "COL_EMPTY", COL_EMPTY);
	setGlobalLuaNum(state, "COL_PLAYER", COL_PLAYER);
	setGlobalLuaNum(state, "COL_ENTITY", COL_ENT);
	setGlobalLuaNum(state, "COL_OBJECT", COL_OBJ);
	setGlobalLuaNum(state, "COL_TRIGGER", COL_TRG);
	setGlobalLuaNum(state, "COL_SOLID", COL_SOLID);
	
	alu_global.beginClass<ABTDataStub>("ABT")
		.addFunction("setFloat", &ABTDataStub::set_float)
		.addFunction("addFloat", &ABTDataStub::add_float)
		.addFunction("getFloat", &ABTDataStub::get_float)
		.addFunction("setInt", &ABTDataStub::set_int)
		.addFunction("addInt", &ABTDataStub::add_int)
		.addFunction("getInt", &ABTDataStub::get_int)
		.addFunction("setString", &ABTDataStub::set_string)
		.addFunction("addString", &ABTDataStub::add_string)
		.addFunction("getString", &ABTDataStub::get_string)
		.addFunction("setBool", &ABTDataStub::set_bool)
		.addFunction("addBool", &ABTDataStub::add_bool)
		.addFunction("getBool", &ABTDataStub::get_bool)
		.addFunction("clear", &ABTDataStub::clear)
		.addFunction("keys", &ABTDataStub::get_keys)
		.addFunction("contains", &ABTDataStub::m_contains)
		.addFunction("serialize", &ABTDataStub::serialize_json)
		.addFunction("deserialize", &ABTDataStub::deserialize_json)
		.addConstructor<ABTDataStub(*)()>()
	.endClass();

	alu_global.beginClass<lu_SDL_Window_impl>("WindowImpl")
		.addData("name", &lu_SDL_Window_impl::__name)
		.addData("debug", &lu_SDL_Window_impl::debug)
		.addData("debug_window", &lu_SDL_Window_impl::debug_window_open)
		.addData("debug_window_active", &lu_SDL_Window_impl::debug_window_active, false)
		.addData("is_editor", &lu_SDL_Window_impl::is_editor, false)
		.addData("noclip", &lu_SDL_Window_impl::noclip)
	.endClass();
	alu_global.beginClass<Vector2>("Vector2")
		.addStaticFunction("new", &Vector2::lu_new) // ctor
		.addConstructor<Vector2(*)(int, int)>()
		.addStaticFunction("distance", &Vector2::distance)
		.addData("x", &Vector2::x)
		.addData("y", &Vector2::y)
		.addStaticData("up", &Vector2::up, false)
		.addStaticData("right", &Vector2::right, false)
		.addStaticFunction("invert", &Vector2::lu_invert)
		.addStaticFunction("abs", &Vector2::lu_abs)
		.addFunction("magnitude", &Vector2::magnitude)
		.addFunction("normalize", &Vector2::normalize)
		.addStaticFunction("cross", &Vector2::cross)
		.addStaticFunction("dot", &Vector2::dot)
		.addStaticFunction("angle", &Vector2::angle)
		.addFunction("clamp_magnitude", &Vector2::clamp_magnitude)
		.addFunction("__add", &Vector2::luaL__add)
		.addFunction("__sub", &Vector2::luaL__sub)
		.addFunction("__mul", &Vector2::luaL__mul)
		.addFunction("__div", &Vector2::luaL__div)
		.addFunction("__unm", &Vector2::luaL__unm)
		.addFunction("__eq", &Vector2::luaL__eq)
		.addFunction("__lt", &Vector2::luaL__lt)
		.addFunction("__le", &Vector2::luaL__le)
	.endClass();
	alu_global.beginClass<MeshLine>("Line")
		.addConstructor<void(*)(double, double, double, double)>()
		.addConstructor<void(*)(Vector2, Vector2)>()
		.addStaticFunction("new", &MeshLine::lu_new)
		.addData("start", &MeshLine::start)
		.addData("end", &MeshLine::end)
		.addData("layer", &MeshLine::layer)
		.addData("id", &MeshLine::coll_id)
		.addFunction("set_freestanding", &MeshLine::set_freestanding)
		.addFunction("erase_freestanding", &MeshLine::erase_freestanding)
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
	alu_global.beginClass<SDL_Rect>("SDL_Rect")
		.addData("x", &SDL_Rect::x)
		.addData("y", &SDL_Rect::y)
		.addData("w", &SDL_Rect::w)
		.addData("h", &SDL_Rect::h)
		.addConstructor<SDL_Rect* (*)(int, int, int, int)>()
	.endClass();

	alu_global.beginNamespace("cdecl")
		.addConstant("NDEBUG", NDEBUG)
	.endNamespace();
	alu_global.beginClass<Transform>("Transform")
		.addData("angle", &Transform::angle)
		.addData("origin", &Transform::origin)
		.addData("position", &Transform::position)
		.addData("scale", &Transform::scale)
	.endClass();
	alu_global.beginClass<SpriteAnimationMeta>("SpriteMeta")
		.addConstructor<SpriteAnimationMeta(*)(int, float)>()
		.addData("animate", &SpriteAnimationMeta::animate)
		.addData("animate_once", &SpriteAnimationMeta::animate_once)
		.addData("resets", &SpriteAnimationMeta::reset_after_once)
		.addData("frame", &SpriteAnimationMeta::CURRENT_FRAME)
		.addData("delay", &SpriteAnimationMeta::delay)
		.endClass();
	alu_global.beginClass<Sprite>("Sprite")
		.addStaticFunction("new", Sprite::luaNew)
		.addConstructor<Sprite(*)(const char*)>()
		.addData("meta", &Sprite::meta)
		.addData("alpha", &Sprite::alpha)
		.addData("centered", &Sprite::center)
		.addData("enabled", &Sprite::enabled)
		.addData("name", &Sprite::name)
		.addProperty("id", 
			std::function <std::string(const Sprite*)>([](const Sprite* s) {
				return s->identifier;
			}),
			std::function <void(Sprite*, std::string)>([](Sprite* s, std::string id) {
				s->setID(id);
			})
		)
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
		.addFunction("update", &Sprite::Update)
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
		.addFunction("add_pre_render", &lu_cw_add_prer)
		.addFunction("delete_pre_render", &lu_cw_del_prer)
		.addFunction("add_render_task", &lu_cw_add_render)
		.addFunction("delete_render_task", &lu_cw_del_render)
		.addFunction("delete_start", &lu_cw_del_start)
		.addFunction("delete_pre_update", &lu_cw_del_preu)
		.addFunction("delete_update", &lu_cw_del_update)
		.addFunction("delete_post_update", &lu_cw_del_postu)
		.addFunction("get_window", &lu_cw_get_window)
		.addFunction("run_async", std::function<void(double, luabridge::LuaRef)>([](double delay, luabridge::LuaRef func) {
			if (!func.isFunction()) {
				cwError::push(cwError::CW_ERROR);
				std::string t_name = lua_typename(state, func.type());
				cwError::serrof("Unable to register async event : Event needs to be a function. Got %s", t_name.c_str());
				cwError::pop();
				return;
			}
			lu_cw_get_window()->asyncTasks.push_back(std::make_pair(std::make_pair(0, delay), func));
		}))
		.addFunction("set_overlay_enabled", std::function<void(bool)>([](bool s) {
				setbitv(Window::WindowInstance->data, 2, s);
			}))
		.addFunction("on", &lu_do_on)
		.addFunction("re", &lu_cw_register_event)
		.addFunction("ue", &lu_cw_unregister_event)
		.addFunction("ce", std::function<void(std::string)>([](std::string name) {
		std::unordered_map<std::string, luabridge::LuaRef> events = lu_SDL_Window_impl::__INSTANCE->events;
			if (events.contains(name)) {
				events.at(name)();
			}
		}))
		.addFunction("add_interaction", &lu_cw_add_on_interact)
		.addFunction("delete_interaction", &lu_cw_del_on_interact)
		.addFunction("add_collision", &lu_cw_add_on_collide)
		.addFunction("delete_collision", &lu_cw_del_on_collide)
		.addFunction("get_sprite", &Sprite::getElementByID)
		.addFunction("get_particle", &ParticleEffect::getElementByID)
		.addFunction("associate_player", std::function<void(Sprite*)>([](Sprite* s) {
				lu_cw_get_window()->player_sprite = s;
			}))
		.addFunction("get_player", std::function<Sprite*()>([]() {
				return lu_cw_get_window()->player_sprite;
			}))
		.addVariable("is_interacting", &UI::GameTextBox::isInteracting)
		.addFunction("m_runInteraction", std::function <bool(std::string)>([](std::string id) -> bool{

			if (UI::GameTextBox::isInteracting) {
				return false;
			}
			printf("id:'%s'\n", id.c_str());
			if (!__lu_on_interact_funcs.contains(id)) { return false; }
			__lu_on_interact_funcs.at(id)();
			return true;
		}))
		.beginNamespace("Camera")
			.addProperty("position", Camera::lu_GetPosition, Camera::lu_SetPosition)
			.addFunction("target", Camera::lu_SetTarget)
			.addProperty("zoom", Camera::lu_GetScale, Camera::lu_SetScale)
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
		.addFunction("error", &lu_logerr)
		.addFunction("warn", &lu_logwrn)
		.addFunction("debug", &lu_logdeb)
		.addFunction("cout", &lu_logsil)
	.endNamespace();
	alu_global
		.addFunction("print", &lu_logsil) // Override print, and have it print to the debug console.
	;
	alu_global.beginClass<RectCollider2d>("RectCollider")
		.addStaticFunction("new", &RectCollider2d::lu_new_fromi)
		.addStaticFunction("new_centered", &RectCollider2d::lu_new_fromic)
		.addStaticFunction("from_rect", &RectCollider2d::lu_new_fromr)
		.addStaticFunction("from_sprite", &lu_new_froms)
		.addData("layer", &RectCollider2d::layer)
		.addProperty("id",
			std::function <std::string(const RectCollider2d*)>([](const RectCollider2d* s) {
				return s->id;
				}),
			std::function <void(RectCollider2d*, std::string)>([](RectCollider2d* s, std::string id) {
					s->id = id;
				})
		)
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
		.addFunction("any_line_except", &Collider::Raycast::TestExcept)
		.addFunction("any_circle_except", &Collider::Raycast::TestCircleExcept)
		.addFunction("any_cone_except", &Collider::Raycast::TestConeExcept)
	.endNamespace();
	alu_global.beginNamespace("RaycastHit")
		.addVariable("start", &Collider::Hit::start, false)
		.addVariable("end", &Collider::Hit::end, false)
		.addVariable("length", &Collider::Hit::length, false)
		.addVariable("pos", &Collider::Hit::pos, false)
		.addVariable("hasHit", &Collider::Hit::hasHit, false)
		.addVariable("layer", &Collider::Hit::layer, false)
		.addVariable("id", &Collider::Hit::col_id, false)
	.endNamespace();
	alu_global.beginClass<ParticleEffects::stub>("ParticleEffectType")
		.addData("meta", &ParticleEffects::stub::meta)
		.addData("uv", &ParticleEffects::stub::uv)
		.addData("id", &ParticleEffects::stub::id)
		.addStaticData("RAIN", &ParticleEffects::RAIN, false)
		.addStaticData("BLOOD", &ParticleEffects::BLOOD, false)
		.addStaticData("SNOW", &ParticleEffects::SNOW, false)
		.addStaticData("FLAME", &ParticleEffects::FLAME, false)
	.endClass();
	alu_global.beginClass<ParticleEffect>("Particle")
		.addConstructor<ParticleEffect*(*)(ParticleEffects::stub, int)>()
		.addData("uv", &ParticleEffect::m_uv)
		.addProperty("direction",
			std::function<float(const ParticleEffect*)>([](const ParticleEffect* e) {return alib_rad2deg(e->m_dir_angle_rad); }),
			std::function<void(ParticleEffect*, float)>([](ParticleEffect* e,float a) {
				e->m_dir_angle_rad = alib_deg2rad(a);
			}))
		.addData("layer", &ParticleEffect::m_layer)
		.addData("lifetime", &ParticleEffect::m_lifetime)
		.addData("pos", &ParticleEffect::start_pos)
		.addData("randomness", &ParticleEffect::m_randomness)
		.addFunction("add", &ParticleEffect::append)
		.addFunction("remove", &ParticleEffect::pop)
	.endClass();
	alu_global.beginNamespace("math")
		.addFunction("lerp", std::function <Vector2(Vector2, Vector2, float percent)>([](Vector2 start, Vector2 end, float percent) -> Vector2 {
			return (start + (end - start) * percent);
		}))
		.addFunction("clerp", std::function <Vector2(Vector2, Vector2, float percent)>([](Vector2 start, Vector2 end, float percent) -> Vector2 {
			return alib_clamp((start + (end - start) * percent), start, end);
		}))
	.endNamespace();
	alu_global.beginClass<AudioWAV>("Audio")
		.addFunction("name", &AudioWAV::name)
		.addConstructor<AudioWAV*(*)(std::string)>()
		//.addStaticFunction("new", &AudioWAV::lu_new)
		.addFunction("end", &AudioWAV::operator~)
		.addFunction("play", &AudioWAV::Play)
		.addFunction("stop", &AudioWAV::Stop)
		.addFunction("play_set_volume", &AudioWAV::SetVolume)
		.addFunction("play_get_volume", &AudioWAV::GetVolume)
		.addFunction("get_error", &AudioWAV::GetError)
		.addData("left", &AudioWAV::pan_left, true)
		.addData("right", &AudioWAV::pan_right, true)
		.addData("distance", &AudioWAV::pan_left, true)
		.addData("volume", &AudioWAV::volume, true)
		//.addProperty("volume", &AudioWAV::GetVolume, &AudioWAV::SetVolume)
	.endClass();
	alu_global.beginClass<lu_col>("Color")
		.addConstructor<lu_col(*)(int, int, int, int)>()
		.addConstructor<lu_col(*)(int, int, int)>()
		.addData("r", &lu_col::r, true)
		.addData("g", &lu_col::g, true)
		.addData("b", &lu_col::b, true)
		.addData("a", &lu_col::a, true)
	.endClass();
	alu_global.beginNamespace("Alignment")
		.addConstant("left", UI::Alignment::LEFT)
		.addConstant("right", UI::Alignment::RIGHT)
		.addConstant("center", UI::Alignment::CENTER)
		.addConstant("top", UI::Alignment::TOP)
		.addConstant("bottom", UI::Alignment::BOTTOM)
	.endNamespace();
	alu_global.beginClass<UI::GameOptionsBox>("__GameOptionBox")
		.addData("alignment", &UI::GameOptionsBox::alignment)
		.addData("title", &UI::GameOptionsBox::title)
		.addData("options", &UI::GameOptionsBox::options)
	.endClass();
	alu_global.beginNamespace("Render")
		.addFunction("SetDrawColor", std::function<void(lu_col)>([](lu_col _col) {
			SDL_SetRenderDrawColor(__sdlRenderer, _col.r, _col.g, _col.b, _col.a);
		}))
		.addFunction("DrawLine", std::function<void(Vector2, Vector2)>([&](Vector2 start, Vector2 end) {
			SDL_RenderDrawLine(__sdlRenderer, start.x, start.y, end.x, end.y);
		}))
		.addFunction("DrawRect", std::function<void(VectorRect)>([&](VectorRect r) {
			SDL_Rect _r = r;
			SDL_RenderDrawRect(__sdlRenderer, &_r);
		}))
		.addFunction("Clear", std::function<void()>([&]() {
			SDL_RenderClear(__sdlRenderer);
		}))
		.addFunction("DrawText", &ImGui::TextForeground)
		.addFunction("TextBox", &lu_new_textbox)
		.addFunction("OptionsBox", &lu_new_optionsbox)
		.addFunction("OptionsBoxAligned", &lu_new_optionsbox_WITHALIGNMENT)
	.endNamespace();
	int status = luaL_dostring(state, R"(
		printf = function(s,...)
			return print(s:format(...))
        end -- function
		io.stderr:setvbuf "no"
		--collectgarbage("stop")
		dofile("lib/debug.lua")
		function foldr(f, ...)
			if select('#', ...) < 2 then return ... end
			local function helper(x, ...)
				if select('#', ...) == 0 then
				  return x
				end
				return f(x, helper(...))
			end
			return helper(...)
		end
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
void __lu_RenderTaskImpl() {
	for (int i = 0; i < __renderTasks.size(); i++) {
		printf("doing render task %d\n", i);
		__renderTasks.at(i)();
	}
	__renderTasks.clear();
}



#endif