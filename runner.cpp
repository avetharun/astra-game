#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "utils.hpp"
// THIS MUST RUN BEFORE EVERYTHING ELSE!!!
alib_inline_run _nn{ [&]() {
	// Running from (out)/lib/game.exe
	if (alib_endswith(alib_gcwd(), "\\lib") || alib_file_exists("SDL2.dll")) {
		alib_show_console();
		std::cout << "User ran cw.exe or cwd contains SDL2.dll! Falling back to previous directory.\nNext time, please run the executable in the previous directory!\n";
		alib_scwd("../");
	}
} };

#include <algorithm>
#include "game.hpp"



int main(int argc, char **argv)
{
	if (Window::WindowInstance == nullptr) {
		std::cout << "Window hasn't been created - not able to start!";
		exit(-1000);
	}
	
	Window::WindowInstance->argc = argc;
	Window::WindowInstance->argv = argv;
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
								ic->rect_cs,
								oc->rect_cs
							  )
							)
						{
							ic->OnColliderHit(oc); 
						}
					}
				}
				Sleep(1);
			}
		})
	);
	while (Window::WindowInstance->running) {
		Window::WindowInstance->procEvent();
		Window::WindowInstance->IdleFuncInternal();
		Sleep(0);
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
	exit(0);
}
