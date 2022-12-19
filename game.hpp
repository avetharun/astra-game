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


#include "imgui/imgui_format.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This game requires SDL 2.0.17+ because of the SDL_RenderGeometry() function
#endif
#include "cw_lua_interp.hpp"
#include "lua.hpp"

#include "editor.h"

bool debug_draw_line_bounds = false;
bool debug_draw_line_overlay = true;
/* Player initializer function  */
UI::TextElement fps_tx = UI::TextElement();
// which seems to be caused by this ^
ImRGB _c = ImRGB(32, 32, 255);
std::string col = _c.tostring();
Window::Initializer __init_window_f{
	[]() {
		alib_assert_p(!&__lu_component_impl, "LUA Component Manager not initialized, unable to run!");
		Window::WindowInstance->AddComponent(&__lu_component_impl);
		SDL_MaximizeWindow(Window::WindowInstance->SDL_WIND);
		cwError::onError = [](const char* errv, uint32_t errs) {
			if (errs == cwError::CW_NONE) {
				Win->cons.pushf("%s\n", errv);
				return;
			}
			else {
				const char* errn = cwError::wtoh(errs);
				Win->cons.pushf("%s|%s\n", errn, errv);
			}
		};
		// init fonts




		Win->cons.commands.insert({ "cw_edit", [&](const char* _args) {
			std::vector<std::string> _argsv;
			alib_split_quoted(_args, &_argsv);
				if (_argsv.size() >= 1) {
					bool __state_old = getbitv(Window::WindowInstance->data, 8);
					bool __state = alib_atob(_argsv.at(0).c_str());
					setbitv(Window::WindowInstance->data, 8, __state);
					if (__state != __state_old) {
						if (__state) {
							SDL_SetWindowResizable(Window::WindowInstance->SDL_WIND, SDL_FALSE);
							SDL_MaximizeWindow(Window::WindowInstance->SDL_WIND);
							Window::WindowInstance->scene->editor_loaded = true;
						}
						else {
							Camera::GetInstance()->scale = 1.0f;
							SDL_SetWindowResizable(Window::WindowInstance->SDL_WIND, SDL_TRUE);
						}
					}
				}
			}
			});

		Win->cons.commands.insert({ "cw_save", [&](const char* _args) {
			if (Win->scene) {
				Window::WindowInstance->scene->Save();
				cwError::serrof("Saved!");
			}
			else {
				cwError::serrof("Unable to save, no scene loaded!");
			}
		} });
		Win->cons.commands.insert({ "cw_unload", [&](const char* _args) {
			if (Win->scene) {
				Window::WindowInstance->scene->Discard();
				Window::WindowInstance->scene = nullptr;
			}
		} });
		Win->cons.commands.insert({ "cw_load", [&](std::string args) {
				if (!alib_endswith(args.c_str(), ".cwl")) {
					args.append(".cwl");
				}
				if (alib_file_exists(("maps/" + args).c_str())) {
					args = "maps/"+args;
				}
				if (alib_file_exists(args.c_str())) {
					if (Win->scene != nullptr) {
						Win->scene->Discard();
					}
					Window::WindowInstance->scene = new cwLayout(args.c_str());
					Window::WindowInstance->scene->dtor = []() {__lu_on_interact_funcs.clear(); };
				}
				else {
					Win->cons.pushuf("this requires an existing file!");
				}
			} });

		Win->cons.commands.insert({ "cw_reload", [&](std::string args) {
			if (Win->scene) {
				Win->cons.pushf("Reloading scene %s", Win->scene->name.c_str());
				std::string cwlSceneName = Win->scene->name;
				Win->scene->Discard();
				Win->scene = new cwLayout(cwlSceneName.c_str());
				Window::WindowInstance->scene->dtor = []() {__lu_on_interact_funcs.clear(); };
			}
			else {
				Win->cons.pushf("No scene loaded. Load a scene with cw_load <name>");
			}
		} });

		Win->cons.commands.insert({ "cl_debug", [&](const char* _args) {
			std::vector<std::string> _argsv;
			alib_split_quoted(_args, &_argsv);
			if (_argsv.size() >= 1) {
				int state = atoi(_argsv.at(0).c_str());
				cwError::debug_enabled = state;
			}
			} 
		});
		Win->cons.commands.insert({ "cl_pos", [&](const char* _args) {
			Win->cons.pushlnf("Player position: %.2f, %.2f", Camera::GetInstance()->m_target->x, Camera::GetInstance()->m_target->y);
		} });
		Win->cons.commands.insert({ "r_fullbright", [&](const char* _args) {
			std::vector<std::string> _argsv;
			alib_split_quoted(_args, &_argsv);
				if (_argsv.size() >= 1) {
					setbitv(Window::WindowInstance->data, 1, alib_atob(_argsv.at(0).c_str()));
				}
			}
		});
		Win->cons.commands.insert({ "r_overlay", [&](const char* _args) {
			std::vector<std::string> _argsv;
			alib_split_quoted(_args, &_argsv);
				if (_argsv.size() >= 1) {
					setbitv(Window::WindowInstance->data, 2, alib_atob(_argsv.at(0).c_str()));
				}
			}
		});
		Win->cons.commands.insert({ "r_debug", [&](const char* _args) {
			std::vector<std::string> _argsv;
			alib_split_quoted(_args, &_argsv);
				if (_argsv.size() >= 1) {
					__phys_is_debug = alib_atob(_argsv.at(0).c_str());
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

			int status_global = luaL_dofile(state, "lib/lua/global.lua");
			if (status_global) {
				int parseError1_pos = lua_gettop(state);
				std::cout << "Errors parsing the global autoexec script " << std::endl;
				std::cout << "Parser error when interpreting as an expression:" << std::endl;
				std::cout << lua_tostring(state, parseError1_pos) << std::endl << std::endl;
			}
			if (Window::WindowInstance->scene->data.contains("autoexec")) {
				printf("starting autoexec\n");
				try {
					nlohmann::json _autoexec = Window::WindowInstance->scene->data.at("autoexec");
					std::string autoexec__str = alib_j_getstr(_autoexec);
					int status = luaL_dofile(state, autoexec__str.c_str());

					if (status) {
						// since the error message includes the script which we're trying
						// to parse, the result has unbounded length; so use another
						// ostringstream to hold the error message
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
		Win->Update = []() {
			if (!Win->hasFocus) {
				Sleep(30);
			}
			if (Window::WindowInstance->scene) {
				if (Window::WindowInstance->scene->ui_img_elem_assets.contains("overlay")) {
					Win->scene->ui_img_elem_assets.at("overlay")->enabled = getbitv(Window::WindowInstance->data, 2);
				}
			}
		};
		Win->PostPreRender = [&]() {
			fps_tx.textfmt("%s%.2f", col.c_str(), Window::WindowInstance->time.fps);
			if (!getbitv(Win->data, 0)) {
				ImGui::SetWindowFocus(NULL);
				Win->debug_window_open = false;
				lu_SDL_Window_impl::__INSTANCE->debug_window_open = false;
				lu_SDL_Window_impl::__INSTANCE->debug_window_active = false;
			}
			fps_tx.enabled = Win->debug;

			if (Win->debug) {
				setbitv(Win->data, 0, 1);
				if (__phys_is_debug || getbitv(Window::WindowInstance->data,8)) {

					for (unsigned int o = 0; o < RectCollider2d::_mGlobalColArr.size(); o++) {
						RectCollider2d* c = RectCollider2d::_mGlobalColArr[o];
						SDL_Rect r = RectCollider2d::recalc(*c);
						SDL_SetRenderDrawColor(Win->SDL_REND, 128, 128, 128, 128);
						SDL_RenderFillRect(Win->SDL_REND, &r);
						ImGui::TextBackground(alib_strfmt("rect %d", o), { (float)r.x + (r.w * 0.5f), (float)r.y + (r.h * 0.5f) });
					}
					for (size_t m = 0; m < MeshCollider2d::_mGlobalColArr.size(); m++) {
						alib_remove_any_of<MeshLine*>(MeshCollider2d::_mGlobalColArr[m]->lines, nullptr);
						for (size_t l = 0; l < MeshCollider2d::_mGlobalColArr[m]->lines.size(); l++) {
							MeshLine* line = MeshCollider2d::_mGlobalColArr[m]->lines[l];
							Vector2 s = line->start + -*Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
							Vector2 e = line->end + -*Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
							SDL_SetRenderDrawColor(Win->SDL_REND, 0, 0, 255, 128);
							SDL_RenderDrawLine(Win->SDL_REND, s.x, s.y, e.x, e.y);
							// Only render line bounds if we're not in the editor- It's for debugging purposes!
							SDL_Rect c_lr = MeshLine::bounding_box({ s,e });
							SDL_SetRenderDrawColor(__phys_internal_renderer, 255, 128, 128, 128);
							SDL_RenderDrawRect(Win->SDL_REND, &c_lr);
							Vector2 mid = MeshLine::midpoint(s,e);
							ImGui::TextBackground(alib_strfmt("line %d", l), {(float)mid.x, (float)mid.y});
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