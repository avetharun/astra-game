#ifndef ENGINE_HPP
#define ENGINE_HPP
#include "utils.hpp"
#ifndef noop
#define noop (void)__noop
#endif
#pragma warning (disable:4244)
#include <stdlib.h>
#undef APIENTRY
#include <Windows.h>
#include <SDL2/SDL.h>
#include <chrono>
#include <vector>
#include <thread>
#include <functional>
#include "renderer.hpp"
#include "Audio.hpp"
#undef main
typedef char int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
#include<stdarg.h>
#include <string.h> /* for strchr() */

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

#include "cwlib/cwlib.hpp"
#include "e_keybindings.h"


#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>


#include "GUIRenderer.h"
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
		std::map<std::string, int> conds;
		double data = B32(00000000,00000000,00000000,00000000);
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
		char keys[1024]{};
		char key_frame[1024]{};
		std::function <void(unsigned char)> onKeyPress = [](  unsigned char) {};
		std::function <void(unsigned char)> onKeyRelease = [](unsigned char) {};
		std::function <void(unsigned char)> onKeyHold = [](   unsigned char) {};

		void EmulateKeyboardDown(unsigned char key, SDL_Event ev) {
			keys[key] = true;
			if (!key_frame[key]) {
				key_frame[key] = true;
				onKeyPress(key);
				return;
			}
			onKeyHold(key);
		}
		void EmulateKeyboardUp(unsigned char key, SDL_Event ev) {
			keys[key] = false;
			key_frame[key] = false;
			onKeyRelease(key);
		}
		bool GetKey(int k) { // Is Key pressed?
			return keys[k];
		}
		bool GetKeyDown(int k) { // Key pressed this frame?
			return key_frame[k];
		}
		void flush() {
			memset(keys, 0, 1024);
		}
	};
	struct Mouse {
		int x; int y;
		bool m1d, m2d, m3d, mt1d, mt2d;
		
		std::function <void(int, int, bool, bool, bool, bool, bool)> onClick = [](int x, int y, bool l, bool r, bool m, bool t1, bool t2) {};
		std::function <void(int, int)> onMove = [](int x, int y) {};
	};
	bool hasFocus;
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
	// Otherwise, you would need to do *Win<<&(component), or *Window::GetInstance()<<&(component)
	bool running = true;
	private: bool started = false;
public:
	bool debug = false;
	Time time;
	std::vector<std::thread> workers;
	unsigned char* consolepx;
	ImGuiIO io;
	HWND hwnd;
	HDC hdc;
	Vector2 size;
	Vector2 pos = Vector2(100,100);
	SDL_Window* SDL_WIND;
	SDL_Renderer* SDL_REND;
	SDL_GLContext SDL_GLCTX; 
	SDL_Texture* CSL_TEX;
	unsigned long long data = 0; // 8 byte wide data bit/byte storage for entire window. 
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

		SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		
		uint32_t __winflags = SDL_RENDERER_PRESENTVSYNC | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
		hdc = GetDC(hwnd);
		SDL_WIND = SDL_CreateWindow(name, pos.x, pos.y, width, height, (__winflags));
		hwnd = ::GetActiveWindow();
		SDL_REND = SDL_CreateRenderer(SDL_WIND, -1, SDL_RENDERER_ACCELERATED);
		SDL_SetRenderDrawBlendMode(SDL_REND, SDL_BLENDMODE_BLEND);
		SDL_GLCTX = SDL_GL_CreateContext(SDL_WIND);
		
		initRenderer(SDL_REND, SDL_WIND);
		size = Vector2(width, height);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForSDLRenderer(SDL_WIND);
		ImGui_ImplSDLRenderer_Init(SDL_REND);
		initRenderer__PHYS(SDL_REND);
		keyboard.flush();
		SDL_Texture* texture = SDL_CreateTexture(SDL_REND, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
		//HideConsole();

	};
	Vector2 center = Vector2{size.x /2, size.y/2};
	void StartInternal();
	void PreUpdateInternal();
	void UpdateInternal();
	void PostUpdateInternal();
	void PreRenderInternal();
	void PostRenderInternal();




	CWLScene* scene;
	struct Console {
		char debugWindowInput[512]{};
		ImGuiTextBuffer debugWindowConsoleText;
		int m_scrollToBottom = false;
		void pushuf(std::string cstr) {
			if (&debugWindowConsoleText == nullptr) { return; }
			debugWindowConsoleText.append(cstr.c_str());
			m_scrollToBottom = 10;
		}
		void pushf(std::string fmt, ...) {
			if (&debugWindowConsoleText == nullptr) { return; }
			va_list args;
			va_start(args, fmt);
			debugWindowConsoleText.appendfv(fmt.c_str(), args);
			va_end(args);
			m_scrollToBottom = 10;

		}


		std::map < std::string, std::function<void(const char*)> > commands{
			{"echo",[&](const char* _v) {
				if (_v[0] == '\0') {
					return;
				}
				pushuf(_v); 
			} },
			{"help", [&](const char* _v) {
				if (_v[0] == '\0') {
					pushuf("Please input a valid command!");
				}
			}},
			{"clear", [&](const char* unused) {
				debugWindowConsoleText.clear();
			}}
		};


		void handlecmdf(std::string cmd, char* _cmd_args) {
			if (commands.find(cmd) == commands.end()) {
				pushf("Command %s not found!", cmd.c_str());
				return;
			}
			commands.at(cmd)(_cmd_args);
		}
	};
	Console cons{};
	bool debug_window_open = false;

	void DebugWindow() {
		ImGui::Begin("Debug Console");
		ImGui::BeginChildFrame(10, {0,ImGui::GetWindowHeight() - 75});
		if (cons.debugWindowConsoleText.size() > 2048) {
			std::string __tmp = cons.debugWindowConsoleText.end() - 2044;
			__tmp.assign(__tmp.c_str() + __tmp.find('\n'));
			cons.debugWindowConsoleText.clear();
			cons.debugWindowConsoleText.append(__tmp.c_str());
		}
		ImGui::TextColouredFormatted(cons.debugWindowConsoleText.begin());
		if (cons.m_scrollToBottom && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
			ImGui::SetScrollHereY(1.0f);
			cons.m_scrollToBottom--;
		}

		ImGui::EndChildFrame();
		if (ImGui::IsAnyItemFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			ImGui::SetKeyboardFocusHere(0);
		bool _isEnterPressed = ImGui::InputText("##consInput", cons.debugWindowInput, 512, ImGuiInputTextFlags_EnterReturnsTrue);
		if (_isEnterPressed) {
			std::string inputCommand(cons.debugWindowInput);
			// truncate command from args
			std::string::size_type cmdend = inputCommand.find(' ');
			if (cmdend != std::string::npos) {
				inputCommand = inputCommand.substr(0, cmdend);
				cons.handlecmdf(inputCommand, cons.debugWindowInput + cmdend + 1);
			}
			else {
				cons.handlecmdf(inputCommand, (char*)"\0");
			}
			// resets input text
			cons.debugWindowInput[0] = {0}; // generates a call to memset(debugWindowInput[0], '\0', 8)
		}
		debug_window_open = ImGui::IsWindowFocused();
		ImGui::Text("Application average %.2f ms/frame (%.1f FPS)", Time::DeltaTime, Time::fps);
		ImGui::End();
		
	}

	struct Postprocessing {
			

	};

	void RenderInternal() {
		BatchRenderer().Render();
		PostPreRender();
		UI::GUIRenderer::Render();
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(SDL_REND);
	}
	void procEvent() {
		while (SDL_PollEvent(&sdlevent))
		{
			ImGui_ImplSDL2_ProcessEvent(&sdlevent);
			switch (sdlevent.type)
			{
			default: break;
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN: {

				mouse.m1d = false; mouse.m2d = false; mouse.m3d = false; mouse.mt1d = false; mouse.mt2d = false;
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					mouse.m1d = true;
				}
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
					mouse.m2d = true;
				}
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
					mouse.m3d = true;
				}
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_X1)) {
					mouse.mt1d = true;
				}
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_X2)) {
					mouse.mt2d = true;
				}
				mouse.onClick(mouse.x, mouse.y, mouse.m1d, mouse.m2d, mouse.m3d, mouse.mt1d, mouse.mt2d);
			} break;
			case SDL_MOUSEMOTION: {
				SDL_GetMouseState(&mouse.x, &mouse.y);
				mouse.onMove(mouse.x, mouse.y);
			} break;

			case SDL_WINDOWEVENT_FOCUS_LOST:
				this->OnFocusLost();
				this->hasFocus = false;
				this->keyboard.flush();
				std::cout << "window focus lost \n";
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				this->OnMaximize();
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				this->OnMinimize();
				break;
			case SDL_WINDOWEVENT_MOVED: 
				// Flush keyboard. This is so when moving, if a key is held, it won't teleport!
				this->keyboard.flush();
				std::cout << "Moved window\n";
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				this->OnFocus();
				this->hasFocus = true;
				break;
			case SDL_KEYDOWN:
				if (SDL_GetWindowFlags(SDL_WIND) & SDL_WINDOW_INPUT_FOCUS) {
					keyboard.EmulateKeyboardDown(sdlevent.key.keysym.scancode, sdlevent);
				}
				break;
			case SDL_KEYUP:
				if (SDL_GetWindowFlags(SDL_WIND) & SDL_WINDOW_INPUT_FOCUS) {
					keyboard.EmulateKeyboardUp(sdlevent.key.keysym.scancode, sdlevent);
				}
				break;


			case SDL_QUIT:
				running = false;
				break;
			}
		}
	}



	void IdleFuncInternal() {
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame(Window::WindowInstance->SDL_WIND);
		ImGui::NewFrame();
		SDL_SetRenderDrawColor(SDL_REND, 0, 0, 0, 255);
		// Fullbright enabled?
		if (getbitv(data, 1)) {
			SDL_SetRenderDrawColor(SDL_REND, 128, 128, 128, 255);
		}
		SDL_RenderClear(SDL_REND);
		if (!started) {
			started = true;
			Start();
			for (unsigned int i = 0; i < components.size(); i++) {
				/*(components[i] == NULL) ? noop :*/components[i]->Start();
			}
		}
		RECT rect;
		if (GetWindowRect(hwnd, &rect))
		{
			size.x= rect.right - rect.left;
			size.y= rect.bottom - rect.top;
		}
		

		SDL_GetMouseState(&mouse.x, &mouse.y);
		center = Vector2{ size.x / 2, size.y / 2 };



		PreUpdateInternal();
		UpdateInternal();
		PostUpdateInternal();
		PreRenderInternal();

		if (debug_window_open) {
			DebugWindow();
		}

		RenderInternal();
		Camera::GetInstance()->Update();
		PostRenderInternal();
		Time::DeltaTimeUnscaled = (double)ImGui::GetIO().DeltaTime * 1000;
		Time::DeltaTime = Time::DeltaTimeUnscaled * Time::DeltaTimeScale;
		Time::fps = ImGui::GetIO().Framerate;
		for (int i = 0; i < 1024; i++) {
			if (keyboard.key_frame[i]) {
				keyboard.key_frame[i] = false;
				continue;
			}
		}

	}
	void AddComponent(Component* c) {
		*this << c;
	};
	void RemoveComponent(Component* c) {
		*this >> c;
	};
};

void recalc_rect_positions() {
	for (int i = 0; i < RectCollider2d::_mGlobalColArr.size(); i++) {
		RectCollider2d* m = RectCollider2d::_mGlobalColArr[i];
		if (!SUCCEEDED(m)) { continue; }
		if (m->layer & COL_PLAYER) {
			continue;
		}
		m->rect_cs->x = m->rect->x + -Camera::GetInstance()->m_target->x;
		m->rect_cs->y = m->rect->y + -Camera::GetInstance()->m_target->y;
	}
}
#pragma region WindowDefs
void Window::PreUpdateInternal() {

	PreUpdate();
	for (unsigned int i = 0; i < components.size(); i++) {
		recalc_rect_positions();
		/*(components[i] == NULL) ? noop :*/components[i]->PreUpdate();
	}
}
void Window::UpdateInternal() {
	Update();
	for (unsigned int i = 0; i < components.size(); i++) {
		recalc_rect_positions();
		/*(components[i] == NULL) ? noop :*/components[i]->Update();
	}
}
void Window::PostUpdateInternal() {
	PostUpdate();
	for (unsigned int i = 0; i < components.size(); i++) {
		recalc_rect_positions();
		/*(components[i] == NULL) ? noop :*/components[i]->PostUpdate();
	}
}
void Window::PreRenderInternal() {
	PreRender();
	for (unsigned int i = 0; i < components.size(); i++) {
		recalc_rect_positions();
		/*(components[i] == NULL) ? noop :*/components[i]->PreRender();
	}
}
void Window::PostRenderInternal() {
	PostRender();
	for (unsigned int i = 0; i < components.size(); i++) {
		recalc_rect_positions();
		/*(components[i] == NULL) ? noop :*/components[i]->PostRender();
	}
}
#include "cwlib/cwlib.hpp"
#pragma endregion
#define Keyboard Window::WindowInstance->keyboard
#define Mouse Window::WindowInstance->mouse
#define WindowRenderer Window::WindowInstance->SDL_REND
#define Win Window::WindowInstance
#endif