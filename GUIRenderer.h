#if !defined(GUI_RENDERER_HPP)
#define GUI_RENDERER_HPP
#include "renderer.hpp"
#include "utils.hpp"
#include <SDL2/SDL_image.h>
#include <any>
#include "imgui/imgui_format.h"
#include "imgui/imgui_markdown.h"
#include "imgui/imgui_uielement.h"

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
		static void RenderAnyUpdate();
	};
	struct TextElement;
	struct ButtonElement;
	struct ImageElement;
	std::vector<std::any*> _element_arr;
	// rect:
	// xy = pos
	// wh = scale
	struct TextElement {
		bool __render_on_update = false;
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
		bool __render_on_update = false;
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
		bool centered = false;
		bool __render_on_update = false;
		bool enabled = true;
		SDL_Surface* surf = new SDL_Surface();
		SDL_Texture* texture;
		VectorRect uv{0,0,1,1};
		Transform transform;
		std::string fname;
		
		void operator ~() { 
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surf);
		}
		ImageElement(const char* filename) {
			this->fname = filename;
			*surf = *IMG_Load(filename);
			texture = SDL_CreateTextureFromSurface(SDL_REND_RHPP, surf);
			_element_arr.push_back(new std::any(this));
		}
		void Render() {
			if (surf && texture) {
				SDL_Rect _dst = { (int)transform.position.x, (int)transform.position.y, (int)transform.scale.x, (int)transform.scale.y };
				if (centered) {
					_dst.x = Camera::GetInstance()->m_Offset.x + transform.position.x;
					_dst.y = Camera::GetInstance()->m_Offset.y + transform.position.y;
				}
				SDL_Rect _src = { (int)uv.x, (int)uv.y, (int)uv.w, (int)uv.h };
				SDL_Point _origin_ = { transform.origin.x, transform.origin.y };
				int status = SDL_RenderCopyEx(SDL_REND_RHPP, texture, &_src, &_dst, transform.angle, &_origin_, SDL_FLIP_NONE);
				if (status < 0) {
					printf("Unable to draw: %s\n", SDL_GetError());
					return;
				}
			}
			else {
				_element_arr.erase(_element_arr.begin() + ui_elem_offset);
				return;
			}
			//printf("Rendering ui image with size (%d, %d) at (%d, %d) with uv (%d, %d, %d, %d)\n", transform.scale.x, transform.scale.y, transform.position.x, transform.position.y, uv.x, uv.y, uv.w, uv.h);
		}
	};
}

#endif
#include "guiRendererDefs.hpp"