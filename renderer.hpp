#if !defined(RENDERER_HPP)
#define RENDERER_HPP
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <functional>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <vector>
#include <map>
#include "utils.hpp"
#include "Vectors.hpp"
#include "camera.hpp"
#include <thread>
#include <typeinfo>
#include <typeindex>
#include <math.h>
#include <algorithm>
#include "lua.hpp"
#include "cwlib/cwerror.h"
#include <lauxlib.h>
#include "imgui/imgui.h"

#include <variant>
#include <gl/glew.h>
#include <gl/GL.h>
#include "imgui/imgui_internal.h"
#include "imgui/imgui.h"


SDL_Renderer* SDL_REND_RHPP;
SDL_Window* SDL_WIND_RHPP; 

namespace ImGui {
	const char ColorMarkerStart = '^';
	const char ColorMarkerEnd = ']';
	struct Colour {
		unsigned char colour[4] = {255,255,255,255};
		unsigned char getr() {
			return (colour)[0];
		}
		unsigned char getg() {
			return (colour)[1];
		}
		unsigned char getb() {
			return (colour)[2];
		}
		unsigned char geta() {
			return (colour)[3];
		}
		void setr(unsigned char _r) {
			colour[0] = _r;
		}
		void setg(unsigned char _g) {
			colour[1] = _g;
		}
		void setb(unsigned char _b) {
			colour[2] = _b;
		}
		void seta(unsigned char _a) {
			colour[3] = _a;
		}
		Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
			this->setr(r);
			this->setg(g);
			this->setb(b);
			this->seta(a);
		}
		Colour() {}
		const char* tostring() {
			return alib_strfmt("%c%02x%02x%02x%c", ColorMarkerStart, getr(), getg(), getb(), ColorMarkerEnd);
		}
	};
	bool ProcessInlineHexColorImpl(const char* start, const char* end, ImVec4& color)
	{
		const int hexCount = (int)(end - start);
		if (hexCount == 6 || hexCount == 8)
		{
			char hex[9];
			strncpy(hex, start, hexCount);
			hex[hexCount] = 0;

			unsigned int hexColor = 0;
			if (sscanf(hex, "%x", &hexColor) > 0)
			{
				color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
				color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
				color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
				color.w = 1.0f;
				if (hexCount == 8)
				{
					color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
				}

				return true;
			}
		}

		return false;
	}

	void TextColouredFormatted(const char* fmt, ...)
	{
		char tempStr[4096];

		va_list argPtr;
		va_start(argPtr, fmt);
		_vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
		va_end(argPtr);
		tempStr[sizeof(tempStr) - 1] = '\0';

		bool pushedColorStyle = false;
		const char* textStart = tempStr;
		const char* textCur = tempStr;
		while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
		{
			if (*textCur == ColorMarkerStart)
			{
				// Print accumulated text
				if (textCur != textStart)
				{
					ImGui::TextUnformatted(textStart, textCur);
					ImGui::SameLine(0.0f, 0.0f);
				}

				// Process color code
				const char* colorStart = textCur + 1;
				do
				{
					++textCur;
				} while (*textCur != '\0' && *textCur != ColorMarkerEnd);

				// Change color
				if (pushedColorStyle)
				{
					ImGui::PopStyleColor();
					pushedColorStyle = false;
				}

				ImVec4 textColor;
				if (ProcessInlineHexColorImpl(colorStart, textCur, textColor))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, textColor);
					pushedColorStyle = true;
				}

				textStart = textCur + 1;
			}
			else if (*textCur == '\n')
			{
				// Print accumulated text an go to next line
				ImGui::TextUnformatted(textStart, textCur);
				textStart = textCur + 1;
			}

			++textCur;
		}

		if (textCur != textStart)
		{
			ImGui::TextUnformatted(textStart, textCur);
		}
		else
		{
			ImGui::NewLine();
		}

		if (pushedColorStyle)
		{
			ImGui::PopStyleColor();
		}
	}

	void TextColouredUnformatted(const char* text) {
		TextColouredFormatted(text);
	}
};

void initRenderer(SDL_Renderer* r, SDL_Window* w) {
	SDL_REND_RHPP = r;
	SDL_WIND_RHPP = w;

}



SDL_Color getrgb(SDL_Surface* surface, Uint32 px) {
	SDL_Color rgb;
	SDL_GetRGB(px, surface->format, &rgb.r, &rgb.g, &rgb.b);
	return rgb;
}
Uint32 getPixel(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}
std::map<std::string, SDL_Surface*> surfFilemaps;
SDL_Surface* loadImage(const char* path)
{	
	return IMG_Load(path);
}
struct Sprite{
	/**
	*  \brief Should the batch renderer draw this?
	*/
	bool enabled = true;
	/**
	 *  \brief Sprite paramaters
	 *	\param bit definitions:
	 *  \param | 0 : tells renderer to render sprite by world coordinates.
	 *
	 *
	 * \param ..
	 */
	char params;
	const char* name = nullptr;
	std::string identifier = "";
	/**
	 *  \brief What layer is the sprite on? (basically Z offset)
	 */
	int32_t layer = -10;
	bool enableCameraOffset = false; // Enables/disables if camera::m_Offset will offset sprite position.
	static std::vector<Sprite*> _mglobalspritearr;
	bool center = false;
	
	Sprite() {
		_mglobalspritearr.push_back(this); 
		rect = (SDL_Rect*)malloc(sizeof(struct SDL_Rect));
		surf = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
	};
	SDL_Surface* surf{};
	unsigned char* pixels = nullptr;
	int channels = 4;
	SDL_Rect uv{};
	SDL_Rect __uv_last{};
	
	/*
	Theory:
	Starting position of uv = uv.x, uv.y
	Multiply xy with the following vector, which will be the "tile" of the sprite. Check if in bounds.
	*/
	Vector2 uv_tile{0,0};
	SDL_Rect uv_final{};
	void unlockCamera() {
		setbitv(params, 0, 0);
	}
	void lockCamera() {
		setbitv(params, 0, 1);
	}
	Vector2 center_position{};
	Vector2 _offset{};
	Vector2 _cameraViewOffset{};
	SDL_Rect* rect = nullptr;
	Transform transform{};
	int calc_uv_pair(int from, int base, int amt) {
		if (from == 0) { return base * amt; }
		if (amt == 0) { return from; }
		return from + ((base * (amt)) - base);
	}
	Vector2 pixel_size;
	void Update() {
		uv_final.x = calc_uv_pair(uv.x, uv.w, uv_tile.x);
		uv_final.y = calc_uv_pair(uv.y, uv.h, uv_tile.y);
		uv_final.w = uv.w;
		uv_final.h = uv.h;
		if (center) {
			_offset = Vector2(
				Camera::GetInstance()->m_Viewport.x / 2,
				Camera::GetInstance()->m_Viewport.y / 2
			) + _cameraViewOffset;
		}
		else {
			_offset = (
					(-*Camera::GetInstance()->m_target + transform.position) 
				) + _cameraViewOffset;
		}
		if (enableCameraOffset) {
			_offset += Camera::GetInstance()->m_Offset;
		}
		if (!SUCCEEDED(rect)) {
			rect = new SDL_Rect();
		}
		rect->x =(int)_offset.x;
		rect->y =(int)_offset.y;
		rect->w = abs(transform.scale.x);
		rect->h = abs(transform.scale.y);
		if (center) {
			rect->x = (int)_offset.x - (transform.scale.x / 2);
			rect->y = (int)_offset.y - (transform.scale.y / 2);
		}
		// To center the actual sprite, we offset it by -half size
		center_position = Vector2(
			rect->x + rect->w / 2,
			rect->y + rect->h / 2
		);

		if (!SDL_RectEmpty(rect)) {
			pixel_size.x = transform.scale.x / this->rect->w;
			pixel_size.y = transform.scale.x / this->rect->h;
		}
	};
	bool manualDraw = false;
	bool isRendering;
	SDL_Texture* Texture;
	/*
	* \brief Draws to screen immediately. You can run this, or let the batch renderer do its thing. Either works.
	* 
	*/
	void Draw() {
		if (!enabled && !manualDraw) { return; }
		Update();
		// If sprite is centered, this method will never render it!
		if (!center) {
			SDL_Rect bounds = {
				(int)Camera::GetInstance()->m_Offset.x,
				(int)Camera::GetInstance()->m_Offset.y,
				(int)Camera::GetInstance()->m_Viewport.x,
				(int)Camera::GetInstance()->m_Viewport.y
			};
			//SDL_SetRenderDrawColor(SDL_REND_RHPP, 128, 255, 255, 255);
			//SDL_RenderDrawRect(SDL_REND_RHPP, &bounds);
			if (!Raycast2D::RectIntersects(rect, &bounds)) {
				isRendering = false;
#ifdef CW_EXTRA_DEBUG_INFO
				cwError::sstate(cwError::CW_DEBUG);
				cwError::serrof("Not rendering sprite at %d, %d wh(%d, %d) because it doesn't intersect camera at %d, %d wh(%d, %d)", rect->x, rect->y, rect->w, rect->h, bounds.x, bounds.y, bounds.w, bounds.h);
#endif
				return;
			}
		}

		isRendering = true;
		int SDL_FLIP_V = SDL_FLIP_NONE;
		SDL_Point _origin = SDL_Point{ (int)transform.origin.x, (int)transform.origin.y };
		if (&uv == nullptr || SDL_RectEmpty(&uv))
		{
			(SDL_RenderCopyEx(SDL_REND_RHPP, Texture, NULL, rect, transform.angle, &_origin, (SDL_RendererFlip)SDL_FLIP_V) < 0)
				? []() {std::cout << SDL_GetError() << '\n'; }() : noop;
		}
		else {
			(SDL_RenderCopyEx(SDL_REND_RHPP, Texture, &uv_final, rect, transform.angle, &_origin, (SDL_RendererFlip)SDL_FLIP_V) < 0)
				? []() {std::cout << SDL_GetError() << '\n'; }() : noop;
		}
	}

	
	void operator ~() {
		auto& s_sf = surfFilemaps;
		// the pixels variable won't be initialized if it's invalid. If so, ignore it and just remove this object.
		s_sf.erase(name);
		auto& sv = Sprite::_mglobalspritearr;
		sv.erase(std::remove(sv.begin(), sv.end(), this), sv.end());
		SDL_FreeSurface(surf);
		SDL_DestroyTexture(Texture);
		RtlZeroMemory(this, sizeof(struct Sprite));
		
	}
	Sprite(const char* filename, SDL_Rect spriteuv = SDL_Rect{ 0,0,0,0 }) {
		name = filename;
		uv = spriteuv;
		layer = 0;
		_mglobalspritearr.push_back(this);
		_cameraViewOffset = Vector2(0, 0);
		unlockCamera();
		printf("%d: ", surfFilemaps.contains(filename));
		if (!surfFilemaps.contains(filename))
		{

			surf = loadImage(filename);
			if (surf == nullptr) {
				std::cout << "Surface null in build source " << __FILE__ << " with surface file: " << filename << std::endl;
				return;
			}
			std::cout << "Created surface from " << filename << " with " << (int)surf->format->BytesPerPixel << " channels with report of " << IMG_GetError() << '\n';
			surf->flags |= SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET;
			surfFilemaps.emplace(filename, surf);
		}
		else {
			surf = surfFilemaps.at(filename);
			std::cout << "Created surface from pre existing sprite at " << filename << " with " << (int)surf->format->BytesPerPixel << " channels with report of " << IMG_GetError() << '\n';
		}
		if (uv.x == 0 && uv.y == 0 && uv.w == 0 && uv.h == 0) {
			uv = { 0,0, surf->w, surf->h };
		}
		transform.scale = Vector2(uv.w, uv.h);
		rect = (SDL_Rect*)malloc(sizeof(struct SDL_Rect));
		Texture = SDL_CreateTextureFromSurface(SDL_REND_RHPP, surf);
		return;
	}
	void setID(std::string i) {
		this->identifier = i;
	}
	static Sprite* getElementByID(std::string id) {
		for (int i = 0; i < _mglobalspritearr.size(); i++) {
			Sprite* _s = _mglobalspritearr[i];
			std::cout << "Testing ID '" << id.c_str() << "' with id '" << _s->identifier << '\'' << '\n';
			if (alib_streq(id, _s->identifier.c_str())) {
				std::cout << "ID matches!";
				return _s;
			}
		}
		return nullptr;
	}


	void lua_set_uv(int x, int y, int w, int h) {
		this->uv = { x,y,w,h };
	}
	SDL_Rect lua_get_uv() { return this->uv; }
	lua_Number lu_getLayer() { return (this->layer); }
	Vector2 getPosition() {
		return transform.position;
	}
	Vector getRotation() {
		return transform.angle;
	}
	Vector2 getScale() {
		return transform.scale;
	}
	void Scale(Vector2 scaleAmt) {
		this->transform.scale.x *= scaleAmt.x;
		this->transform.scale.y *= scaleAmt.y;
	}
	void Move(Vector2 moveAmt) {
		this->transform.position.x += moveAmt.x;
		this->transform.position.y += moveAmt.y;
	}
	void SetTransform(Transform _transform) {
		this->transform = _transform;
	}
	void Rotate(int amt) {
		this->transform.angle += amt;
	}	
	void setEnabled(bool val) {
		this->enabled = val;
	}
	std::list<double> luaGetScale() {
		return {this->transform.scale.x, this->transform.scale.y};
	}
	std::list<double> luaGetPosition() {
		return { this->transform.position.x, this->transform.position.y };
	}
	int luaGetRotation() {
		return transform.angle;
	}
	void luaScale(std::vector<double> _scal) {
		if (_scal.size() < 2) {
			luaL_error(_lstate, "Error: Scale requires two values, amount of values used: %i", _scal.size());
			return;
		}
		this->Scale({ _scal[0], _scal[1] });
	}
	static Sprite* luaNew(const std::string& file) {
		return new Sprite(file.c_str());
	}
};
std::vector<Sprite*> Sprite::_mglobalspritearr = std::vector<Sprite*>();


struct Object : private Sprite {
	Sprite* sprite = nullptr;
	SDL_Rect* collider;
private:
	Vector2 rect;
	bool enabled = false;
public:
	Transform transform;

	void UpdateInternal() {
		rect.x = transform.position.x;
		rect.y = transform.position.y;

		if (sprite != nullptr) {
			sprite->transform = this->transform;
			sprite->setEnabled(this->enabled);
		}
	}
	void AssignSprite(const Sprite & _s) {
		sprite = (Sprite*) & _s;
	}
	void RemoveSprite() {
		sprite->operator~();
		sprite = nullptr;
	}
	void Move(Vector2 const& amt) {
		this->transform.position.x += amt.x;
		this->transform.position.y += amt.y;
	}
	void Rotate(int const& amt) {
		this->transform.angle += amt;
	}
	void SetTransform(Transform const& _transform) {
		this->transform = _transform;
	}
	void_0a_f* UpdateFunc;
	void setUpdateF(void_0a_f * _f) {
		this->UpdateFunc = _f;
	}
};

struct BatchRenderer {
	void Render() {
		for (unsigned int i = 0; i < Sprite::_mglobalspritearr.size(); i++) {
			Sprite* s = Sprite::_mglobalspritearr[i];
			if (s == NULL || s == nullptr || &s->pixels == NULL) {
				Sprite::_mglobalspritearr.erase(Sprite::_mglobalspritearr.begin() + i);
				continue;
			}
			(s->enabled) ? s->Draw() : noop ;
		}

	}
};













#endif