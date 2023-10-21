#pragma once
#if !defined(GUI_RENDERER_HPP)
#define GUI_RENDERER_HPP
#include "utils.hpp"
#include <SDL2/SDL.h>
#include <any>
#include "imgui/imgui_format.h"
#include "imgui/imgui_markdown.h"
#include "imgui/imgui_uielement.h"
#include "global.h"
#include <stack>
#include <SDL2/SDL_image.h>
#include "renderer.hpp"
#include "cwlib/cwabt.hpp"
namespace UI {
	struct TextElement;
	struct ImageElement;
	struct ButtonElement;
	struct ImageElement;
	struct GameTextBox;
	struct GameOptionsBox;

	struct GUIRenderer
	{
		static const ImGuiWindowFlags __elementWinFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
		static void Render();
		static void RenderAnyUpdate();
	};
	size_t ui_elem_offset;
	std::string _fmt_name() {
		std::string _name__ = "##___ELEMENT_UI";
		_name__.append("__");
		_name__.append(alib_strfmt("%d_", ui_elem_offset));
		return _name__;
	}
	static std::vector<std::any*> _element_arr;
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
			va_list args;
			va_start(args, fmt);
			size_t bufsz = 0;
			bufsz = vsnprintf(NULL, 0, fmt, args);
			char* _buf = new char[bufsz];
			vsnprintf(_buf, bufsz, fmt, args);
			va_end(args);
			text = std::string(_buf);
		}

		void Render() {
			if (!enabled) { return; }
			if (this->size <= 0) { this->size = 1.0f; }
			ImGui::TextForeground(this->text, this->pos);
		}
		TextElement(std::string _txt) {
			_element_arr.push_back(new std::any(this));
			this->text = _txt;
		}
		TextElement() {
			_element_arr.push_back(new std::any(this));
		}
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
		void Render() {
			if (!enabled) { return; }
			if (this->size == 0) { this->size = 1.0f; }
			ImGui::SetNextWindowPos({ (float)this->pos.x - 8, (float)this->pos.y - 12 });
			ImGuiIO& __io = ImGui::GetIO();
			ImVec2 _v = ImGui::CalcTextSize(this->text.c_str());
			std::string _s = _fmt_name();
			ImGui::Begin(_s.c_str(), 0, GUIRenderer::__elementWinFlags);
			if (this->buttonFlags & ButtonElementFlags::NoBackground) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0, 0, 0, 0)));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0, 0, 0, 0)));
			}

			bool __pressed = ImGui::Button(_fmt_name().c_str(), _v);
			ImGui::SameLine();
			ImGui::SetCursorPosX(this->pos.x + _v.x / 2);
			ImGui::SetWindowFontScale(this->size);
			ImGui::TextMulticolored(this->text.c_str());
			if (this->buttonFlags & ButtonElementFlags::NoBackground) {
				ImGui::PopStyleColor(3);
			}
			if (__pressed) { this->onClick(); }
			ImGui::End();
		}
		ButtonElement(std::string _txt, ButtonElementFlags _flags = {}) {
			_element_arr.push_back(new std::any(this));
			this->text = _txt;
			this->buttonFlags = _flags;
		}
		ButtonElement()
		{
			_element_arr.push_back(new std::any(this));
		}
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
		}
		ImageElement(const char* filename) {
			this->fname = filename;
			*surf = *IMG_Load(filename);
			texture = SDL_CreateTextureFromSurface(SDL_REND_RHPP, surf);
			_element_arr.push_back(new std::any(this));
		}
		SDL_Rect _dst = {};
		void Render() {
			if (surf && texture) {
				if (centered) {
					_dst.x = (Camera::GetInstance()->m_Viewport.x * 0.5f) - (transform.scale.x * 0.5f);
					_dst.y = (Camera::GetInstance()->m_Viewport.y * 0.5f) - (transform.scale.y * 0.5f);
				}
				_dst.w = transform.scale.x;
				_dst.h = transform.scale.y;
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

	struct GameTextBox {
		luabridge::LuaRef on_complete = nullptr;
		std::string text;
		std::string title;
		float delta = 0;
		bool is_active;
		bool is_first_frame = true;
		bool is_wanting_dtor = false;
		static inline std::stack<GameTextBox*> _mBoxes;
		static inline float seconds_for_btn_flash = 0.5f;
		static inline float padding_bottom = 128;
		static inline float padding_sides = 128;
		static inline int TextBoxOffset = 0;
		static inline bool isInteracting;
		GameTextBox(std::string label_id, std::string m_text, luabridge::LuaRef on_complete_func) {
			this->text = m_text;
			this->on_complete = on_complete_func;
			this->title = label_id + alib_strfmt("###%d", TextBoxOffset);
			_mBoxes.push(this);
			TextBoxOffset++;
		}
		void Render() {
			if (is_first_frame) {

				is_first_frame = false;
				return;
			}
			ImGuiIO& io = ImGui::GetIO();
			if (io.DisplaySize.x > io.DisplaySize.y) {
				padding_sides = io.DisplaySize.x * 0.21f;
			}
			else {
				padding_sides = io.DisplaySize.x * 0.04f;
			}
			float ysz = ImGui::CalcTextSize(text.c_str()).y + ImGui::CalcTextSize("#").y;
			ImGui::SetNextWindowSize({ io.DisplaySize.x - (padding_sides * 2), padding_bottom * 0.9f });
			ImGui::SetNextWindowPos({ padding_sides, io.DisplaySize.y - padding_bottom });
			ImGui::Begin(alib_strfmt("%sGameTextBox", title.c_str(), title.c_str()), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			this->isInteracting = true;
			this->is_active = ImGui::IsWindowFocused();
			const char* line = text.c_str();
			const char* text_end = text.c_str() + text.length();

			if (line < text_end)
			{
				while (line < text_end)
				{
					const char* line_end = (const char*)memchr(line, '\n', text_end - line);
					if (!line_end)
						line_end = text_end;
					ImGui::Text(std::string(line, (int)(line_end - line)).c_str());
					line = line_end + 1;
				}
			}
			if (delta > seconds_for_btn_flash) {
				ImGui::SameLine();
				ImGui::PushFont(CWLGlobals::SymbolFont);
				ImGui::Text("_");
				ImGui::PopFont();
				if (delta > seconds_for_btn_flash * 2) {
					delta = 0;
				}
			}
			ImGui::End();
			delta += io.DeltaTime;
			if (
				Input::Keyboard::GetKeyPressed(Input::K_return) ||
				Input::Keyboard::GetKeyPressed(Input::K_space) ||
				Input::Keyboard::GetKeyPressed(Input::K_e)
				) {
				if (!on_complete.isNil()) {
					on_complete();
				}
				is_wanting_dtor = true;
				isInteracting = false;

			}
		}
	};
	struct Alignment{
		static inline const int LEFT = 1;
		static inline const int RIGHT = 2;
		static inline const int CENTER = 3;
		static inline const int TOP = 4;
		static inline const int BOTTOM = 5;
	};
	struct GameOptionsBox {
		static inline bool key_pressed = false;
		luabridge::LuaRef on_complete = nullptr;
		bool is_wanting_dtor = false;
		std::string title;
		int alignment;
		std::vector<std::string> keys;
		ABTDataStub options;
		int selected_key = 0;
		static inline std::stack<GameOptionsBox*> _mOptionsBoxes;
		bool is_first_frame = true;
		volatile bool wait_a_frame = false;
		void Render() {
			// wait for destructor
			if (is_wanting_dtor || is_first_frame) {
				is_first_frame = false;
				return;
			}
			if (options.is_array()) {
				this->is_wanting_dtor = true;
				cwError::push(cwError::CW_ERROR);
				cwError::serrof("Error rendering GameOptionsBox: Options was an array! It needs to be a map! Are you initializing this properly?");
				cwError::pop();
				return;
			}
			UI::GameTextBox::isInteracting = true;
			ImGuiIO& io = ImGui::GetIO();
			if (io.DisplaySize.x > io.DisplaySize.y) {
				GameTextBox::padding_sides = io.DisplaySize.x * 0.21f;
			}
			else {
				GameTextBox::padding_sides = io.DisplaySize.x * 0.04f;
			}
			ImGui::SetNextWindowSize({ io.DisplaySize.x - (GameTextBox::padding_sides * 1.25f), GameTextBox::padding_bottom * 0.9f });
			ImVec2 pos;
			float padding_bottom = (GameTextBox::padding_bottom * 2);
			switch (alignment) {
			default:
			case Alignment::BOTTOM:
			case Alignment::CENTER: pos = { GameTextBox::padding_sides, io.DisplaySize.y - GameTextBox::padding_bottom }; break;
			case Alignment::LEFT: pos = { GameTextBox::padding_sides * 0.5f, io.DisplaySize.y - padding_bottom }; break;
			case Alignment::RIGHT: pos = { io.DisplaySize.x - GameTextBox::padding_sides * 2.25f, io.DisplaySize.y - padding_bottom * 0.5f }; break;
			case Alignment::TOP:pos = { GameTextBox::padding_sides, GameTextBox::padding_bottom }; break;
			}
			ImGui::SetNextWindowPos(pos);
			ImGui::Begin(title.c_str(), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			for (int i = 0; i < keys.size(); i++) {
				if (wait_a_frame || is_wanting_dtor) {
					continue;
				}
				std::string _cur_key = keys.at(i);
				if (i == 0 && is_first_frame) {
					selected_key = i;
				}
				json key = this->options.at(_cur_key);
				if (key.is_boolean()) {
					bool _m_selected = true;
					if (selected_key != i) {
						_m_selected = false;
						ImGui::BeginDisabled();
					}
					ImGui::Text(_cur_key.c_str());
					if (!_m_selected) {
						ImGui::EndDisabled();
					}
					if (
						Input::Keyboard::GetKeyPressed(Input::K_return) ||
						Input::Keyboard::GetKeyPressed(Input::K_space) ||
						Input::Keyboard::GetKeyPressed(Input::K_e)
						) {
						is_wanting_dtor = true;
						UI::GameTextBox::isInteracting = false;
						options.set_bool(keys.at(selected_key), true);
						if (!on_complete.isNil()) {
							on_complete(options);
						}
					}
					// we need to wait a frame if some action is done, otherwise it'll think the key is pressed when iterating. This will not affect anything visually.
					if (Input::Keyboard::GetKeyPressed(Input::K_down) || Input::Keyboard::GetKeyPressed(Input::K_S)) {
						if (!key_pressed) {
							key_pressed = true;
							alib_clampptr(&(selected_key += 1), 0, (int)keys.size() - 1);
							wait_a_frame = true;
							continue;
						}
						else {
							key_pressed = false;
						}
					}
					else if (Input::Keyboard::GetKeyPressed(Input::K_up) || Input::Keyboard::GetKeyPressed(Input::K_W)) {
						if (!key_pressed) {
							key_pressed = true;
							alib_clampptr(&(selected_key -= 1), 0, (int)keys.size() - 1);
							wait_a_frame = true;
							continue;
						}
						else {
							key_pressed = false;
						}
					}
				}
			}
			ImGui::End();
			wait_a_frame = false;
		}
		GameOptionsBox(std::string m_title, ABTDataStub m_options, luabridge::LuaRef m_on_complete) {
			_mOptionsBoxes.push(this);
			this->options = m_options;
			this->title = m_title;
			this->alignment = Alignment::RIGHT;
			this->keys = m_options.get_keys();
			this->on_complete = m_on_complete;
		}
		GameOptionsBox(std::string m_title, ABTDataStub m_options, luabridge::LuaRef m_on_complete, int m_alignment) {
			_mOptionsBoxes.push(this);
			this->options = m_options;
			this->title = m_title;
			this->alignment = m_alignment;
			this->keys = m_options.get_keys();
			this->on_complete = m_on_complete;
		}

	};
	void GUIRenderer::Render() {
		_element_arr.shrink_to_fit();
		for (ui_elem_offset = 0; ui_elem_offset < _element_arr.size(); ui_elem_offset++) {
			std::any* tx_elem = _element_arr.at(ui_elem_offset);
			if (!tx_elem) {
				_element_arr.erase(_element_arr.begin() + ui_elem_offset);
				// Will shrink vector next frame.
				continue;
			}
			if (alib_costr(tx_elem->type().name(), "UI::TextElement *")) {
				TextElement* _e = std::any_cast<TextElement*>(*tx_elem);
				if (!_e->enabled || _e->__render_on_update) { continue; }
				_e->Render();
				continue;
			}
			if (alib_costr(tx_elem->type().name(), "UI::ButtonElement *")) {
				ButtonElement* _e = std::any_cast<ButtonElement*>(*tx_elem);
				if (!_e->enabled || _e->__render_on_update) { continue; }
				_e->Render();
				continue;
			}
			if (alib_costr(tx_elem->type().name(), "UI::ImageElement *")) {
				ImageElement* _e = std::any_cast<ImageElement*>(*tx_elem);
				if (!_e->enabled || _e->__render_on_update) { continue; }
				_e->Render();
				continue;
			}
		}
		if (GameTextBox::_mBoxes.size() >= 1) {
			GameTextBox* box = GameTextBox::_mBoxes.top();
			if (!box->is_wanting_dtor) {
				box->Render();
			}
			else {
				GameTextBox::_mBoxes.pop();
				delete box;
			}
		}
		if (GameOptionsBox::_mOptionsBoxes.size() >= 1) {
			GameOptionsBox* box = GameOptionsBox::_mOptionsBoxes.top();
			if (!box->is_wanting_dtor) {
				box->Render();
			}
			else {
				GameOptionsBox::_mOptionsBoxes.pop();
				delete box;
			}
		}
	}
	void GUIRenderer::RenderAnyUpdate() {
		_element_arr.shrink_to_fit();
		//for (ui_elem_offset = 0; ui_elem_offset < _element_arr.size(); ui_elem_offset++) {
		//	std::any* tx_elem = _element_arr.at(ui_elem_offset);
		//	if (!tx_elem) {
		//		_element_arr.erase(_element_arr.begin() + ui_elem_offset);
		//		// Will shrink vector next frame.
		//		continue;
		//	}
		//	if (alib_costr(tx_elem->type().name(), "UI::TextElement *")) {
		//		TextElement* _e = std::any_cast<TextElement*>(*tx_elem);
		//		if (!_e->enabled || !_e->__render_on_update) { continue; }
		//		_e->Render();
		//		continue;
		//	}
		//	if (alib_costr(tx_elem->type().name(), "UI::ButtonElement *")) {
		//		ButtonElement* _e = std::any_cast<ButtonElement*>(*tx_elem);
		//		if (!_e->enabled || !_e->__render_on_update) { continue; }
		//		_e->Render();
		//		continue;
		//	}
		//	if (alib_costr(tx_elem->type().name(), "UI::ImageElement *")) {
		//		ImageElement* _e = std::any_cast<ImageElement*>(*tx_elem);
		//		if (!_e->enabled || !_e->__render_on_update) { continue; }
		//		_e->Render();
		//		continue;
		//	}
		//}
	}
}

#endif
