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


enum IMAGE_LAYERS : int {
	TOP = INT32_MAX,
	BOTTOM = INT32_MIN
};

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
std::map<std::string, SDL_Surface*> surfFilemaps;
SDL_Surface* loadImage(const char* path)
{	
	return IMG_Load(path);
}
struct SpriteAnimationMeta {
	bool animate = true;
	bool animate_once = false;
	bool reset_after_once = true;
	int CURRENT_FRAME = 0;
	float DELTA = 0;
	int frames = 0;
	float delay = 0;
	float alpha_start = 1;
	float alpha_end = 1;
	SpriteAnimationMeta() {}
	SpriteAnimationMeta(int n_frames, float m_delay) {
		this->frames = n_frames;
		this->delay = m_delay;
	}
};
struct Sprite{
	SpriteAnimationMeta meta;
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
	std::string name;
	std::string identifier;
	/**
	 *  \brief What layer is the sprite on? (basically Z offset)
	 */
	int32_t layer = -10;
	bool enableCameraOffset = false; // Enables/disables if camera::m_Offset will offset sprite position.
	static std::vector<Sprite*> _mglobalspritearr;
	bool center = false;
	
	Sprite() {
		_mglobalspritearr.push_back(this); 
		rect = new SDL_Rect();
		surf = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
	};
	SDL_Surface* surf{};
	unsigned char* pixels = nullptr;
	int channels = 4;
	SDL_Rect uv{};
	SDL_Rect __uv_last{};
	float alpha = 1;
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
	float alpha_delta;
	float alpha_from = 1;
	float alpha_to = 1;
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
		if (meta.frames > 0 && (meta.animate || meta.animate_once) ) {
			if (meta.animate_once) {
				meta.animate = true;
			}
			if (meta.DELTA > meta.delay) {
				if (meta.animate_once && meta.CURRENT_FRAME == meta.frames) {
					meta.animate = false;
					meta.animate_once = false;
					if (!meta.reset_after_once) {
						meta.CURRENT_FRAME = meta.frames;
					}
					else {
						meta.CURRENT_FRAME = 1;
					}
				}
				else {
					meta.CURRENT_FRAME = meta.CURRENT_FRAME == meta.frames ? 1 : meta.CURRENT_FRAME + 1;
				}
				uv_tile.y = meta.CURRENT_FRAME;
				meta.DELTA = 0;
			}
			ImGuiIO& io = ImGui::GetIO();
			meta.DELTA += io.DeltaTime;
		}
		else if (meta.CURRENT_FRAME > 0) {
			uv_tile.y = meta.CURRENT_FRAME;
		}
		uv_final.x = calc_uv_pair(uv.x, uv.w, uv_tile.x);
		uv_final.y = calc_uv_pair(uv.y, uv.h, uv_tile.y);
		uv_final.w = uv.w;
		uv_final.h = uv.h;
		if (center) {
			_offset = Vector2(
				Camera::GetInstance()->m_Viewport.x / 2 + Camera::GetInstance()->m_Offset.x,
				Camera::GetInstance()->m_Viewport.y / 2 + Camera::GetInstance()->m_Offset.y
			) + _cameraViewOffset;
		}
		else {
			_offset = (
					(-*Camera::GetInstance()->m_target + transform.position) 
				) + Camera::GetInstance()->m_Offset;
		}
		if (!SUCCEEDED(rect)) {
			rect = new SDL_Rect();
		}
		rect->x =(int)_offset.x;
		rect->y =(int)_offset.y;
		rect->w = abs(transform.scale.x) * Camera::GetInstance()->scale;
		rect->h = abs(transform.scale.y) * Camera::GetInstance()->scale;
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
		// cull images outside of viewport
		if (!center) {
			SDL_Rect bounds = {
				(int)Camera::GetInstance()->m_Offset.x,
				(int)Camera::GetInstance()->m_Offset.y,
				(int)Camera::GetInstance()->m_Viewport.x,
				(int)Camera::GetInstance()->m_Viewport.y
			};
			// If overlay isn't visible, then cull
			if (!Camera::GetInstance()->culling) {
				// 2 pixels padding
				bounds = {
				-2,-2,
				(int)Camera::GetInstance()->m_GlobalViewport.x + 2,
				(int)Camera::GetInstance()->m_GlobalViewport.y + 2
				};
			}
			if (!Raycast2D::RectIntersects(rect, &bounds)) {
				isRendering = false;
				return;
			}
		}

		isRendering = true;
		int SDL_FLIP_V = SDL_FLIP_NONE;
		SDL_Point _origin = SDL_Point{ (int)transform.origin.x, (int)transform.origin.y };
		SDL_SetTextureAlphaMod(Texture, (int)(alpha * 255));

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
		Sprite::_mglobalspritearr.erase(std::remove(Sprite::_mglobalspritearr.begin(), Sprite::_mglobalspritearr.end(), this), Sprite::_mglobalspritearr.end());
		if (Texture) {
			SDL_DestroyTexture(Texture);
		}
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
	std::string getID() { return this->identifier; }
	static bool eraseElementByID(std::string id) {
		for (int i = 0; i < _mglobalspritearr.size(); i++) {
			Sprite* _s = _mglobalspritearr[i];
			if (alib_streq(id, _s->identifier.c_str())) {
				_mglobalspritearr.erase(_mglobalspritearr.begin() + i);
				_s->operator~();
				return true;
			}
		}
		return false;
	}
	static Sprite* getElementByID(std::string id) {
		for (int i = 0; i < _mglobalspritearr.size(); i++) {
			Sprite* _s = _mglobalspritearr[i];
			if (alib_streq(id, _s->identifier.c_str())) {
				return _s;
			}
		}
		return nullptr;
	}
	static void insertElementWithID(std::string id, Sprite* s) {
		s->setID(id);
		_mglobalspritearr.push_back(s);
	}
	Sprite* copy() {
		Sprite* s = new Sprite();
		s->transform = this->transform;
		s->uv = this->uv;
		s->enabled = true;
		s->setID(this->getID() + " copy");
		s->layer = this->layer;
		s->name = this->name;
		*s->rect = *this->rect;
		s->surf = SDL_DuplicateSurface(this->surf);
		s->Texture = SDL_CreateTextureFromSurface(SDL_REND_RHPP, surf);;
		return s;
	}
	Sprite * withID(std::string id) {
		this->setID(id);
		return this;
	}
	void insert() {
		_mglobalspritearr.push_back(this);
	}
	void remove() {
		eraseElementByID(this->identifier);
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
struct ParticleEffects {
	struct stub {
		SDL_Rect uv;
		std::string id;
		float lifetime;
		Vector2 size;
		Vector2 randomness = {12, 12};
		float velocity = 10;
		float angle = 90; // down
		int layer = 0;
		SpriteAnimationMeta meta;
	};
	static inline stub FLAME = { .uv = {0,0, 1,1}, .id = "cw::flame", .lifetime = 2, .size = {4,4}, .angle = 270,
		.meta = SpriteAnimationMeta(3, 0.2f)
		};
	static inline stub STONE_PARTICLE = {.uv = {1,0, 1,1}, .id = "cw::stone_particle", .lifetime=0.3, .size = {4,4},
		.meta = SpriteAnimationMeta(3, 0.2f)
		};
	static inline stub RAIN = { .uv{2,0,1,1}, .id = "cw::rain", .lifetime = 10, .size = {4,4}, .randomness={100,0}, .angle = Vector2::parse_angle_vec(Vector2::down + (Vector2::left * 0.2f)),
		.meta = SpriteAnimationMeta(3, 0.2f)
		};
	static inline stub BLOOD = { .uv{6,0,1,1}, .id = "cw::blood", .lifetime = 10, .size = {4,4}, .randomness = {100,0}, .velocity = 0, .angle = Vector2::parse_angle_vec(Vector2::down + (Vector2::left * 0.2f)),
		.meta = SpriteAnimationMeta(3, 0.2f)
		};
	static inline stub SNOW = { .uv{3,0,3,3}, .id = "cw::snow", .lifetime = 10, .size = {18,18}, .randomness = {500,40}, .velocity = 4, .angle = Vector2::parse_angle_vec(Vector2::down + Vector2::left), .layer = IMAGE_LAYERS::TOP,
		.meta = SpriteAnimationMeta(3, 0.2f)
		};
};
struct ParticleEffect {
	static inline Sprite* _m_ParticleImage = nullptr;
	ParticleEffects::stub effect;
	struct _stub {
		float life = 0;
		float life_randomness = 0.8f;
		float velocity_randomness = 1;
	};
	std::vector<std::pair<Sprite*, _stub*>> particles;
	int amt;
	bool collides;
	int m_layer;
	float m_velocity;
	float m_lifetime;
	float m_dir_angle_rad = 0;
	Vector2 m_randomness;
	Vector2 start_pos;
	SDL_Rect m_uv;
	bool dtor;
	Sprite* m_particle_sprite;
	std::string name;

	static ParticleEffect* getElementByID(std::string id) {
		for (int i = 0; i < m_particle_arr.size(); i++) {
			ParticleEffect* _s = m_particle_arr[i];
			if (alib_streq(id, _s->name.c_str())) {
				return _s;
			}
		}
		return nullptr;
	}
	ParticleEffect(ParticleEffects::stub m_particle, float m_amt) {
		if (!_m_ParticleImage) {
			_m_ParticleImage = new Sprite("sprites/particles.png");
			_m_ParticleImage->transform.scale = m_particle.size;
			_m_ParticleImage->enabled = false;
			_m_ParticleImage->meta = effect.meta;
		}
		this->effect = m_particle;
		this->amt = m_amt;
		m_particle_sprite = _m_ParticleImage->copy();
		m_particle_sprite->enabled = false;
		m_particle_sprite->layer = effect.layer;
		this->m_velocity = effect.velocity;
		this->m_layer = effect.layer;
		this->m_lifetime = effect.lifetime;
		this->m_randomness = effect.randomness;
		this->m_uv = effect.uv;
		this->m_dir_angle_rad = alib_deg2rad(effect.angle);
		m_particle_sprite->uv = effect.uv;
		name = effect.id;
		for (int i = 0; i < amt; i++) {
			Sprite* copy = m_particle_sprite->copy();
			copy->enabled = true;
			particles.push_back(std::make_pair(copy, new _stub{.life = m_lifetime}));
		}

		m_particle_arr.push_back(this);
	}
	void operator ~() {
		pop(amt);
		alib_remove_any_of(m_particle_arr, this);
	}
	void append(int count) {
		amt += count;
		for (int i = 0; i < count; i++) {
			Sprite* copy = m_particle_sprite->copy();
			copy->enabled = true;
			particles.push_back(std::make_pair(copy, new _stub{ .life = m_lifetime }));
		}
	}
	void pop(int count) {
		amt -= count;
		for (int i = 0; i < count; i++) {
			auto& m_particle = particles.at(particles.size() - 1);
			m_particle.first->operator~();
			particles.erase(particles.end() - 1);
		}
	}
	void Render() {
		for (int i = 0; i < particles.size(); i++) {
			std::pair<Sprite*, _stub*>& m_particle = particles.at(i);
			m_particle.first->layer = m_layer;
			m_particle.first->uv = m_uv;
			m_particle.first->transform.scale = effect.size;
		}
		ImGuiIO io = ImGui::GetIO();
		Vector2 m_direction = Vector2(
				(1 * cos(m_dir_angle_rad)),
				(1 * sin(m_dir_angle_rad))
			);
		// transform pass
		for (int i = 0; i < particles.size(); i++) {
			std::pair<Sprite*, _stub*>& m_particle = particles.at(i);
			m_particle.second->life += io.DeltaTime;
			m_particle.first->transform.position += (m_direction * m_velocity / m_particle.second->velocity_randomness);
			float velocity_randomness = ((rand() % (int)(effect.velocity * 1000 + 1))) * 0.001f + 1;
			if (m_particle.second->life > m_lifetime * m_particle.second->life_randomness) {
				m_particle.second->velocity_randomness = velocity_randomness;
				float random_x = ((rand() % ((int)m_randomness.x == 0 ? 1 : (int)m_randomness.x * 200)) * 0.01f);
				float random_y = ((rand() % ((int)m_randomness.y == 0 ? 1 : (int)m_randomness.y * 200)) * 0.01f);
				m_particle.first->transform.position = start_pos + Vector2(random_x, random_y) ;
				m_particle.second->life = 0;
				float lr = (rand() % 100) * 0.002f;
				m_particle.second->life_randomness = lr;
			}
		}
	}
	static inline std::vector<ParticleEffect*> m_particle_arr = {};
};
struct BatchRenderer {
	void Render() {
		for (int i = 0u; i < ParticleEffect::m_particle_arr.size(); i++) {
			ParticleEffect* s = ParticleEffect::m_particle_arr[i];
			if (s == nullptr) {
				ParticleEffect::m_particle_arr.erase(ParticleEffect::m_particle_arr.begin() + i);
				continue;
			}
			s->Render();
		}
		for (unsigned int i = 0; i < Sprite::_mglobalspritearr.size(); i++) {
			Sprite* s = Sprite::_mglobalspritearr[i];
			if (s == nullptr) {
				Sprite::_mglobalspritearr.erase(Sprite::_mglobalspritearr.begin() + i);
				continue;
			}
			(s->enabled) ? s->Draw() : noop ;
		}
	}
};











#endif