#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP
#include "utils.hpp"
#if!defined(NDEBUG)
	#define NDEBUG
#endif
#ifndef noop
#define noop (void)__noop
#endif
#pragma warning (disable:4244)
#include <stdlib.h>
#undef APIENTRY
#include <gl/glfw3.h>
#undef APIENTRY
#include <Windows.h>
#include <sdl/SDL.h>
#include <chrono>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>
#include "renderer.hpp"
#include "Audio.hpp"
#undef main
typedef char int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
#include<stdarg.h>


#ifndef PHYSICS_HPP // Include physics.hpp if it hasn't already.
#include "physics.hpp"
#endif
#include "cwlib.hpp"


enum col_types {
	COL_WALL = 'WALL',
	COL_SOLID = COL_WALL,
	COL_PLAYER = 'PLR',
	COL_ENT = 'ENT',
	COL_TRG = 'TRG',
	COL_ALL = -1
};

struct Window {
private:
	SDL_Event sdlevent;
public:
	int argc;
	char** argv;
	static Window* WindowInstance;
	#pragma region Window::Structs
	std::function< void() > Start = [] {noop; };			// Code to run before first frame
	std::function< void() > PreUpdate = [] {noop; };		// Code to run before every frame
	std::function< void() > Update = [] {noop; };			// Code to run every frame
	std::function< void() > PostUpdate = [] {noop; };		// Code to run before every frame
	std::function< void() > PreRender = [] {noop; };		// Code to run just before rendering
	std::function< void() > PostPreRender = [] {noop; };	// Code to run just before rendering, but after all other functions have been run.
	std::function< void() > PostRender = [] {noop; };		// Code to run just after rendering
	std::function< void() > OnMinimize = [] {noop; };		// 
	std::function< void() > OnMaximize = [] {noop; };		// 
	std::function< void() > OnFocus = [] {noop; };			// 
	std::function< void() > OnFocusLost = [] {noop; };		// 
	struct Component {
		int32 data = INT32_MAX;
		std::function< void() > Start = [] {noop;};			// Code to run before first frame
		std::function< void() > PreUpdate = [] {noop; };		// Code to run before every frame
		std::function< void() > Update = [] {noop;};			// Code to run every frame
		std::function< void() > PostUpdate = [] {noop;};		// Code to run before every frame
		std::function< void() > PreRender = [] {noop;};		// Code to run just before rendering
		std::function< void() > PostRender = [] {noop;};		// Code to run just after rendering
		Component() {}
		Component(std::function<void()> initFunc) {
			initFunc();
			#if defined(DEBUG_COUT)
				std::cout << "\nInitialized initFunc with struct size: " << sizeof(this) << " and size of function: " << sizeof(initFunc) << '\n';
			#endif
			ZeroMemory(&initFunc, 0);
		}
	};
	struct Time {
		float TimeSinceStart = 0;
		float DeltaTime = 0;
		struct Timer {
		private: 
			std::chrono::steady_clock::time_point _start;
			std::chrono::steady_clock::time_point _end;
		public:
			float Elapsed;
			void Start() {
				_start = std::chrono::high_resolution_clock::now();
			}
			void End() {
				_end = std::chrono::high_resolution_clock::now();
				Elapsed = std::chrono::duration_cast<std::chrono::seconds>(_end - _start).count();
			}
		};
	};

	struct Initializer {
		Initializer(std::function<void()> initFunc) {  
			initFunc();
			#if defined(DEBUG_COUT)
				std::cout << "\nInitialized initFunc with struct size: " << sizeof(this) << " and size of function: " << sizeof(initFunc) << '\n';
			#endif
			ZeroMemory(&initFunc, 0);
			ZeroMemory(this, 0);
		}
	};
	struct Keyboard {
		char keys[1024];
		std::function <void(unsigned char, char*)> onKeyPress = [](  unsigned char,  char* = keys) {};
		std::function <void(unsigned char, char*)> onKeyRelease = [](unsigned char,  char* = keys) {};
		std::function <void(unsigned char, char*)> onKeyHold = [](   unsigned char,  char* = keys) {};

		void KeyboardDown(unsigned char key, SDL_Event ev) {
			keys[key] = true + true;
			onKeyPress(key, keys);
		}
		void KeyboardUp(unsigned char key, SDL_Event ev) {
			keys[key] = false;
			onKeyRelease(key, keys);
		}
		bool GetKey(int k) { // Is Key pressed?
			return keys[k];
		}
		bool GetKeyDown(int k) { // Key pressed this frame?
			return (keys[k] == true + true) ? true : false;
		}
	};
	struct Mouse {
		int x; int y;
		bool m1d;
		bool m2d;
	};
#pragma endregion
	#pragma region Window::Variables
	Mouse mouse;
	Keyboard keyboard;
	std::vector<Component*> components;
	void operator << (Component* o) { components.push_back(o); } // Add component to window. Used internally by AddComponent(Component* c)
	void operator >> (Component* o) { // Removes component. Used internally by RemoveComponent(Component* c)
		auto position = std::find(components.begin(), components.end(), o);
		if (position != components.end()) // Make sure it exists first!
			components.erase(position); // Remove component from window .
		components.shrink_to_fit();
	}
	// The above two operators can be used by end-user, though it's easiest to use AddComponent.
	// Otherwise, you would need to do *$win<<&(component), or *Window::GetInstance()<<&(component)
	bool running = true;
	private: bool started = false;
public:
	bool debug;
	Time time;
	std::vector<std::thread> workers;
	unsigned char* consolepx;
	HWND hwnd;
	HDC hdc;
	Vector2 size;
	Vector2 pos = Vector2(100,100);
	SDL_Window* SDL_WIND;
	SDL_Renderer* SDL_REND;
	SDL_GLContext SDL_GLCTX; 
	SDL_Texture* CSL_TEX;
	cwlScene* CUR_SCENE;
	long int data; // 8 byte wide data bit/byte storage for entire window. 
								 // Can be modified using setbit() and respective functions in utils.h
	bool fullscreen = false;
	Window(HWND console /* Text drawing base, unused. */, bool fullscreen /* Is game in fullscreen? */, const char* name, int width = 720, int height = 720) {
		WindowInstance = this;
		if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 8, 4096) == -1) {
			printf("Mix_OpenAudio: %s\n", Mix_GetError());
		}
		for (int i = 0; i < sizeof(keyboard.keys); i++) {
			keyboard.keys[i] = 0;
		}
		Mix_AllocateChannels(12);
		// Open device
		SDL_AudioSpec want, have;
		SDL_AudioDeviceID dev;

		SDL_memset(&want, 0, sizeof(want));
		want.freq = 48000;
		want.format = AUDIO_F32;
		want.channels = 8;
		want.samples = 4096;

		dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
		if (dev == 0) {
			std::cout << "Failed to open audio: " << SDL_GetError();
		}
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		}
		std::cout << SDL_GetError();
		int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		}
		#if defined (DEBUG_COUT)
			std::cout << "SDL_Image version : " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_PATCHLEVEL << std::endl;
		#endif


		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		hdc = GetDC(hwnd);
		SDL_WIND = SDL_CreateWindow(name, pos.x, pos.y, width, height, SDL_WINDOW_OPENGL);
		hwnd = ::GetActiveWindow();
		SDL_REND = SDL_CreateRenderer(SDL_WIND, -1, SDL_RENDERER_ACCELERATED);
		SDL_SetRenderDrawBlendMode(SDL_REND, SDL_BLENDMODE_BLEND);
		SDL_GLCTX = SDL_GL_CreateContext(SDL_WIND);
		
		initRenderer(SDL_REND, SDL_WIND);
		size = Vector2(width, height);
	};
	Vector2 center = Vector2{size.x /2, size.y/2};
#pragma endregion
	#pragma region Window::Funcs
	void StartInternal();
	void PreUpdateInternal();
	void UpdateInternal();
	void PostUpdateInternal();
	void PreRenderInternal();
	void PostRenderInternal();


	void RenderInternal() {
		BatchRenderer().Render();
		PostPreRender();
		SDL_RenderPresent(SDL_REND);		
	}
	void procEvent() {
		while (SDL_PollEvent(&sdlevent))
		{
			switch (sdlevent.type)
			{
			default: break;
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEMOTION:
				if (SDL_GetWindowFlags(SDL_WIND) & SDL_WINDOW_MOUSE_FOCUS) {
					mouse.m1d = false; mouse.m2d = false;
					if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
						mouse.m1d = true;
					}
					if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
						mouse.m2d = true;
					}
				}
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				this->OnFocusLost();
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				this->OnMaximize();
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				this->OnMinimize();
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
					this->OnFocus();
					break;
			case SDL_KEYDOWN:
				if (SDL_GetWindowFlags(SDL_WIND) & SDL_WINDOW_INPUT_FOCUS) {
					keyboard.KeyboardDown(sdlevent.key.keysym.scancode, sdlevent);
				}
				break;
			case SDL_KEYUP:
				if (SDL_GetWindowFlags(SDL_WIND) & SDL_WINDOW_INPUT_FOCUS) {
					keyboard.KeyboardUp(sdlevent.key.keysym.scancode, sdlevent);
				}
				break;


			case SDL_QUIT:
				running = false;
				break;
			}
		}
	}



	void IdleFuncInternal() {
		if (!started) {
			started = true;
			Start();
			for (unsigned int i = 0; i < components.size(); i++) {
				/*(components[i] == NULL) ? noop :*/components[i]->Start();
			}
		}
		auto start = std::chrono::high_resolution_clock::now();
		RECT rect;
		if (GetWindowRect(hwnd, &rect))
		{
			size.x= rect.right - rect.left;
			size.y= rect.bottom - rect.top;
		}
		

		SDL_GetMouseState(&mouse.x, &mouse.y);
		center = Vector2{ size.x / 2, size.y / 2 };
		SDL_SetRenderDrawColor(SDL_REND, 0, 0, 0, 255);
		SDL_RenderClear(SDL_REND);
		PreUpdateInternal();
		UpdateInternal();
		PostUpdateInternal();
		PreRenderInternal();
		RenderInternal();
		Camera::GetInstance()->Update();
		PostRenderInternal();
		for (int i = 0; i < sizeof(keyboard.keys); i++) {
			switch (keyboard.keys[i])
			{
			case true + true:
				keyboard.keys[i] = true;
				// Don't break, go to the next case. (:
				keyboard.onKeyHold(i, keyboard.keys);
				break;
			case true:
				keyboard.onKeyHold(i, keyboard.keys);
				break;
			default:
				break;
			}
		}
		auto end = std::chrono::high_resolution_clock::now();

		time.DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() * .00001f;
		
	}
	void AddComponent(Component* c) {
		*this << c;
	};
	void RemoveComponent(Component* c) {
		*this >> c;
	};
#pragma endregion
};


#pragma region WindowDefs
void Window::PreUpdateInternal() {
	PreUpdate();
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->PreUpdate();
	}

}
void Window::UpdateInternal() {
	Update();
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->Update();
	}
}
void Window::PostUpdateInternal() {
	PostUpdate();
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->PostUpdate();
	}
}
void Window::PreRenderInternal() {
	PreRender();
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->PreRender();
	}
}
void Window::PostRenderInternal() {
	PostRender();
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->PostRender();
	}
}
#pragma endregion
#define Keyboard Window::WindowInstance->keyboard
#define Mouse Window::WindowInstance->mouse
#define WindowRenderer Window::WindowInstance->SDL_REND
#define Time Window::WindowInstance->time
#endif