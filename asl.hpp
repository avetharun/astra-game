#pragma once
#ifndef AST_ASL_HPP
#define AST_ASL_HPP
#define ALIB_FORCE_INLINE_RUNNERS
#include "utils.hpp"

#include <random>
#include <luaaa.hpp>
lua_State* state = luaL_newstate(); // create and init lua
using namespace luaaa;
#include "engine.hpp"
#include "renderer.hpp"
#include "Vectors.hpp"
#include "Audio.hpp"
#include "physics.hpp"
#include "cwlib/cwlib.hpp"

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
void initDataLua(Sprite* _s) {
	playersprite = _s;
	windowInstance = Win;
}

Transform* _newTransformLua(std::array<int, 2> pos, int a) {
	return new Transform{
		{pos[0],pos[1]},
		{},
		{},
		a
	};
}
struct luaTransform : private Transform {
	luaTransform() {}
	luaTransform(Vector2 _pos, int _angle) {
		this->position.x = _pos.x;
		this->position.y = _pos.y;
	}
	luaTransform(std::array<int, 2> _pos, int angle) {
		*this = { Vector2{_pos[0], _pos[1]}, angle };
	}
	std::array<int, 2> getPosition() {
		return {this->position.x, this->position.y};
	}
	std::array<int, 2> getScale() {
		return { this->scale.x, this->scale.y };
	}
	int getAngle() {
		return this->angle;
	}
	luaTransform setAngle(int amt) {
		this->angle = amt;
		return *this;
	}
	luaTransform setPosition(std::array<int, 2> _pos) {
		this->position = {_pos[0], _pos[1]};
		return *this;
	}
	luaTransform setScale(std::array<int, 2> _scl) {
		this->scale = { _scl[0], _scl[1] };
	}
};
struct luaColor {
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
	void setColor(luaColor _c) {
		cols = _c.cols;
	}
};
struct luaPixbuf {
	luaTransform transform;
	Sprite* _s;
	int getWidth() {
		return _s->surf->w;
	}
	int getHeight() {
		return _s->surf->h;
	}
	luaTransform getTransform() {
		return transform;
	}
	void resize(std::array<int,2> s) {
		char* _tmp = nullptr;
		size_t o_w = _s->surf->w;
		size_t o_h = _s->surf->h;
		size_t pxsz_o = _s->surf->w * _s->surf->h * _s->surf->format->BytesPerPixel;
		size_t pxsz = s[0] * s[1] * _s->surf->format->BytesPerPixel;
		_tmp = (char*)malloc(pxsz_o);
		if (_tmp == nullptr) {
			windowInstance->cons.pushf("Pixbuf.Resize failed!");
			return;
		}
		// Copy the pixel buffer into the new array
		memcpy(_tmp, _s->surf->pixels, pxsz_o);
		SDL_FreeSurface(_s->surf);
		_s->surf = SDL_CreateRGBSurface(SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET, s[0], s[1], 32, 0,0,0,0);
		SDL_LockSurface(_s->surf);
		// Copy each horizontal row of the old image onto the new one
		for (size_t xpc = 0; xpc < o_w; xpc++) {
			size_t offset = alib_2d_ar_pos(s[0], xpc - o_w, 0);
			memcpy(_s->surf->pixels, _tmp, o_w);
		}
		// Free the temporary buffer
		free(_tmp);
		SDL_UnlockSurface(_s->surf);
	}

	void Update() {
		// Run default sprite update task
		_s->Update();
		// Run extra tasks (ie. re-calculating pixels)
	}
	void Draw() {
		_s->Draw();
	}
	void luaDrawPixel(std::array<int, 2> p, luaColor c) {
		SDL_LockSurface(_s->surf);
		size_t offset = alib_2d_ar_pos(_s->surf->pitch, p[0], p[1], _s->surf->format->BytesPerPixel);
		memset(&_s->surf->pixels + offset, *c.cols.data(), 4);
		SDL_UnlockSurface(_s->surf);
	}
	void luaDrawLine(std::array<std::array<int, 2>, 2> l, luaColor c) {
		// push
		int x1 = l[0][0];
		int y1 = l[0][1];
		int x2 = l[1][0];
		int y2 = l[1][1];

		unsigned char lr, lg, lb, la;
		SDL_GetRenderDrawColor(Win->SDL_REND, &lr, &lg, &lb, &la);

		SDL_Texture* tex = SDL_CreateTextureFromSurface(SDL_REND_RHPP, _s->surf);
		SDL_SetRenderTarget(Win->SDL_REND, tex);
		SDL_SetRenderDrawColor(Win->SDL_REND, c.getR(), c.getG(), c.getB(), c.getA());
		SDL_RenderDrawLine(Win->SDL_REND, x1, y1, x2, y2);

		(SDL_RenderCopyEx(SDL_REND_RHPP, tex, NULL, NULL, NULL, NULL, SDL_FLIP_NONE) < 0)
			? []() {std::cout << SDL_GetError() << '\n'; }() : noop;
		// pop
		SDL_SetRenderTarget(Win->SDL_REND, NULL);
		SDL_SetRenderDrawColor(Win->SDL_REND, lr, lg, lb, la);
		SDL_DestroyTexture(tex);
	}
	void luaDrawRect(std::array<std::array<int, 2>, 2> r, luaColor c) {
		int x, y, w, h;
		x = r[0][0];
		y = r[0][1];
		w = r[1][0];
		h = r[1][1];


		// push
		unsigned char lr, lg, lb, la;
		SDL_GetRenderDrawColor(Win->SDL_REND, &lr, &lg, &lb, &la);

		SDL_Texture* tex = SDL_CreateTextureFromSurface(SDL_REND_RHPP, _s->surf);
		SDL_SetRenderTarget(Win->SDL_REND, tex);
		SDL_SetRenderDrawColor(Win->SDL_REND, c.getR(), c.getG(), c.getB(), c.getA());
		SDL_RenderDrawRect(Win->SDL_REND, new SDL_Rect{x,y,w,h});

		(SDL_RenderCopyEx(SDL_REND_RHPP, tex, NULL, NULL, NULL, NULL, SDL_FLIP_NONE) < 0)
			? []() {std::cout << SDL_GetError() << '\n'; }() : noop;
		// pop
		SDL_SetRenderTarget(Win->SDL_REND, NULL);
		SDL_SetRenderDrawColor(Win->SDL_REND, lr, lg, lb, la);
		SDL_DestroyTexture(tex);
		Win->cons.pushf("Created sprite!");
	}

	luaPixbuf(std::array<int, 2> s) {
		_s = new Sprite();
		_s->enabled = true;
		_s->transform.position = {0,0};
		_s->transform.scale = { s[0],s[1]};
		_s->surf = SDL_CreateRGBSurface(SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET, s[0], s[1], 32, 0, 0, 0, 0);
		SDL_Texture* tex = SDL_CreateTextureFromSurface(SDL_REND_RHPP, _s->surf);
		SDL_SetRenderTarget(Win->SDL_REND, tex);
		SDL_SetRenderDrawColor(Win->SDL_REND, 128, 128, 128, 255);
		SDL_RenderClear(Win->SDL_REND);
		SDL_DestroyTexture(tex);
		Win->cons.pushf("Created sprite!");
	}
	luaPixbuf() {
		_s = new Sprite();
		_s->enabled = true;
		_s->transform.position = { 0,0 };
		_s->transform.scale = { 128,128 };
		_s->surf = SDL_CreateRGBSurface(SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET, 128, 128, 32, 0, 0, 0, 0);
		SDL_Texture* tex = SDL_CreateTextureFromSurface(SDL_REND_RHPP, _s->surf);
		SDL_SetRenderTarget(Win->SDL_REND, tex);
		SDL_SetRenderDrawColor(Win->SDL_REND, 128, 128, 128, 255);
		SDL_RenderClear(Win->SDL_REND);
		SDL_DestroyTexture(tex);
	}
	void operator~() {
		_s->operator~();
	}
};

alib_inline_run _nn([&]() {

	luaL_openlibs(state);
	luaopen_base(state);
	deleteGlobalLua(state, "debug");
	deleteGlobalLua(state, "os");
	deleteGlobalLua(state, "xpcall");
	deleteGlobalLua(state, "io");
	deleteGlobalLua(state, "package");


	LuaModule global = LuaModule(state);
	LuaClass<Sprite> luaSpriteCls(state, "Sprite");
	LuaClass<Object> luaObjCls(state, "Object");
	LuaClass<Collider::Line> luaColLineCls(state, "CollisionLine");
	LuaClass<Collider::Mesh> luaColMeshCls(state, "CollisionMesh");
	LuaClass<Collider::Rect> luaColRectCls(state, "CollisionRect");
	LuaClass<Collider::Hit> luaColHitCls(state, "Hit");
	LuaClass<Collider::Raycast> luaColRaycastCls(state, "Raycast");
	LuaClass<luaTransform> luaTransformCls(state, "Transform");
	LuaClass<Vector2> luaVec2Cls(state, "Vector2");
	LuaClass <luaPixbuf> luaPixbufCls(state, "Pixbuf");


	LuaModule playerModu(state, "Player");
	LuaModule cwModu(state, "cw");
	LuaModule consModu(state, "console");
	LuaModule audModu(state, "audio");

	luaL_dostring(state, R"(
dofile("debug.lua")
)");
	LuaModule cwErrorStates(state, "cwErrorState");
	cwErrorStates.def("error", (int)cwError::CW_ERROR);
	cwErrorStates.def("warn", (int)cwError::CW_WARN);
	cwErrorStates.def("message", (int)cwError::CW_MESSAGE);
	cwErrorStates.def("verbose", (int)cwError::CW_VERBOSE);
	cwErrorStates.def("silent", (int)cwError::CW_SILENT);
	consModu.fun("error", [&](std::string str) {
		cwError::sstate(cwError::CW_ERROR);
		cwError::serror(str.c_str());
	});
	consModu.fun("log", [&](std::string str) {
		cwError::sstate(cwError::CW_MESSAGE);
		cwError::serror(str.c_str());
	});
	consModu.fun("debug", [&](std::string str) {
		cwError::sstate(cwError::CW_DEBUG);
		cwError::serror(str.c_str());
	});
	consModu.fun("warn", [&](std::string str) {
		cwError::sstate(cwError::CW_WARN);
		cwError::serror(str.c_str());
	});
	audModu.fun("load", [&](std::string path){
		AudioWAV* w = new AudioWAV(path);
		return &audModu;
	});
	audModu.fun("play", [](std::string name) {
		if (!AudioWAV::audiofiles.contains(name)) {
			cwError::sstate(cwError::CW_ERROR);
			cwError::serrof("No audio found with the path %s! Playing anyways..", name.c_str());
			AudioWAV* w = new AudioWAV(name);
		}
		AudioWAV::audiofiles.at(name)->Play();
	});

	

	luaTransformCls.ctor<std::array<int,2>,int>();
	luaTransformCls.fun("getPosition", &luaTransform::getPosition);
	//luaTransformCls.def("position", { });
	//luaTransformCls.def("angle", &Transform::angle);
	//playerModu.def("transform", &playersprite->transform);
	luaPixbufCls.ctor<std::array<int, 2>>();
	luaPixbufCls.fun("drawLine", &luaPixbuf::luaDrawLine);
	luaPixbufCls.fun("drawRect", &luaPixbuf::luaDrawRect);
	luaPixbufCls.fun("drawPixel", &luaPixbuf::luaDrawPixel);
	luaPixbufCls.fun("height", &luaPixbuf::getHeight);
	luaPixbufCls.fun("width", &luaPixbuf::getWidth);
	luaPixbufCls.fun("resize", &luaPixbuf::resize);
	




	
	luaColHitCls.def("pos", &Raycast2D::hit.pos);

	global.fun("getSpriteById", [&](std::string id) -> Sprite* {
		Win->cons.pushf("Getting sprite at id %s", id.c_str());
		Sprite* _s = Sprite::getElementByID(id);
		if (_s == nullptr) {
			cwError::sstate(cwError::CW_ERROR);
			cwError::serrof("No sprite exists at id %s!", id.c_str());
			Sprite* _s_errorf = Sprite::getElementByID("_CW_ERROR_SPRITE");
			if (_s_errorf == nullptr) {
				_s_errorf = new Sprite();
				_s_errorf->setID("_CW_ERROR_SPRITE");
			}
			return _s_errorf;
		}

		Win->cons.pushf("Got sprite with id %s", id.c_str());
		return _s;
	});
	luaSpriteCls.ctor<const char*>();
	luaSpriteCls.fun("Move", &Sprite::Move);
	luaSpriteCls.fun("Rotate", &Sprite::Rotate);
	luaSpriteCls.fun("Scale", &Sprite::luaScale);
	luaSpriteCls.fun("setEnabled", &Sprite::setEnabled);
	luaSpriteCls.fun("setTransform", &Sprite::SetTransform);
	luaSpriteCls.fun("getPosition", &Sprite::luaGetPosition);
	luaSpriteCls.fun("getRotation", &Sprite::luaGetRotation);

	

	setGlobalLuaNum(state, "c_empty", COL_EMPTY);
	setGlobalLuaNum(state, "c_player", COL_PLAYER);
	setGlobalLuaNum(state, "c_entity", COL_ENT);
	setGlobalLuaNum(state, "c_object", COL_OBJ);
	setGlobalLuaNum(state, "c_trigger", COL_TRG);
	setGlobalLuaNum(state, "c_solid", COL_SOLID);
	global.fun("RaycastCircle", [&](std::array<int, 2> center, int diameter, int layer) {
		return Raycast2D::TestCircle({center[0], center[1]}, diameter, layer);
	});
	global.fun("RaycastLine", [&](std::array<std::array<int,2>,2> line, int layer) {
		return Raycast2D::Test({ line[0][0],line[0][1] }, { line[1][0],line[1][1] }, layer);
	});
	
	global.fun("print", [&](std::string txt) {
		Win->cons.pushuf(txt);
	});
	
	global.fun("print_n", [&](std::string txt, int amt) {
		for (int i = 0; i < amt; i++) {
			Win->cons.pushuf(txt);
		}
	});



	LuaClass<int*>(state, "int")
		.ctor<int*>("new")
		.def("type", std::string("[c int *]"))
		.def("max", INT_MAX)
		.def("min", INT_MIN);


});




#endif