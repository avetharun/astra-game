#if !defined(RENDERER_HPP)
#define RENDERER_HPP
#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <functional>
#include <iostream>
#include <sdl/SDL_image.h>
#include <vector>
#include <map>
#include "utils.hpp"
#include "Vectors.hpp"
#include "camera.hpp"
#include <thread>
#include <typeinfo>
#include <typeindex>
#include <math.h>
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
	/**
	 *  \brief What layer is the sprite on? (basically Z offset)
	 */
	int layer = -10;
	bool enableCameraOffset = false; // Enables/disables if camera::m_Offset will offset sprite position.
	static std::vector<Sprite*> _mglobalspritearr;
	
	Sprite() {
		_mglobalspritearr.push_back(this);
	};
	SDL_Surface* surf{};
	Vector2 size;
	unsigned char* pixels = nullptr;
	int channels = 4;
	SDL_Rect uv;
	void operator ~() {
		SDL_FreeSurface(surf);
	}
	void unlockCamera() {
		setbit(params, 0, 0);
	}
	void lockCamera() {
		setbit(params, 0, 1);
	}

	Vector2 center_position;
	Vector2 _offset;
	SDL_Rect _rect;
	Vector2 _cameraViewOffset; 
	SDL_Rect* rect;
	struct Transform{
		Vector2 position;
		Vector angle;
	};
	Sprite::Transform transform;
	void Update() {
		if (getbit(params, 0)) {
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
		rect->x =(int)_offset.x-size.x/2;
		rect->y =(int)_offset.y-size.y/2;
		rect->w =(int)size.x;
		rect->h =(int)size.y;
		// To center the actual sprite, we offset it by -half size
		center_position = Vector2(
			rect->x + rect->w / 2,
			rect->y + rect->h / 2
		);
	};
	/*
	* \brief Draws to screen immediately. You can run this, or let the batch renderer do its thing. Either works.
	* 
	*/
	void Draw() {
		Update();
		/* Not working, don't use commented out below. */
		/*
		SDL_Rect bounds =
			SDL_Rect{
				(int)Camera::GetInstance()->m_Position.x,
				(int)Camera::GetInstance()->m_Position.y,
				(int)Camera::GetInstance()->m_Viewport.x,
				(int)Camera::GetInstance()->m_Viewport.y
		};
		if (!VectorRect::checkCollision(&bounds, &rect) && getbit(params,0)) {
#ifdef COUT_DEBUG
			std::cout << "Not rendering sprite at position: " << transform.position << " because it's off screen of camera at " <<
				Camera::GetInstance()->m_Position << " with viewport " << Camera::GetInstance()->m_Viewport << '\n';
#endif
			return;

		}*/
		int SDL_FLIP_V = SDL_FLIP_NONE;
		SDL_Rect r = SDL_Rect{ 
			rect->x,
			rect->y,
			abs(size.x) == 0? surf->w*3:abs(size.x) ,
			abs(size.y) == 0? surf->h*3:abs(size.y) ,
		};
		if (size.x < 0) {
			SDL_FLIP_V |= SDL_FLIP_VERTICAL;
		}
		if (size.y < 0) {
			SDL_FLIP_V |= SDL_FLIP_HORIZONTAL;
		}
		SDL_Texture* Texture = SDL_CreateTextureFromSurface(SDL_REND_RHPP, surf);
		if (&uv == nullptr || cmpSDL_Rect(uv, SDL_Rect{0, 0, 0, 0}))
		{
			(SDL_RenderCopyEx(SDL_REND_RHPP, Texture, NULL, &r, transform.angle, NULL, (SDL_RendererFlip)SDL_FLIP_V) < 0)
				? []() {std::cout << SDL_GetError() << '\n'; }() : noop;
		}
		else {
			(SDL_RenderCopyEx(SDL_REND_RHPP, Texture, &uv, &r, transform.angle, NULL, (SDL_RendererFlip)SDL_FLIP_V) < 0)
				? []() {std::cout << SDL_GetError() << '\n'; }() : noop;
		}
		SDL_DestroyTexture(Texture);
		Update();
	}

	


	Sprite(const char* filename, SDL_Rect spriteuv = SDL_Rect{ 0,0,0,0 }) {
		uv = spriteuv;
		layer = 0;
		rect = &_rect;
		_mglobalspritearr.push_back(this);
		_cameraViewOffset = Vector2(0, 0);
		unlockCamera();
		std::map<const char*, SDL_Surface*>::iterator it = surfFilemaps.find(filename);
		if (it != surfFilemaps.end())
		{
			surf = surfFilemaps[filename];
			return;
		}
		surf = loadImage(filename);
		if (surf == nullptr) {
			std::cout << "Surface null in build source " << __FILE__ << " with surface file: " << filename << std::endl;
			ForceExit();
			return;
		}
		#if defined (COUT_DEBUG)
			std::cout << "Created surface from " << filename << " with " << (int)surf->format->BytesPerPixel << " channels with report of " << IMG_GetError() << '\n';
		#endif
		surfFilemaps.emplace(filename, surf);
	}

};
std::vector<Sprite*> Sprite::_mglobalspritearr = std::vector<Sprite*>();

struct BatchRenderer {
	void Render() {
		std::sort(Sprite::_mglobalspritearr.begin(), Sprite::_mglobalspritearr.end(), [](Sprite* a, Sprite* b)
			{ 
				return a->layer < b->layer; 
			}
		);
		for (unsigned int i = 0; i < Sprite::_mglobalspritearr.size(); i++) {
			Sprite* s = Sprite::_mglobalspritearr[i];
			(s->enabled) ? s->Draw() : noop ;
		}

	}
};

#endif