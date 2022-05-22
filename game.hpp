#ifndef __cw_engine_game_handler_hpp
#define __cw_engine_game_handler_hpp


#define COUT_DEBUG
#define CWL_DEBUG
#include "engine.hpp"
#include "e_keybindings.h"
#define Win_width 720
#define Win_height 720
Window* Window::WindowInstance = new Window(::GetConsoleWindow(), false, "\0", Win_width, Win_height);;
Window* window = Window::WindowInstance;


#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This game requires SDL 2.0.17+ because of the SDL_RenderGeometry() function
#endif
#include "cw_lua_interp.hpp"
#include "lua.hpp"
bool debug_draw_line_bounds = false;
bool debug_draw_line_overlay = true;
/* Player initializer function  */
UI::TextElement fps_tx = UI::TextElement();
UI::ImageElement __img_el = UI::ImageElement("sprites/test.png");
// which seems to be caused by this ^
ImGui::Colour _c = ImGui::Colour(32, 32, 255);
const char* col = _c.tostring();
Window::Initializer __init_window_f{
	[]() {
		__img_el.enabled = true;
		__img_el.transform.position = {128,128};
		__img_el.transform.scale = {512,512};
		alib_assert_p(!&__lu_component_impl, "LUA Component Manager not initialized, unable to run!");
		Window::WindowInstance->AddComponent(&__lu_component_impl);
		alib_show_console();
		if (NDEBUG) { alib_hide_console(); }
		cwError::onError = [](const char* errv, uint32_t errs) {
			if (errs == cwError::CW_NONE) {
				Win->cons.pushf("%s\n", errv);
				return;
			}
			else {
				Win->cons.pushf("%s|%s\n", cwError::wtoh(errs), errv);
			}
		};


		Win->cons.commands.insert({ "cl_debug", [&](const char* _args) {
			std::vector<std::string> _argsv;
			alib_split_quoted(_args, &_argsv);
			if (_argsv.size() >= 1) {
				int state = atoi(_argsv.at(0).c_str());
				cwError::debug_enabled = state;
			}
			} 
		});
		Win->cons.commands.insert({ "r_line_bounds", [&](const char* _args) {
			std::vector<std::string> _argsv;
			alib_split_quoted(_args, &_argsv);
			if (_argsv.size() >= 1) {
				debug_draw_line_bounds = atoi(_argsv.at(0).c_str());
			}
			}
		});
		Win->cons.commands.insert({ "r_line_overlay", [&](const char* _args) {
			std::vector<std::string> _argsv;
			alib_split_quoted(_args, &_argsv);
			if (_argsv.size() >= 1) {
				debug_draw_line_overlay = atoi(_argsv.at(0).c_str());
			}
			}
		});
		Win->cons.commands.insert({ "cw", [&](const char* arg) {
			std::vector<std::string> argv{};
			alib_split_quoted(arg, & argv);
			size_t argc = argv.size();
			if (argc == 0) { return; }
			for (size_t i = 1; i < argc; i++) {
				// Win->cons.pushf("Arg: %s at %zi", argv[i].c_str(), i);
			}
			if (argv.at(0) == "unload") {
				if (SUCCEEDED(Win->scene)) {
					Win->scene->Discard();
					Win->cons.pushuf("Unloaded.");
				}
			}
			if (argv.at(0) == "load") {
				if (argc < 1) {
					Win->cons.pushuf("cw load requires a scene name!");
					return;
				}
				if (!alib_endswith(argv.at(1).c_str(), ".cwl")) {
					argv.at(1).append(".cwl");
				}
				if (alib_file_exists(argv.at(1).c_str())) {
					if (Win->scene != nullptr) {
						Win->scene->Discard();
					}
					Window::WindowInstance->scene = new cwLayout(argv.at(1).c_str());
				}
				else {
					Win->cons.pushuf("cw load requires a existing file!");
				}
			}
			if (argv.at(0) == "reload") {
				if (Win->scene == nullptr || !SUCCEEDED(Win->scene)) {
					return;
				}
				Win->cons.pushf("Reloading scene %s", Win->scene->name.c_str());
				std::string cwlSceneName = Win->scene->name;
				Win->scene->Discard();
				Win->scene = new cwLayout(cwlSceneName.c_str());
			}
			}
		});
		Win->cons.commands.insert({ "exec", [&](const char* arg) {
			// Condition to return if nothing is passed into exec, or if it runs twice. Which it does...
			if (std::string(arg).compare("") == 0) { return; }
			int status = luaL_dostring(state, arg);
			if (status) {
				// since the error message includes the script which we're trying
				// to parse, the result has unbounded length; so use another
				// ostringstream to hold the error message
				std::ostringstream errmsg;
				int parseError1_pos = lua_gettop(state);
				errmsg << "Errors parsing the following script:" << std::endl;
				errmsg << arg << std::endl << std::endl;
				errmsg << "Parser error when interpreting as an expression:" << std::endl;
				errmsg << lua_tostring(state, parseError1_pos) << std::endl << std::endl;
				Win->cons.pushuf(errmsg.str().c_str());
			}
		} });
		Win->cons.commands.insert({ "execf", [&](const char* arg) {
			if (!alib_file_exists(arg)) { cwError::sstate(cwError::CW_ERROR); cwError::serrof("File %s doesn't exist.", arg); return; }
			int status = luaL_dofile(state, arg);
			if (status) {
				std::ostringstream errmsg;
				int parseError1_pos = lua_gettop(state);
				errmsg << "Errors parsing the following file:" << std::endl;
				errmsg << arg << std::endl << std::endl;
				errmsg << "Parser error when interpreting as an expression:" << std::endl;
				errmsg << lua_tostring(state, parseError1_pos) << std::endl << std::endl;
				Win->cons.pushuf(errmsg.str().c_str());
			}
		} });
	}
};





#pragma endregion
#pragma region ObjectCode

Window::Component boxObject;
Window::Initializer _nn{
	[]() {
		boxObject.Update = [](){
			//world.Draw();
		};
		Win->AddComponent(&boxObject);
	}
};


#pragma endregion
#pragma region WindowScriptable



Window::Initializer _nn{
	[]() /*  Window functions  */ {
		/*
		* bit 1: debug
		* 
		* 
		* 
		*/
		Win->Start = []() {
			initDataLua(Win->SDL_REND, Win->SDL_WIND, &Time::DeltaTime, &Time::DeltaTimeScale, &Time::DeltaTimeUnscaled, &Time::fps);

			if (alib_file_exists("data/main.cwl")) {
				if (SUCCEEDED(Win->scene)) {
					Win->scene->Discard();
				}
				Window::WindowInstance->scene = new cwLayout("data/main.cwl");
			}
			else
				if (alib_file_exists("main.cwl")) {
					if (SUCCEEDED(Win->scene)) {
						Win->scene->Discard();
					}
					Window::WindowInstance->scene = new cwLayout("main.cwl");
				}
			if (Window::WindowInstance->scene->data.contains("autoexec")) {
				try {
					nlohmann::json _autoexec = Window::WindowInstance->scene->data.at("autoexec");
					std::string autoexec__str = alib_j_getstr(_autoexec);
					int status = luaL_dofile(state, autoexec__str.c_str());

					if (status) {
						// since the error message includes the script which we're trying
						// to parse, the result has unbounded length; so use another
						// ostringstream to hold the error message
						std::ostringstream errmsg;
						int parseError1_pos = lua_gettop(state);
						std::cout << "Errors parsing the autoexec script " << Window::WindowInstance->scene->data.at("autoexec").get<std::string>().c_str() << ":" << std::endl;
						std::cout << "Parser error when interpreting as an expression:" << std::endl;
						std::cout << lua_tostring(state, parseError1_pos) << std::endl << std::endl;
					}
				}
				catch (const std::exception& e) {
					printf("Autoexec failed %s\n", e.what());
				}
			}

		};
		Win->OnFocus = []() {};
		Win->OnFocusLost = []() {};
		Win->OnMaximize = []() {};
		Win->OnMinimize = []() {};
		Win->Update = [](){
			if (Win->keyboard.GetKeyDown(Input::K_PAUSE)) {
				Win->debug = !Win->debug;
			}
			if (!Win->hasFocus) {
				Sleep(30);
			}
			// Toggle fullbright
			if (Keyboard.GetKeyDown(Input::K_F)) {
				flipbitv(Win->data, 1);
			}
			// Toggle debug window
			if (Keyboard.GetKeyDown(Input::K_GRAVE) && Win->debug) {
				flipbitv(Win->data, 12);
			}
			// Toggle drawing lines
			if (Keyboard.GetKeyDown(Input::K_F1)) {
				flipbitv(Win->data, 2);
			}
		};
		Win->PostPreRender = [&](){
			fps_tx.textfmt("%s%.2f", col, Window::WindowInstance->time.fps);
			if (!getbitv(Win->data, 0)) {
				ImGui::SetWindowFocus(NULL);
				Win->debug_window_open = false;
			}
			fps_tx.enabled = Win->debug;
			if (Win->debug) {
				setbitv(Win->data, 0, 1);
				for (unsigned int o = 0; o < RectCollider2d::_mGlobalColArr.size(); o++) {
					RectCollider2d* c = RectCollider2d::_mGlobalColArr[o];
					SDL_SetRenderDrawColor(Win->SDL_REND, 128, 128, 128, 128);
					SDL_RenderFillRect(Win->SDL_REND, c->rect_cs);
				}
				for (size_t m = 0; m < MeshCollider2d::_mGlobalColArr.size(); m++) {
					alib_remove_any_of<MeshLine*>(MeshCollider2d::_mGlobalColArr[m]->lines, nullptr);
					for (size_t l = 0; l < MeshCollider2d::_mGlobalColArr[m]->lines.size(); l++) {
						MeshLine* line = MeshCollider2d::_mGlobalColArr[m]->lines[l];
						Vector2 s = line->start + -*Camera::GetInstance()->m_target;
						Vector2 e = line->end + -*Camera::GetInstance()->m_target;
						if (debug_draw_line_overlay) {
							SDL_SetRenderDrawColor(Win->SDL_REND, 0, 0, 255, 128);
							SDL_RenderDrawLine(Win->SDL_REND, s.x, s.y, e.x, e.y);
						}
						if (debug_draw_line_bounds) {
							SDL_Rect c_lr = MeshLine::bounding_box({ s,e });
							SDL_SetRenderDrawColor(__phys_internal_renderer, 255, 128, 128, 128);
							SDL_RenderDrawRect(Win->SDL_REND, &c_lr);
						}
					}
				}
			}
			else {
				setbitv(Win->data, 0, 0);
			}
		};
	}
};


#pragma endregion







#endif