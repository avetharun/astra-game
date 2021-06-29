#pragma once
#ifndef PHYSICS_HPP
#include "physics.hpp"
#endif
#ifndef RENDERER_HPP
#include "renderer.hpp"
#endif
#ifndef UTILS_HPP
#include "utils.hpp"
#endif
#include <fstream>
void readFileToMem(const char* fname, char*& out) {

	std::ifstream file(fname, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		int size = file.tellg();
		out = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(out, size);
		file.close();
	}
}
void _cpyCharArr(char*& from, char*& to, int start, int end) {
	for (int iter = start; iter < end; ++iter) {to[iter] = from[iter];}
}



struct cwlScene {
private:
	char* fs;
	SDL_Surface* cwlSurf;
	static cwlScene* inst;
	std::map<int, std::function<int()>>::iterator ev_iterator;
	std::map<int, bool*>::iterator ev_trg_iterator;
	Vector2 _SIZE;
	char HEAD_SIZE;
	char VEC_SIZE = 2;
	__int64 HEAD_IMG_LOC_IDENTIFIER = 0x31CF0AAEFCE2AAC2;
public:
	std::map<int, std::function<int()>> events;
	std::map<int, bool*> event_triggers;
	std::map<short int, Sprite*> entities;
	MeshCollider2d* mesh;
private:
	void _cwlLoadF(const char*fn){readFileToMem(fn,fs);}
	Vector2 _cwlVectorPair(int off) {
		memcpy(&_SIZE.x, fs + off, VEC_SIZE);
		memcpy(&_SIZE.y, fs + off+VEC_SIZE, VEC_SIZE);
	}
	void _cwlLoadH() {
		HEAD_SIZE =  fs[0]	  ;// First byte of CWL file should be 0-255 as to note how big the header is (in bytes)
		VEC_SIZE  =  fs[0x04] ;
		char* header_data = new char[HEAD_SIZE+1];
		int HEAD_ITER = 1; // Start at 1.
		_cpyCharArr(fs, header_data, 0, HEAD_SIZE);
		if (strncmp(header_data, "CWL", 10)) {
			HEAD_ITER += 4; // Assume next byte after CWL prefix is the size of a single vector in bytes.
			std::cout << "CWLLib: Valid CWL prefix found.";
			
		}
	}
	MeshLine _cwlMeshLinePair(int at) {
		
	}
	void _cwlFree() {
		delete[] fs;
	}
	void _cwlLoad(const char*fn) {
		_cwlLoadF(fn);
		_cwlLoadH();
	}
public:
	cwlScene() {
		cwlScene::inst = this;
	};
	cwlScene(const char* c) {
		std::cout << "CWLLib: Loading scene " << c << std::endl;
		cwlScene::inst = this;
		_cwlLoad(c);
	};
	char* getHeader() {}
	Vector2 mapSize() {
		return _SIZE;
	}


	/** \brief Register scene event under name ev_strint (using char literal example: 'event_name') */
	void registerEvent(int ev_strint, std::function<int()> ev_func) {
		events.insert({ ev_strint,ev_func });
	}
	void registerEventTrigger(int ev_strint, bool* boolptr) {
		ev_iterator = events.find(ev_strint);
		if (ev_iterator == events.end()) {
			return;
		}
		event_triggers.insert({ev_strint, boolptr});
	}
	void callEvent(int ev_strint) {
		ev_iterator = events.find(ev_strint);
		if (ev_iterator != events.end()) {
			events[ev_strint]();
		}
	}
	void removeEvent(int ev_strint) {
		ev_iterator = events.find(ev_strint);
		if (ev_iterator != events.end()) {
			events.erase(ev_iterator);
		}
	}
	void StartInternal();
	void PreUpdateInternal();
	void UpdateInternal();
	void PostUpdateInternal();
	void PreRenderInternal();
	void PostRenderInternal();
	void OnLoadInternal();

	static cwlScene* Instance() { return inst = (inst != nullptr) ? inst : new cwlScene(); }
	static cwlScene* _I() { return inst = (inst != nullptr) ? inst : new cwlScene(); } // Shorthand for Instance()


};
typedef cwlScene CWLScene;
cwlScene* cwlScene::inst = nullptr;
#define $scene cwlScene::inst