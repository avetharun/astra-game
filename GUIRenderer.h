#if !defined(GUI_RENDERER_HPP)
#define GUI_RENDERER_HPP
#include "renderer.hpp"
#include "utils.hpp"
#include <SDL2/SDL_image.h>
#include <any>


namespace UI {
	size_t ui_elem_offset;
	std::string _fmt_name() {
		std::string _name__ = "##___ELEMENT_UI";
		_name__.append("__");
		_name__.append(alib_strfmt("%d_", ui_elem_offset));
		return _name__;
	}
	struct GUIRenderer
	{
		static const ImGuiWindowFlags __elementWinFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
		static void Render();
	};
	struct TextElement;
	struct ButtonElement;
	struct ImageElement;
	std::vector<std::any> _element_arr;
	// rect:
	// xy = pos
	// wh = scale
	struct TextElement {
		bool enabled = true;
		Vector2 pos;
		float size = 1;
		std::string text;
		void textfmt(const char* fmt, ...) {
			char* x;
			va_list ap;
			va_start(ap, fmt);
			int size = vasprintf(&x, fmt, ap);
			va_end(ap);
			this->text = x;
		}
		void Render();
		TextElement(std::string);
		TextElement();
	};
	struct ButtonElement {
		bool enabled = true;
		Vector2 pos{0,0};
		enum ButtonElementFlags {
			NoBackground = B8(00000001),
		};
		ButtonElementFlags buttonFlags{};
		std::string text = "";
		float size = 1.0f;
		std::function<void()> onClick = [&](){};
		void textfmt(const char* fmt, ...) {
			char* x;
			va_list ap;
			va_start(ap, fmt);
			int size = vasprintf(&x, fmt, ap);
			va_end(ap);
			this->text = x;
		}
		void Render();
		ButtonElement(std::string, ButtonElementFlags _flags);
		ButtonElement();
	};
	struct ImageElement {
		bool enabled = true;
		unsigned char* pixels;
		int width, height, pitch;
		VectorRect uv{0,0,1,1};
		GLuint texid;
		void operator ~() { free(pixels); }
		ImageElement(const char* filename) {
			ImplLoadTextureFromFile(filename, &texid, &width, &height);
			_element_arr.push_back(this);
		}
		void Render() {
			ImVec2 uvbegin = {(float)uv.x, (float)uv.y};
			ImVec2 uvend =   {(float)uv.w, (float)uv.h};
			ImGui::Begin(_fmt_name().c_str(), 0, GUIRenderer::__elementWinFlags);
			printf("rendering ui image, (%d, %d) to (%d, %d) with size (%d, %d)\n", uvbegin.x, uvbegin.y, uvend.x, uvend.y, width, height);
			ImGui::Image((void*)(intptr_t)texid, ImVec2(width,height), uvbegin, uvend);
			ImGui::End();
		}
	};



}

#endif
#include "guiRendererDefs.hpp"