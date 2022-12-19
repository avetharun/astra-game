#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "utils.hpp"
// THIS MUST RUN BEFORE EVERYTHING ELSE!!!
alib_inline_run _nn{ [&]() {
	// Running from (out)/lib/game.exe
	if (alib_endswith(alib_gcwd(), "\\lib") || alib_file_exists("SDL2.dll")) {
		std::cout << "User ran cw.exe or cwd contains SDL2.dll! Falling back to previous directory.\nNext time, please run the executable in the previous directory!\n";
		alib_scwd("../");
	}
} };

#include <algorithm>
#include "game.hpp"



int main(int argc, char **argv)
{
	alib_assert_p(Window::WindowInstance == nullptr, "Window hasn't been initialized!", 0);
	printf("SDL vsync state: %d with error %s\n", SDL_GL_GetSwapInterval(), SDL_GetError());
	Window::WindowInstance->argc = argc;
	Window::WindowInstance->argv = argv;
	Window::WindowInstance->workers.push_back(
		std::thread([&]() {
			while (Window::WindowInstance->running) {
				alib_remove_any_of<Sprite*>(Sprite::_mglobalspritearr, nullptr);
				alib_remove_any_of<RectCollider2d*>(RectCollider2d::_mGlobalColArr, nullptr);
				std::sort(Sprite::_mglobalspritearr.begin(), Sprite::_mglobalspritearr.end(), [](Sprite* a, Sprite* b)
					{
						return a->layer < b->layer;
					}
				);

				Sleep(15);
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
	try {
		ImGui::DestroyContext();
		SDL_Quit();
	}
	catch (...) {}

	exit(0);
}
