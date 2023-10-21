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
typedef char int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
#include<stdarg.h>
#include <string.h> /* for strchr() */
#include "lua.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "input.h"

#include "global.h"


#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>


#include "GUIRenderer.h"
#include <queue>
#include "cwlib/cwlayout.hpp"
struct Window {
private:
	SDL_Event sdlevent;
public:
	/*
		0 : debug
		1 : fullbright
		2 : overlay
		3 : ui
		8 : is engine as editor
	*/
	unsigned long long data = B32(10010000, 00000000, 00000000, 00000000);
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
		long long data = B32(00000000,00000000,00000000,00000000);
		std::function< void() > Start = [] {noop;};			// Code to run before first frame
		std::function< void() > PreUpdate = [] {noop; };		// Code to run before every frame
		std::function< void() > Update = [] {noop;};			// Code to run every frame
		std::function< void() > PostUpdate = [] {noop;};		// Code to run before every frame
		std::function< void() > PreRender = [] {noop;};		// Code to run just before rendering
		std::function< void() > Render = [] {noop; };		// Code to run while rendering
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
	bool hasFocus;
	Input::Mouse mouse;
	Input::Keyboard keyboard;
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
	float border_size_x;
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

		int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		}
		dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
		if (dev == 0) {
			std::cout << "Failed to open audio: " << SDL_GetError();
		}
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		}
		SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		uint32_t __winflags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
		hdc = GetDC(hwnd);
		SDL_WIND = SDL_CreateWindow(name, pos.x, pos.y, width, height, (__winflags));
		hwnd = ::GetActiveWindow();
		SDL_REND = SDL_CreateRenderer(SDL_WIND, -1, SDL_RENDERER_ACCELERATED);

		SDL_SetRenderDrawBlendMode(SDL_REND, SDL_BLENDMODE_BLEND);
		SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_SCALING, "1");

		SDL_GLCTX = SDL_GL_CreateContext(SDL_WIND);
		initRenderer(SDL_REND, SDL_WIND);
		size = Vector2(width, height);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		CWLGlobals::DefaultFont = io.Fonts->AddFontDefault();

		// pack as many chars as we can into the symbols font
		auto _ranges = new ImWchar[2]{ 0x0020, 0xFFEF };
		size_t symbol_font_sz = alib_file_length("data/symbols.ttf");
		CWLGlobals::SymbolFontData = new char[symbol_font_sz + 1];
		alib_file_read("data/symbols.ttf", CWLGlobals::SymbolFontData, symbol_font_sz);
		CWLGlobals::SymbolFont = io.Fonts->AddFontFromMemoryTTF(CWLGlobals::SymbolFontData, symbol_font_sz, 13, nullptr, _ranges);
		io.Fonts->Build();
		
		ImGui_ImplSDL2_InitForSDLRenderer(SDL_WIND, SDL_REND);
		ImGui_ImplSDLRenderer_Init(SDL_REND);
		initRenderer__PHYS(SDL_REND);
		keyboard.flush();
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
		std::queue<std::string> commandStack{};
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
		void pushlnuf(std::string cstr) {
			cstr += "\n";
			if (&debugWindowConsoleText == nullptr) { return; }
			debugWindowConsoleText.append(cstr.c_str());
			m_scrollToBottom = 10;
		}
		void pushlnf(std::string fmt, ...) {
			if (&debugWindowConsoleText == nullptr) { return; }
			fmt += "\n";
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

				pushuf("\n" + std::string(_v));
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
	bool debug_window_open, debug_window_active = false;
	

	void DebugWindow() {
		ImGui::Begin("Debug Console");
		Input::Keyboard::input_stolen = ImGui::IsWindowHovered() || ImGui::IsWindowFocused();
		ImGui::BeginChildFrame(10, {0,ImGui::GetWindowHeight() - 75});
		if (cons.debugWindowConsoleText.size() > 2048) {
			std::string __tmp = cons.debugWindowConsoleText.end() - 2044;
			__tmp.assign(__tmp.c_str() + __tmp.find('\n'));
			cons.debugWindowConsoleText.clear();
			cons.debugWindowConsoleText.append(__tmp.c_str());
		}
		ImGui::TextMulticolored(cons.debugWindowConsoleText.begin());
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
			while (cons.commandStack.size() > 32) {
				cons.commandStack.pop();
			}
			cons.commandStack.push(inputCommand);
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
		debug_window_open = true;
		debug_window_active = ImGui::IsWindowFocused() || ImGui::IsWindowHovered();
		ImGui::Text("Application average %.2f ms/frame (%.1f FPS)", Time::DeltaTime, Time::fps);
		ImGui::End();
		
	}

	struct Postprocessing {
			

	};

	void RenderInternal() {
		float s = alib_min(this->size.x, this->size.y);
		float start_posx = (0.5f * this->size.x);
		Camera::GetInstance()->m_GlobalViewport = this->size;
		Camera::GetInstance()->m_Viewport = this->size;
		this->border_size_x = start_posx;
		BatchRenderer().Render();
		PostPreRender();

		if (scene) {
			for (int i = 0u; i < scene->scene_placed_items.size(); i++) {
				scene->scene_placed_items.at(i)->Update();
			}
		}
		if (scene && getbitv(data, 2/*overlay*/) && scene->ui_img_elem_assets.contains("overlay")) {
			UI::ImageElement * OVERLAY = scene->ui_img_elem_assets["overlay"];
			Camera::GetInstance()->culling = true;
			// left top
			ImGui::GetBackgroundDrawList()->AddRectFilled({ 0.0,0.0 }, { (float)OVERLAY->_dst.x, (float)this->size.y }, ImRGB(0,0,0), 0);
			ImGui::GetBackgroundDrawList()->AddRectFilled({ 0.0,0.0 }, { (float)this->size.x, (float)OVERLAY->_dst.y}, ImRGB(0, 0, 0), 0);
			// right bottom
			ImGui::GetBackgroundDrawList()->AddRectFilled({ (float)OVERLAY->_dst.x + (float)OVERLAY->_dst.w,0.0 }, { (float)this->size.x, (float)this->size.y }, ImRGB(0, 0, 0), 0);

			ImGui::GetBackgroundDrawList()->AddRectFilled({ 0.0,(float) (OVERLAY->_dst.y + OVERLAY->_dst.h)}, {(float)this->size.x, (float)this->size.y}, ImRGB(0, 0, 0), 0);


		}
		else {
			Camera::GetInstance()->culling = false;
		}
		UI::GUIRenderer::Render();

		for (unsigned int i = 0; i < components.size(); i++) {
			/*(components[i] == NULL) ? noop :*/components[i]->Render();
		}
			RectCollider2d* currect;
			RectCollider2d* noderect;
			for (int i = 0; i < RectCollider2d::_mGlobalColArr.size(); i++) {
				currect = RectCollider2d::_mGlobalColArr[i];
				SDL_Rect r1 = RectCollider2d::recalc(*currect);
				for (size_t m = 0; m < MeshCollider2d::_mGlobalColArr.size(); m++) {
					alib_remove_any_of<MeshLine*>(MeshCollider2d::_mGlobalColArr[m]->lines, nullptr);
					for (size_t l = 0; l < MeshCollider2d::_mGlobalColArr[m]->lines.size(); l++) {
						MeshLine* line = MeshCollider2d::_mGlobalColArr[m]->lines[l];
						Vector2 s = line->start + -*Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
						Vector2 e = line->end + -*Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
						int x1 = s.x; int y1 = s.y;
						int x2 = e.x; int y2 = e.y;
						if (SDL_IntersectRectAndLine(&r1, &x1, &y1, &x2, &y2) && !getbitv(this->data, 8)) {
							RectCollider2d::OnLineHitRect(currect, line);
						}
					}
				}
				for (int i1 = 0; i1 < RectCollider2d::_mGlobalColArr.size(); i1++) {
					noderect = RectCollider2d::_mGlobalColArr[i1];
					if (currect == noderect) {
						continue;
					}
					SDL_Rect r2 = RectCollider2d::recalc(*noderect);

					if (SDL_HasIntersection(&r1, &r2) && !getbitv(this->data, 8)) {
						RectCollider2d::OnColliderHit(currect, noderect);
					}
				}
			}
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		
		SDL_RenderPresent(SDL_REND);
	}
	void procEvent() {
		mouse.m1df = mouse.m2df = mouse.m3df = mouse.mt1df = mouse.mt2df = false;
		while (SDL_PollEvent(&sdlevent))
		{
			Camera* i = Camera::GetInstance();
			switch (sdlevent.type)
			{

			case SDL_KEYDOWN:
				Input::Keyboard::EmulateKeyboardDown(sdlevent.key.keysym.scancode, sdlevent);
				if (!Input::Keyboard::keys_frame_last[sdlevent.key.keysym.scancode]) {
					Input::Keyboard::keys_frame[sdlevent.key.keysym.scancode] = true;
				}
				break;
			case SDL_KEYUP:
				Input::Keyboard::EmulateKeyboardUp(sdlevent.key.keysym.scancode, sdlevent);
				Input::Keyboard::keys_frame[sdlevent.key.keysym.scancode] = false;
				break;

			default: break;
			case SDL_MOUSEBUTTONUP: {
				if (sdlevent.button.button & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					mouse.m1d = false;
				}
				if (sdlevent.button.button & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
					mouse.m2d = false;
				}
				if (sdlevent.button.button & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
					mouse.m3d = false;
				}
				if (sdlevent.button.button & SDL_BUTTON(SDL_BUTTON_X1)) {
					mouse.mt1d = false;
				}
				if (sdlevent.button.button & SDL_BUTTON(SDL_BUTTON_X2)) {
					mouse.mt2d = false;
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					if (!mouse.m1d) {
						mouse.m1df = true;
					}
					mouse.m1d = true;
				}
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
					if (!mouse.m2d) {
						mouse.m2df = true;
					}
					mouse.m2d = true;
				}
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
					if (!mouse.m2d) {
						mouse.m2df = true;
					}
					mouse.m3d = true;
				}
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_X1)) {
					if (!mouse.mt1d) {
						mouse.mt1df = true;
					}
					mouse.mt1d = true;
				}
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_X2)) {
					if (!mouse.mt2d) {
						mouse.mt2df = true;
					}
					mouse.mt2d = true;
				}
				mouse.onClick(mouse.x, mouse.y, mouse.m1d, mouse.m2d, mouse.m3d, mouse.mt1d, mouse.mt2d);
				break;
			} break;
			case SDL_MOUSEWHEEL: {
				mouse.sy = sdlevent.wheel.y; 
				mouse.onScroll(mouse.sd);
				break;
			}
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
			case SDL_WINDOWEVENT_RESIZED:
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

			case SDL_QUIT:
				running = false;
				break;
			}
			ImGui_ImplSDL2_ProcessEvent(&sdlevent);
		}
		
	}


	int m_dx, m_dy{ 0 };
	float m_ds = 0;
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
			m_dx = mouse.x;
			m_dy = mouse.y;
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

		mouse.dx = m_dx - mouse.x;
		mouse.dy = m_dy - mouse.y;
		mouse.sd = m_ds - mouse.sy;

		m_dx = mouse.x;
		m_dy = mouse.y;
		m_ds = mouse.sy;
		Time::DeltaTimeUnscaled = (double)ImGui::GetIO().DeltaTime * 1000;
		Time::DeltaTime = Time::DeltaTimeUnscaled * Time::DeltaTimeScale;
		Time::fps = ImGui::GetIO().Framerate;

	}
	void AddComponent(Component* c) {
		*this << c;
	};
	void RemoveComponent(Component* c) {
		*this >> c;
	};
};

#pragma region WindowDefs
void Window::StartInternal() {
	Start();
	alib_remove_any_of<Component*>(components, nullptr);
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->Start();
	}
}

void Window::PreUpdateInternal() {
	SDL_RenderClear(SDL_REND);

	PreUpdate();
	alib_remove_any_of<Component*>(components, nullptr);
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop : */components[i]->PreUpdate();
	}
}
void Window::UpdateInternal() {
	Update();
	alib_remove_any_of<Component*>(components, nullptr);
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->Update();
	}
	UI::GUIRenderer::RenderAnyUpdate();
}
void Window::PostUpdateInternal() {
	PostUpdate();
	alib_remove_any_of<Component*>(components, nullptr);
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->PostUpdate();
	}
}
void Window::PreRenderInternal() {
	PreRender();
	alib_remove_any_of<Component*>(components, nullptr);
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->PreRender();
	}
}
void Window::PostRenderInternal() {
	PostRender();
	alib_remove_any_of<Component*>(components, nullptr);
	for (unsigned int i = 0; i < components.size(); i++) {
		/*(components[i] == NULL) ? noop :*/components[i]->PostRender();
	}
}
#pragma endregion
#define WindowRenderer Window::WindowInstance->SDL_REND
#define Win Window::WindowInstance
#endif