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
int readFileToMem(const char* fname, char*& out) {

	std::ifstream file(fname, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		file.seekg(0, std::ios::end);
		int size = file.tellg();
		out = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(out, size);
		file.close();
		return size;
	}
	return 0;
}
void _cpyCharArr(char*& from, char*& to, int start, int end) {
	for (int iter = start; iter < end; ++iter) {to[iter] = from[iter];}
}



struct cwlScene {
private:
	int FS_SIZE;
	char* fs;
	SDL_Surface* cwlSurf;
	static cwlScene* inst;
	std::map<int, std::function<int()>>::iterator ev_iterator;
	std::map<int, bool*>::iterator ev_trg_iterator;
	Vector2 _SIZE;
	char HEAD_SIZE;
	char VEC_SIZE = 2;
	const char IMGF[8] = { 
		(char)0x31,
		(char)0xCF,
		(char)0x0A,
		(char)0xAE,
		(char)0xFC,
		(char)0xE2,
		(char)0xAA,
		(char)0xC2
	};
	const char ENDF[8] = { 
		(char)0x66,
		(char)0xAA,
		(char)0x3A,
		(char)0x35,
		(char)0x86,
		(char)0xED,
		(char)0xA2,
		(char)0xE2 
	};
	int IMG_OFFSET;
	int FEND;
public:
	std::map<int, std::function<int()>> events;
	std::map<int, bool*> event_triggers;
	std::map<short int, Sprite*> entities;
	MeshCollider2d* mesh;
private:
	void _cwlLoadF(const char*fn){ // Load CWL file into memory

		FS_SIZE = readFileToMem(fn,fs);
	}
	Vector2 _cwlVectorPair(int off) {
		Vector2 v;
		memcpy(&v.x, fs + off, VEC_SIZE);
		memcpy(&v.y, fs + off+VEC_SIZE, VEC_SIZE);
		return v;
	}
	void _cwlLoadH() {
		HEAD_SIZE =  fs[0]	  ;// First byte of CWL file should be 0-255 as to note how big the header is (in bytes)
		VEC_SIZE  =  fs[0x04] ;
		char* header_data = new char[HEAD_SIZE+1];
		int HEAD_ITER = 1; // Start at 1.
		_cpyCharArr(fs, header_data, 0, HEAD_SIZE);
		if (strncmp(header_data, "CWL", 10)) {
			HEAD_ITER += 4; // Assume next byte after CWL prefix is the size of a single vector in bytes. So we skip it.
			_SIZE = _cwlVectorPair(HEAD_ITER);
			for (int it = 10; it < FS_SIZE; it) {
				
				MeshLine l{ 
					_cwlVectorPair(it),
					_cwlVectorPair(it + VEC_SIZE) 
				};
				std::cout << "Created line at (from: " << l.start << " | to: " << l.end << ") at offset " << it<< $nl;
				mesh->lines.push_back(l);

				it += VEC_SIZE*4;
			}

		}
	}
	void _cwlFree() {
		delete[] fs;
	}
	void _cwlLoad(const char*fn) {
		mesh = new MeshCollider2d();
		mesh->layer = COL_WALL;
		_cwlLoadF(fn);
		_cwlLoadH();
	}
public:
	cwlScene() {
		cwlScene::inst = this;
	};
	cwlScene(const char* c) {
		std::cout << "CWLLib: Loading scene " << c << $nl;
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