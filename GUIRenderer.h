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
	std::vector<std::any*> _element_arr;
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
		std::shared_ptr < SDL_Surface > surf;
		SDL_Texture* texture;
		VectorRect uv{0,0,1,1};
		Transform transform;
		void operator ~() { }//glDeleteTextures(1, &texid); }
		ImageElement(const char* filename) {
			surf = std::shared_ptr<SDL_Surface>(IMG_Load(filename));
			texture = SDL_CreateTextureFromSurface(SDL_REND_RHPP, surf.get());
			_element_arr.push_back(new std::any(this));
		}
		void Render() {

			if (!surf) {
				_element_arr.erase(_element_arr.begin() + ui_elem_offset);
				return;
			}
			SDL_Rect _src = {uv.x, uv.y, uv.w, uv.h};
			SDL_Rect _dst = { transform.position.x, transform.position.y, transform.scale.x, transform.scale.y };
			int _ = SDL_RenderCopyEx(SDL_REND_RHPP, texture, &_src, &_dst, transform.angle, NULL, SDL_FLIP_NONE);
			if (_ < 0) {
				printf("Unable to draw: %s\n", SDL_GetError());
				return;
			}
			//printf("Rendering ui image with size (%d, %d) at (%d, %d) with uv (%d, %d, %d, %d)\n", transform.scale.x, transform.scale.y, transform.position.x, transform.position.y, uv.x, uv.y, uv.w, uv.h);
		}
	};
}

#endif
#include "guiRendererDefs.hpp"