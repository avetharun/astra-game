#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include "utils.hpp"
#include "game.hpp"

int main(int argc, char **argv)
{
	if (Window::WindowInstance == nullptr) {
		std::cout << "Window hasn't been created - not able to start!";
		exit(-1000);
	}
	
	Win->argc = argc;
	Win->argv = argv;
	Window::WindowInstance->workers.push_back(
		std::thread([&]() {
			while (1) {
				for (unsigned int o = 0; o < RectCollider2d::_mGlobalColArr.size(); o++) {
					RectCollider2d* oc = RectCollider2d::_mGlobalColArr[o];

					for (unsigned int i = 0; i < RectCollider2d::_mGlobalColArr.size(); i++) {
						RectCollider2d* ic = RectCollider2d::_mGlobalColArr[i];
						if (
							!(ic == oc) &&
							VectorRect::checkCollision(
								ic->rect,
								oc->rect
							  )
							)
						{
							ic->OnColliderHit(oc);
						}
					}
					for (unsigned int si = 0; si < Sprite::_mglobalspritearr.size(); si++) {
						Sprite* s = Sprite::_mglobalspritearr[si];
					}
				}
				if (Camera::GetInstance()->m_target) {
				}
				Sleep(1);
			}
		})
	);
	while (Window::WindowInstance->running) {
		Window::WindowInstance->procEvent();
		Window::WindowInstance->IdleFuncInternal();
		Sleep(1);
	}
	for (unsigned int i = 0; i < Window::WindowInstance->workers.size(); i++) {
		try {
			Window::WindowInstance->workers[i].detach();
		}
		catch (const std::system_error) {
		} 

	}

	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_Quit();
}
