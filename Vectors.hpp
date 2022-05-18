#ifndef __cw_vector_hpp
#define __cw_vector_hpp
#include <nlohmann/json/json.hpp>
#include <iostream>
#include <glm/glm.hpp>
//typedef int Vector;

using json = nlohmann::json;
struct highp_ivec1_cw_impl {
	double x = 0;
	highp_ivec1_cw_impl() {}
	operator int() { return lround(this->x); }
	int operator *= (auto other) { return this->x *= other; }
	int operator /= (auto other) { return this->x /= other; }
	int operator += (auto other) { return this->x += other; }
	int operator -= (auto other) { return this->x -= other; }
	int operator ++ (auto other) { return this->x++; }
	int operator -- (auto other) { return this->x--; }
	bool operator ==(auto other) { return (this->x == other); }

};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(highp_ivec1_cw_impl, x);



struct Vector : highp_ivec1_cw_impl {
	Vector(auto _iv) {
		this->x = _iv;
	}
	Vector(int _iv) {
		this->x = (_iv);
	}
	operator int() {
		return (this->x);
	}
	friend std::ostream& operator<<(std::ostream& os, const Vector& o)
	{
		os << o.x;
		return os;
	}
	static int distance(Vector first, Vector second) {
		return fabsl(second.x - first.x);
	}
};
struct Vector2 {

	static Vector2* lu_new(int x, int y) {
		Vector2* _n = new Vector2();
		_n->x = x;
		_n->y = y;
		return _n;
	}
	Vector gx() { return this->x; }
	Vector gy() { return this->y; }
	Vector sx(int v = INT32_MAX) { this->x = v; return this->x; }
	Vector sy(int v = INT32_MAX) { this->y = v; return this->y; }

	Vector2* lu_get() { return this; }

	Vector x = 0; 
	Vector y = 0;
	Vector2() {}
	Vector2(SDL_Rect r) {
		x = r.x; y = r.y;
	}
	Vector2(Vector _x, Vector _y) {
		x = _x; y = _y;
	}
	Vector2 operator +=(Vector2 other) {
		this->x = this->x + other.x;
		this->y = this->y + other.y;
		return *this;
	}
	Vector2 operator -=(Vector2 other) {
		this->x = this->x - other.x;
		this->y = this->y - other.y;
		return *this;
	}
	Vector2(RECT r, bool lt = true /* should rect use left/top (true) or right/bottom (false) */) {
		x = (lt) ? (int)r.left : (int)r.right;
		y = (lt) ? (int)r.top :  (int)r.bottom;
	}
	Vector2 operator -() {
		return { -x, -y };
	}
	Vector2 operator -(Vector2 other) {
		return { x - other.x, y - other.y };
	}
	Vector2 operator +(Vector2 other) {
		return { x + other.x, y + other.y };
	}
	Vector2 operator /(Vector2 other) {
		return { x / other.x, y / other.y };
	}
	Vector2 operator *(Vector2 other) {
		return { x * other.x, y * other.y };
	}
	Vector2 operator -(Vector other) {
		return { x - other, y - other };
	}
	Vector2 operator +(Vector other) {
		return { x + other, y + other };
	}
	Vector2 operator /(Vector other) {
		return { x / other, y / other };
	}
	Vector2 operator *(Vector other) {
		return { x * other, y * other };
	}
	bool operator ==(const nullptr_t other) {
		return (x == 0 && y == 0) ? true : false;
	}
	bool operator ==(Vector2 other) {
		return (x == other.x && y == other.y) ? true : false;
	}
	bool operator > (Vector2 other) {
		return (x > other.x && y > other.y) ? true : false;
	}
	bool operator < (Vector2 other) {
		return (x < other.x && y < other.y) ? true : false;
	}
	bool operator ==(Vector other) {
		return (x == other && y == other) ? true : false;
	}
	bool operator > (Vector other) {
		return (x > other && y > other) ? true : false;
	}
	bool operator < (Vector other) {
		return (x < other && y < other) ? true : false;
	}
	Vector luaL_getx() { return x; }
	Vector luaL_gety() { return y; }
	static Vector2 up;
	static Vector2 right;
	friend std::ostream& operator<<(std::ostream& os, const Vector2& o)
	{
		os << o.x << ", " << o.y;
		return os;
	}
	operator const char* () {
		std::ostringstream _s;
		_s << x << ", " << y;
		auto x = _s.str();
		return x.c_str();
	}
	static Vector2 distance(Vector2 one, Vector2 two) {
		return one - two;
	}
	void operator ~() {
		delete[] this;
	}
};

Vector2 Vector2::up = Vector2{ 0,1 };
Vector2 Vector2::right = Vector2{ 1,0 };
struct VectorRect {
	Vector x = 0;
	Vector y = 0;
	Vector w = 1;
	Vector h = 1;
	static SDL_Rect _emptyRect;

	static VectorRect* lu_new(int x, int y, int w, int h) {
		VectorRect* _n = new VectorRect();
		_n->x = x;
		_n->y = y;
		_n->w = w;
		_n->h = h;
		return _n;
	}
	static SDL_Rect* lu_new_SDL(int x, int y, int w, int h) {
		SDL_Rect* _n = new SDL_Rect();
		_n->x = x;
		_n->y = y;
		_n->w = w;
		_n->h = h;
		return _n;
	}
	Vector sx(Vector v = INT32_MAX) { if (v == INT32_MAX) { return this->x; } this->x = v; return this->x; }
	Vector sy(Vector v = INT32_MAX) { if (v == INT32_MAX) { return this->y; } this->y = v; return this->y; }
	Vector sw(Vector v = INT32_MAX) { if (v == INT32_MAX) { return this->w; } this->w = v; return this->w; }
	Vector sh(Vector v = INT32_MAX) { if (v == INT32_MAX) { return this->h; } this->h = v; return this->h; }

	Vector gx() { return this->x; }
	Vector gy() { return this->y; }
	Vector gw() { return this->w; }
	Vector gh() { return this->h; }
	// Subtract sub from dst, and put the result in dst
	static void SubRectI(SDL_Rect* dst, SDL_Rect* sub) {
		dst->x -= sub->x;
		dst->y -= sub->y;
		dst->w -= sub->w;
		dst->h -= sub->h;
	}
	static SDL_Rect SubRect(SDL_Rect* first, SDL_Rect* second) {
		VectorRect proxy1(*first);
		VectorRect proxy2(*second);
		return proxy1 - proxy2;
	}

	VectorRect() {}
	VectorRect(auto _x, auto _y, auto _w, auto _h) {
		w.x = _w; h.x = _h; x.x = _x; y.x = _y;
	}
	operator SDL_Rect() {
		return SDL_Rect{
			int(w),
			int(h),
			int(x),
			int(y)
		};
	};
	VectorRect(SDL_Rect o) {
		{o.x, o.y, o.w, o.h; };
	}
	VectorRect operator +(VectorRect other) {
		return {
			this->x + other.x,
			this->y + other.y,
			this->w + other.w,
			this->h + other.h
		};
	};
	VectorRect operator -(VectorRect other) {
		return {
			this->x - other.x,
			this->y - other.y,
			this->w - other.w,
			this->h - other.h
		};
	};
	VectorRect operator /(VectorRect other) {
		return {
			this->x / other.x,
			this->y / other.y,
			this->w / other.w,
			this->h / other.h
		};
	};
	VectorRect operator *(VectorRect other) {
		return {
			this->x * other.x,
			this->y * other.y,
			this->w * other.w,
			this->h * other.h
		};
	};


	static bool lu_intersects(VectorRect a, VectorRect b) {
		bool state = false;
		SDL_Rect ra = a;
		SDL_Rect rb = b;
		return SDL_HasIntersection(&ra, &rb);
	}

	static bool checkCollision(VectorRect* a, VectorRect* b)
	{
		return SDL_HasIntersection((SDL_Rect*)a, (SDL_Rect*)b);
	}
	static bool checkCollision(SDL_Rect* a, SDL_Rect* b)
	{
		return SDL_HasIntersection(a, b);
	}
	friend std::ostream& operator<<(std::ostream& os, const VectorRect& o)
	{
		os << o.x << ", " << o.y << " (" << o.w << ':' << o.h << ")";
		return os;
	}
};
SDL_Rect VectorRect::_emptyRect = SDL_Rect{ 0, 0, 0, 0 };
struct Vector3 {
	Vector x = 0;
	Vector y = 0;
	Vector z = 0;
	Vector3() {}
	Vector3(Vector _x, Vector _y, Vector _z) {
		x = _x;
		y = _y;
		z = _z;
	}
	Vector3 operator +=(Vector3 other) {
		this->x = this->x + other.x;
		this->y = this->y + other.y;
		this->z = this->z + other.z;
		return *this;
	}
};

struct Transform {
	static void recalcScale(Vector2* base_sc, Vector2 scale) {
		base_sc->x *= scale.x;
		base_sc->y *= scale.y;
	}
	Vector2 position = {0,0};
	// scale in pixels! To set via a percentage, use Transform::recalcScale(&tr_t.scale, {amt_x, amt_y});
	Vector2 scale = {0,0};
	Vector2 origin = {0,0};
	Vector angle = 0;
};
#endif