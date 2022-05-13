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
	alib_assert_p(Window::WindowInstance == nullptr, "Window hasn't been initialized!", 0);
	Window::WindowInstance->argc = argc;
	Window::WindowInstance->argv = argv;
	Window::WindowInstance->workers.push_back(
		std::thread([&]() {
			while (Window::WindowInstance->running) {
				alib_remove_any_of<Sprite*>(Sprite::_mglobalspritearr, nullptr);
				alib_remove_any_of<RectCollider2d*>(RectCollider2d::_mGlobalColArr, nullptr);
				RectCollider2d::_mGlobalColArr.shrink_to_fit();
				for (unsigned int o = 0; o < RectCollider2d::_mGlobalColArr.size(); o++) {
					RectCollider2d* oc = RectCollider2d::_mGlobalColArr[o];
					if (!SUCCEEDED(oc)) { continue; }
					for (unsigned int i = 0; i < RectCollider2d::_mGlobalColArr.size(); i++) {
						RectCollider2d* ic = RectCollider2d::_mGlobalColArr[i];
						if (
							(SUCCEEDED(ic) && SUCCEEDED(oc)) && (ic != oc) &&
							(SUCCEEDED(ic->rect_cs) || SUCCEEDED(ic->rect_cs)) &&
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
				std::sort(Sprite::_mglobalspritearr.begin(), Sprite::_mglobalspritearr.end(), [](Sprite* a, Sprite* b)
					{
						return a->layer < b->layer;
					}
				);

				Sleep(1);
			}
		})
	);
	while (Window::WindowInstance->running) {
		Window::WindowInstance->procEvent();
		Window::WindowInstance->IdleFuncInternal();
		alib_sleep_micros(10);
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
