#if !defined(RENDERER_HPP)
#define RENDERER_HPP
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

#include <luaaa.hpp>
#include "lua.hpp"
#include "cwlib/cwerror.h"


SDL_Renderer* SDL_REND_RHPP;
SDL_Window* SDL_WIND_RHPP; 


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
std::map<const char*, SDL_Surface*> surfFilemaps;
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
	const char* identifier = nullptr;
	/**
	 *  \brief What layer is the sprite on? (basically Z offset)
	 */
	int layer = -10;
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
	void unlockCamera() {
		setbitv(params, 0, 0);
	}
	void lockCamera() {
		setbitv(params, 0, 1);
	}
	Vector2 center_position{};
	Vector2 _offset{};
	Vector2 _cameraViewOffset{};
	SDL_Rect* rect {nullptr};
	Transform transform{};



	void Update() {
		if (getbitv(params, 0)) {
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
			SDL_SetRenderDrawColor(SDL_REND_RHPP, 128, 255, 255, 255);
			SDL_RenderDrawRect(SDL_REND_RHPP, &bounds);
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
		SDL_Point _origin = SDL_Point{ transform.origin.x, transform.origin.y };
		if (&uv == nullptr || SDL_RectEmpty(&uv))
		{
			(SDL_RenderCopyEx(SDL_REND_RHPP, Texture, NULL, rect, transform.angle, &_origin, (SDL_RendererFlip)SDL_FLIP_V) < 0)
				? []() {std::cout << SDL_GetError() << '\n'; }() : noop;
		}
		else {
			(SDL_RenderCopyEx(SDL_REND_RHPP, Texture, &uv, rect, transform.angle, &_origin, (SDL_RendererFlip)SDL_FLIP_V) < 0)
				? []() {std::cout << SDL_GetError() << '\n'; }() : noop;
		}
	}

	
	void operator ~() {
		auto s_sf = surfFilemaps;
		// the pixels variable won't be initialized if it's invalid. If so, ignore it and just remove this object.
		s_sf.erase(name);
		auto sv = Sprite::_mglobalspritearr;
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
		std::map<const char*, SDL_Surface*>::iterator it = surfFilemaps.find(filename);
		if (it == surfFilemaps.end())
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
			surf = surfFilemaps[filename];
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
	void setID(const char* i) {
		this->identifier = i;
	}
	static Sprite* getElementByID(std::string id) {
		for (int i = 0; i < _mglobalspritearr.size(); i++) {
			Sprite* _s = _mglobalspritearr[i];
			if (_s->identifier == nullptr) { continue; }
			std::cout << "Testing ID '" << id.c_str() << "' with id '" << _s->identifier << '\'' << '\n';
			if (strcmp(id.c_str(), _s->identifier)) {
				std::cout << "ID matches!";
				return _s;
			}
		}
		return nullptr;
	}


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
	std::list<int> luaGetScale() {
		return {this->transform.scale.x, this->transform.scale.y};
	}
	std::list<int> luaGetPosition() {
		return { this->transform.position.x, this->transform.position.y };
	}
	int luaGetRotation() {
		return transform.angle;
	}
	void luaScale(std::vector<int> _scal) {
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
		std::sort(Sprite::_mglobalspritearr.begin(), Sprite::_mglobalspritearr.end(), [](Sprite* a, Sprite* b)
			{ 
				return a->layer < b->layer; 
			}
		);
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