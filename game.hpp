#pragma once


#define COUT_DEBUG
#define CWL_DEBUG
#include "engine.hpp"
#include "e_keybindings.h"
#define Win_width 720
#define Win_height 720
Window* Window::WindowInstance = new Window(::GetConsoleWindow(), false, "Astra", Win_width, Win_height);;
Window* window = Window::WindowInstance;


#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This game requires SDL 2.0.17+ because of the SDL_RenderGeometry() function
#endif
#include <luaaa.hpp>
#include "asl.hpp"

Sprite playerimg = Sprite("sprites.png");

Sprite OVERLAY_IMG = Sprite("sprites.png", SDL_Rect{ 0,91, 256,256});

RectCollider2d playerCollider  { playerimg.rect, COL_PLAYER };
Window::Component player;
const int uvpw = 24;
const int uvph = 42;
SDL_Rect playerAnimFrames [32] = {
	{ 16,1,			uvpw,uvph   }, // Facing camera
	{ 42,1,			uvpw,uvph   }, // Facing camera (walk 1)
	{ 16,1,			uvpw,uvph   }, // Facing camera (walk neutral)
	{ 66,1,			uvpw,uvph   }, // Facing camera (walk 2)
	{ 165,1,		uvpw,uvph   }, // Facing away 
	{ 191,1,		uvpw,uvph   }, // Facing away (walk 1)
	{ 165,1,		uvpw,uvph   }, // Facing away (walk neutral)
	{ 216,1,		uvpw,uvph	}, // Facing away (walk 2)
	{ 92,1,			uvpw,uvph   }, // Facing left
	{ 117,1,		uvpw,uvph	}, // Facing left (walk 1)
	{ 92,1,			uvpw,uvph   }, // Facing left (walk neutral)
	{ 141,1,		uvpw,uvph	}, // Facing left (walk 2)
	{ 92,45,	    uvpw,uvph   }, // Facing right
	{ 119,45,		uvpw,uvph	}, // Facing right (walk 1)
	{ 92,45,	    uvpw,uvph   }, // Facing right (walk neutral)
	{ 143,45,		uvpw,uvph	}  // Facing right (walk 2)
};
float char_anim_offset;


/* Player initializer function  */
Window::Initializer $un{
	[]() {
//		HideConsole();
		cwError::onError = [](const char* errv, uint32_t errs) {
			if (errs == cwError::CW_SILENT) {
				Win->cons.pushuf(errv);
				return;
			}
			Win->cons.pushf("%s|%s", cwError::wtoh(errs), errv);
		};
		/*
		
		if (HEAD_SIZE < 68) {
			const char* fmt = "CWL header must be at least 68 bytes long! Supplied: %d";
			size_t bufsz = snprintf(NULL, 0, fmt, HEAD_SIZE);
			char* buf = (char*)malloc(bufsz + 1);
			sprintf(buf, fmt, HEAD_SIZE);
			cwError::sstate(cwError::CW_ERROR);
			cwError::serror(buf);
			return 1;
		}
		legacy code
		
		*/
		player.Start = [](){
			playerimg.transform.position = Vector2(0,0);
			playerimg.layer = 128;
			playerimg.transform.scale = { uvpw * 2,uvph * 2 };
			playerimg.uv = playerAnimFrames[0];
			playerimg.lockCamera();
			playerimg.enabled = false;
			playerimg.center = true;

			initDataLua(&playerimg);

			/*
			* Component data values:
			* bit 0: can move
			* bit 1: draw?
			* bit 2: looking up (1 up | 0 down)
			* bit 3: looking left(1 left | 0 right)
			* bit 4: interacting with something
			* bit 5: closing interaction
			* bit 6: sprinting
			* bit 7: unused/padding
			* bit 8-12: player animation frame offset
			* bit 13: unused/padding
			* bit 14: walking?
			* bit 15: noclip enabled?
			*/
			player.data = INT64_MAX;
			setbitv(player.data, 15, 0);

			Win->cons.commands.insert({ "tp", [&](const char* _pos) noexcept {
				cwError::sstate(cwError::CW_ERROR);
				cwError::serror("The tp command is disabled until a problem when parsing input is fixed!!");
				return;

				int _seperator_pos = alib_getchrpos(_pos, ' ', alib_fnull(_pos));
				std::vector<std::string> _posv;
				alib_split_quoted(_pos, &_posv);
				if (_posv.size() < 2) {
					Win->cons.pushuf("This command requires 2 arguments");
					return;
				}
				std::string _pos1 = _posv.at(0);
				std::string _pos2 = _posv.at(1);
				int x,y;
				x = atoi(_pos1.c_str());
				y = atoi(_pos2.c_str());
				Win->cons.pushf("<%i> <%i>", x, y);
				playerimg.transform.position = { x,y };
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
					if (Win->scene == nullptr) { return; }
					Win->scene->Discard();
				}
				if (argv.at(0) == "load") {
					if (argc == 1) {
						Win->cons.pushuf("cw load requires a scene name!");
					}
					if (!alib_endswith(argv.at(1).c_str(), ".cwl")) {
						argv.at(1).append(".cwl");
					}
					if (alib_file_exists(argv.at(1).c_str())) {
						if (Win->scene != nullptr) {
							Win->scene->Discard();
						}
						Win->scene = new cwLayout(argv.at(1).c_str());
					}
					else {
						Win->cons.pushuf("cw load requires a existing file!");
					}
				}
				if (argv.at(0) == "reload") {
					std::string cwlSceneName = Win->scene->name;
					if (Win->scene != nullptr) {
						Win->scene->Discard();
					}
					Win->scene = new cwLayout(cwlSceneName.c_str());
					
				}
			  }
			});
			Win->cons.commands.insert({ "exec", [&](const char* arg) {
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
		};


		player.Update = [](){
			Keyboard.GetKey(Input::K_LSHIFT) ? setbitv(player.data, 6, 1) : setbitv(player.data, 6, 0);
			double speed = (getbitv(player.data, 6)) ? 0.3 : 0.2;
			Raycast2D Raycast = Raycast2D();
			if (getbitv(player.data, 0)) {

				if (!(
					Keyboard.GetKey(Input::K_s) &&
					Keyboard.GetKey(Input::K_w) &&
					Keyboard.GetKey(Input::K_a) &&
					Keyboard.GetKey(Input::K_d)
					))
				{
					if (!( window->keyboard.GetKey(Input::K_d) && window->keyboard.GetKey(Input::K_a) )) {
						if (window->keyboard.GetKey(Input::K_a)) {
							setbitv(player.data, 3, 1);
							playerimg.uv = playerAnimFrames[2 * 4 + 0];
							if (!Raycast.TestCone(playerimg.center_position + Vector2(0, 16), 40, 180, 25, COL_SOLID, 35) || getbitv(player.data, 15)) {
								playerimg.uv = playerAnimFrames[2 * 4 + (int)char_anim_offset];
								playerimg.transform.position.x -= (speed * Time.DeltaTime);
							}
						}
						if (window->keyboard.GetKey(Input::K_d)) {
							setbitv(player.data, 3, 0);
							playerimg.uv = playerAnimFrames[3 * 4 + 0];
							if (!Raycast.TestCone(playerimg.center_position + Vector2(0, 16), 40, 0, 25, COL_SOLID, 35) || getbitv(player.data, 15)) {
								playerimg.uv = playerAnimFrames[3 * 4 + (int)char_anim_offset];
								// We add 2 for... well a reason I have no idea about.
								playerimg.transform.position.x += (speed * Time.DeltaTime);
							}
						}
					}
					if (!(window->keyboard.GetKey(Input::K_s) && window->keyboard.GetKey(Input::K_w))) {
						if (window->keyboard.GetKey(Input::K_w)) {
							setbitv(player.data, 2, 0);
							playerimg.uv = playerAnimFrames[1 * 4 + 0];
							if (!Raycast.TestCone(playerimg.center_position + Vector2(0, 16), 40, 270, 25, COL_SOLID, 35) || getbitv(player.data, 15)) {
								playerimg.uv = playerAnimFrames[1 * 4 + (int)char_anim_offset];
								playerimg.transform.position.y -= (speed * Time.DeltaTime);
							}
						}
						if (window->keyboard.GetKey(Input::K_s)) {
							setbitv(player.data, 2, 1);
							playerimg.uv = playerAnimFrames[0 + 0];
							if (!Raycast.TestCone(playerimg.center_position + Vector2(0, 16), 45, 90, 25, COL_SOLID, 35) || getbitv(player.data, 15)) {
								playerimg.uv = playerAnimFrames[0 + (int)char_anim_offset];
								playerimg.transform.position.y += (speed * Time.DeltaTime);
							}
						}
					}


					if (window->keyboard.GetKeyDown(Input::K_q)) {
						Win->cons.pushf("Camera: (%i, %i), Player Pos: (%i, %i)",
							Camera::GetInstance()->m_target->x,
							Camera::GetInstance()->m_target->y,
							playerimg.transform.position.x,
							playerimg.transform.position.y
						);
					}
					if (window->keyboard.GetKeyDown(Input::K_e)) {
						setbitv(player.data, 5, 1);
					}
				}
			}
			playerimg.enabled = getbitv(player.data, 1);

			if ((
				!Keyboard.GetKey(Input::K_s) &&
				!Keyboard.GetKey(Input::K_w) &&
				!Keyboard.GetKey(Input::K_a) &&
				!Keyboard.GetKey(Input::K_d)
				))
			{
				// No movement keys pressed, reset animation offset.
				char_anim_offset = 0;
			}
			else {
				char_anim_offset += (Time.DeltaTime / 3.75f) * speed / 7;
				if (char_anim_offset > 4) { char_anim_offset = 0; }
			}



			// end of player update script
		};
		playerCollider.OnColliderHit = [&](RectCollider2d* col) {
			std::cout << "aaaaa";
		};
		window->AddComponent(&player);
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
			Camera::GetInstance()->SetTarget(&playerimg.transform.position); 
			OVERLAY_IMG.transform.scale = { Win_width, Win_height };
			OVERLAY_IMG.enabled = false;
			OVERLAY_IMG.manualDraw = true;
			OVERLAY_IMG.center = true;
			OVERLAY_IMG.transform.position = Vector2(-720, -720);
			OVERLAY_IMG.lockCamera();

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

			if (Win->debug_window_open) {
				setbitv(player.data, 0, 0);
				return;
			}
			else {
				setbitv(player.data, 0, 1);
			}
			// Toggle noclip
			if (Keyboard.GetKeyDown(Input::K_N)) {
				flipbitv(player.data, 15);
			}
			// Toggle fullbright
			if (Keyboard.GetKeyDown(Input::K_F)) {
				flipbitv(Win->data, 1);
			}
			// Toggle debug window
			if (Keyboard.GetKeyDown(Input::K_GRAVE)) {
				flipbitv(Win->data, 12);
			}
			// Toggle drawing lines
			if (Keyboard.GetKeyDown(Input::K_F1)) {
				flipbitv(Win->data, 2);
			}
		};
		Win->PostPreRender = [](){
			if (!getbitv(Win->data, 0)) {
				OVERLAY_IMG.Draw();
				ImGui::SetWindowFocus(NULL);
				Win->debug_window_open = false;
			}
			if (Win->debug) {
				setbitv(Win->data, 0, 1);
				// Draw debug lines?
				if (getbitv(Win->data, 2)) {

					for (unsigned int o = 0; o < RectCollider2d::_mGlobalColArr.size(); o++) {
						RectCollider2d* c = RectCollider2d::_mGlobalColArr[o];
						SDL_SetRenderDrawColor(Win->SDL_REND, 128, 35, 128, 32);
						SDL_RenderFillRect(Win->SDL_REND, c->rect);
					}
					for (size_t m = 0; m < MeshCollider2d::_mGlobalColArr.size(); m++) {
						for (size_t l = 0; l < MeshCollider2d::_mGlobalColArr[m]->lines.size(); l++) {
							MeshLine line = MeshCollider2d::_mGlobalColArr[m]->lines[l];
							Vector2 s = line.start + -*Camera::GetInstance()->m_target;
							Vector2 e = line.end + -*Camera::GetInstance()->m_target;
							SDL_SetRenderDrawColor(Win->SDL_REND, 0, 0, 255, 255);
							SDL_RenderDrawLine(Win->SDL_REND, s.x, s.y, e.x, e.y);
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




