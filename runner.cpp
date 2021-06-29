
#include <iostream>
#include <algorithm>

#include "game.hpp"

int main(int argc, char **argv)
{
	
	if (Window::WindowInstance == nullptr) {
		std::cout << "Window hasn't been created - not able to start!";
		ForceExit();
	}
	
	$win->argc = argc;
	$win->argv = argv;
	Window::WindowInstance->workers.push_back(
		std::thread([&]() {
			while (1) {
				Sleep(2); // sleep a bit to not hog cpu, thanks.
				for (unsigned int o = 0; o < Collider2d::_mGlobalColArr.size(); o++) {
					Collider2d* oc = Collider2d::_mGlobalColArr[o];

					for (unsigned int i = 0; i < Collider2d::_mGlobalColArr.size(); i++) {
						Collider2d* ic = Collider2d::_mGlobalColArr[i];
						if (
							!(ic == oc) &&
							VectorRect::checkCollision(
								ic->rect,
								oc->rect
							)
							)
						{
							ic->OnColliderHit(Collider2d::_mGlobalColArr[o]);
						}
					}
				}
				if (Camera::GetInstance()->m_target) {
					for (unsigned int si = 0; si < Sprite::_mglobalspritearr.size(); si++) {
						Sprite* s = Sprite::_mglobalspritearr[si];
						s->Update();
					}
				}
			}
		})
	);
	while (Window::WindowInstance->running) {
		Window::WindowInstance->procEvent();
		Window::WindowInstance->IdleFuncInternal();
	}
	for (unsigned int i = 0; i < Window::WindowInstance->workers.size(); i++) {
		try {
			Window::WindowInstance->workers[i].detach();
		}
		catch (const std::system_error) {
		} 
	}

	SDL_Quit();
}
