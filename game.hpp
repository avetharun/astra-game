#pragma once
#define COUT_DEBUG
#include "engine.hpp"
#define $win_width 720
#define $win_height 720
Window* Window::WindowInstance = new Window(::GetConsoleWindow(), false, "Astra", $win_width, $win_height);;
Window* window = Window::WindowInstance;
CWLScene* scene = new CWLScene("scene.cwl");

#pragma region PlayerCode


Sprite playerimg = Sprite("sprites.png");
Sprite OVERLAY_IMG = Sprite("sprites.png", SDL_Rect{ 0,91, 256,256});
Sprite box = Sprite("sprites.png", SDL_Rect{257,92, 26,26});
Sprite world = Sprite("world.png");

Collider2d boxRect{ box.rect , COL_WALL };

Collider2d dbgOverlayRect{ OVERLAY_IMG.rect , COL_WALL };
Collider2d playerCollider  { playerimg.rect, COL_PLAYER };
Window::Component player;
AudioWAV interact = AudioWAV("audio/interact.wav");
const int pw = 24;
const int ph = 42;
SDL_Rect playerAnimFrames [32] = {
	{ 16,1,			pw,ph   }, // Facing camera
	{ 42,1,			pw,ph   }, // Facing camera (walk 1)
	{ 16,1,			pw,ph   }, // Facing camera (walk neutral)
	{ 66,1,			pw,ph   }, // Facing camera (walk 2)
	{ 165,1,		pw,ph   }, // Facing away 
	{ 191,1,		pw,ph   }, // Facing away (walk 1)
	{ 165,1,		pw,ph   }, // Facing away (walk neutral)
	{ 216,1,		pw,ph	}, // Facing away (walk 2)
	{ 92,1,			pw,ph   }, // Facing left
	{ 117,1,		pw,ph	}, // Facing left (walk 1)
	{ 92,1,			pw,ph   }, // Facing left (walk neutral)
	{ 141,1,		pw,ph	}, // Facing left (walk 2)
	{ 92,45,	    pw,ph   }, // Facing right
	{ 119,45,		pw,ph	}, // Facing right (walk 1)
	{ 92,45,	    pw,ph   }, // Facing right (walk neutral)
	{ 143,45,		pw,ph	}  // Facing right (walk 2)
};
float char_anim_offset;
/* Player initializer function  */
Window::Initializer $un{
	$f {



		player.Start = $f{
			playerimg.transform.position = Vector2(400,400);
			playerimg.layer = 128;
			playerimg.size = Vector2( pw * 2,ph * 2);
			playerimg.uv = playerAnimFrames[0];
			playerimg.lockCamera();
			playerimg.enabled = false;
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
			*/
			setbitbulk(player.data, 2, 13, 0);
		};
		player.Update = $f{
			Keyboard.GetKey(Input::K_LSHIFT) ? setbit(player.data, 6, 1) : setbit(player.data, 6, 0);
			int speed = (getbit(player.data, 6)) ? 15 : 7;
			Raycast2D Raycast = Raycast2D();
			if (getbit(player.data, 0)) {

				if (!(
					Keyboard.GetKey(Input::K_s) &&
					Keyboard.GetKey(Input::K_w) &&
					Keyboard.GetKey(Input::K_a) &&
					Keyboard.GetKey(Input::K_d)
					))
				{
					if (!( window->keyboard.GetKey(Input::K_d) && window->keyboard.GetKey(Input::K_a) )) {
						if (window->keyboard.GetKey(Input::K_a)) {
							setbit(player.data, 3, 1);
							if (!Raycast.TestCone(playerimg.center_position + Vector2(0, 16), 40, 180, 25, COL_WALL, 35)) {
								playerimg.uv = playerAnimFrames[2 * 4 + (int)char_anim_offset];
								playerimg.transform.position.x -= speed * Time.DeltaTime;
							}
						}
						if (window->keyboard.GetKey(Input::K_d)) {
							setbit(player.data, 3, 0);
							if (!Raycast.TestCone(playerimg.center_position + Vector2(0, 16), 40, 0, 25, COL_WALL, 35)) {
								playerimg.uv = playerAnimFrames[3 * 4 + (int)char_anim_offset];
								// We add 2 for... well a reason I have no idea about.
								playerimg.transform.position.x += (speed+2) * Time.DeltaTime;
							}
						}
					}
					if (!(window->keyboard.GetKey(Input::K_s) && window->keyboard.GetKey(Input::K_w))) {
						if (window->keyboard.GetKey(Input::K_w)) {
							setbit(player.data, 3, 0);
							if (!Raycast.TestCone(playerimg.center_position + Vector2(0, 16), 40, 270, 25, COL_WALL, 35)) {
								playerimg.uv = playerAnimFrames[1 * 4 + (int)char_anim_offset];
								playerimg.transform.position.y -= speed * Time.DeltaTime;
							}
						}
						if (window->keyboard.GetKey(Input::K_s)) {
							setbit(player.data, 3, 0);
							if (!Raycast.TestCone(playerimg.center_position + Vector2(0, 16), 45, 90, 25, COL_WALL, 35)) {
								playerimg.uv = playerAnimFrames[0 + (int)char_anim_offset];
								playerimg.transform.position.y += (speed + 2) * Time.DeltaTime;
							}
						}
					}

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


					if (window->keyboard.GetKeyDown(Input::K_q)) {
						setbit(player.data, 4, 1);
						std::cout << "Camera position: " << *Camera::GetInstance()->m_target << "  |  Player position: " << playerimg.transform.position << '\n';
					}
					if (window->keyboard.GetKeyDown(Input::K_e)) {
						setbit(player.data, 5, 1);
					}
				}
			}
			playerimg.enabled = getbit(player.data, 1);




			// end of player update script
		};
		playerCollider.OnColliderHit = [&](Collider2d* col) {

		};
		window->AddComponent(&player);
	}
};





#pragma endregion
#pragma region ObjectCode

Window::Component boxObject;
Window::Initializer $nn{
	$f {
		box.size = Vector2(32,32);
		boxObject.Update = $f{
		};
		$win->AddComponent(&boxObject);
	}
};


#pragma endregion
#pragma region WindowScriptable



Window::Initializer $nn{
	$f /*  Window functions  */ {
		/*
		* bit 1: debug
		* 
		* 
		* 
		*/
		$win->Start = $f {
			Camera::GetInstance()->SetTarget(&playerimg.transform.position); 
		};
		$win->OnFocus = $f {};
		$win->OnFocusLost = $f {};
		$win->OnMaximize = $f {};
		$win->OnMinimize = $f {};
		$win->Update = $f{
			if ($win->keyboard.GetKeyDown(Input::K_GRAVE)) {
				$win->debug = !$win->debug;
			}
		};
		OVERLAY_IMG.lockCamera();
		OVERLAY_IMG.size = Vector2(720,720);
		OVERLAY_IMG.enabled = false;
		$win->PostPreRender = $f{
			if (getbit($win->data, 1)) {
				OVERLAY_IMG.Draw();
			}
			if ($win->debug) {
				setbit($win->data, 0, 1);
				for (unsigned int o = 0; o < Collider2d::_mGlobalColArr.size(); o++) {
					Collider2d* c = Collider2d::_mGlobalColArr[o];
					SDL_SetRenderDrawColor($win->SDL_REND, 255, 35, 0, 128);
					SDL_RenderFillRect($win->SDL_REND, c->rect);
				}
				for (uint m = 0; m < MeshCollider2d::_mGlobalColArr.size(); m++) {
					for (uint l = 0; l < MeshCollider2d::_mGlobalColArr[m]->lines.size(); l++) {
						MeshLine line = MeshCollider2d::_mGlobalColArr[m]->lines[l];
						Vector2 s = line.start + -*Camera::GetInstance()->m_target;
						Vector2 e = line.end + -*Camera::GetInstance()->m_target;
						SDL_SetRenderDrawColor($win->SDL_REND, 0, 0, 255, 255);
						SDL_RenderDrawLine($win->SDL_REND, s.x, s.y, e.x, e.y);
					}
				}
			}
			else {
				setbit($win->data, 0, 0);
			}
		};
	}
};


#pragma endregion




