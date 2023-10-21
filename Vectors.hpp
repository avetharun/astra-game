#ifndef __cw_vector_hpp
#define __cw_vector_hpp
#include <nlohmann/json/json.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include "LUA_INCLUDE.h"
#include "utils.hpp"
#include "imgui/imgui.h"
#include <SDL2/SDL.h>
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


struct Vector2 {
	static Vector2* lu_new(int x, int y) {
		Vector2* _n = new Vector2();
		_n->x = x;
		_n->y = y;
		return _n;
	}
	double gx() { return this->x; }
	double gy() { return this->y; }
	double sx(int v = INT32_MAX) { this->x = v; return this->x; }
	double sy(int v = INT32_MAX) { this->y = v; return this->y; }
	Vector2* lu_get() { return this; }
	double x = 0; 
	double y = 0;
	Vector2() {}
	Vector2(SDL_Rect r) {
		x = r.x; y = r.y;
	}
	Vector2(double _x, double _y) {
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
	Vector2 operator -(double other) {
		return { x - other, y - other };
	}
	Vector2 operator +(double other) {
		return { x + other, y + other };
	}
	Vector2 operator /(double other) {
		return { x / other, y / other };
	}
	Vector2 operator *(double other) {
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
	bool operator ==(double other) {
		return (x == other && y == other) ? true : false;
	}
	bool operator > (double other) {
		return (x > other && y > other) ? true : false;
	}
	bool operator < (double other) {
		return (x < other && y < other) ? true : false;
	}
	static Vector2 parse_angle(float angle) {
		Vector2 i(
			(1* cos(alib_deg2rad(angle))),
			(1* sin(alib_deg2rad(angle)))
		);
		printf("%f, %f", i.x, i.y);
		return i;
	}
	static float parse_angle_vec(Vector2 v) {
		return atan2f(v.y, v.x) * 180 / M_PI;
	}
	double luaL_getx() { return x; }
	double luaL_gety() { return y; }
	Vector2 luaL__add(Vector2 rhs) { return *this + rhs; }
	Vector2 luaL__sub(Vector2 rhs) { return *this - rhs; }
	Vector2 luaL__mul(Vector2 rhs) { return *this * rhs; }
	Vector2 luaL__div(Vector2 rhs) { return *this / rhs; }
	bool luaL__eq(Vector2 rhs) { return (* this == rhs); }
	bool luaL__lt(Vector2 rhs) { return (*this < rhs); }
	bool luaL__le(Vector2 rhs) { return (*this <= rhs); }
	Vector2 luaL__unm() { return -*this; }

	static Vector2 up;
	static Vector2 right;
	static Vector2 down;
	static Vector2 left;
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
	operator SDL_Point () { return {(int)x,(int)y}; }
	operator ImVec2 () { return { (float)x,(float)y }; }
	static Vector2 distance(Vector2 one, Vector2 two) {
		return one - two;
	}
	static double distancef(Vector2 a, Vector2 b) {
		return sqrtf(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
	}
	static double distances(Vector2 a, Vector2 b) {
		return pow(a.x - b.x, 2) + pow(a.y - b.y, 2);
	}
	double magnitude() { return sqrt((x * x) + (y * y)); }
	Vector2 normalize() {
		float mag = magnitude();
		if (mag > 0.001f)
			return *this / mag;
		else
			return {0,0};
	}
	static double cross(Vector2 lhs, Vector2 rhs) {
		return (lhs.x * rhs.y) - (lhs.y * rhs.x);
	}
	static double dot(Vector2 lhs, Vector2 rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }
	static float angle(Vector2 a, Vector2 b) {
		return alib_rad2deg(atan2(b.y - a.y, b.x - a.x));
	}
	Vector2 clamp_magnitude(double maxLength) {
		double sqrmag = sqrt(this->magnitude());
		if (sqrmag > maxLength * maxLength)
		{
			float mag = (float)sqrt(sqrmag);
			//these intermediate variables force the intermediate result to be
			//of float precision. without this, the intermediate result can be of higher
			//precision, which changes behavior.
			float normalized_x = x / mag;
			float normalized_y = y / mag;
			return { normalized_x * maxLength,
				normalized_y * maxLength,
			};
		}
		return *this;
	}
	void operator ~() {
		delete[] this;
	}

	static Vector2 lu_invert(Vector2 v) { return -v; }
	static Vector2 lu_abs(Vector2 v) { return {abs(v.x), abs(v.y)}; }
};

Vector2 Vector2::up = Vector2{ 0,-1 };
Vector2 Vector2::right = Vector2{ 1,0 };
Vector2 Vector2::down = Vector2{ 0,1 };
Vector2 Vector2::left = Vector2{ -1,0 };
struct VectorRect {
	double x = 0;
	double y = 0;
	double w = 1;
	double h = 1;
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
	double sx(double v = INT32_MAX) { if (v == INT32_MAX) { return this->x; } this->x = v; return this->x; }
	double sy(double v = INT32_MAX) { if (v == INT32_MAX) { return this->y; } this->y = v; return this->y; }
	double sw(double v = INT32_MAX) { if (v == INT32_MAX) { return this->w; } this->w = v; return this->w; }
	double sh(double v = INT32_MAX) { if (v == INT32_MAX) { return this->h; } this->h = v; return this->h; }

	double gx() { return this->x; }
	double gy() { return this->y; }
	double gw() { return this->w; }
	double gh() { return this->h; }
	
	Vector2 gsz() { return {this->w, this->h}; }
	Vector2 gpos() { return { this->w, this->h }; }
	Vector2 spos(Vector2 __pos) { this->x = __pos.x; this->y = __pos.y; }
	Vector2 ssz(Vector2 __siz) { this->w = __siz.x; this->h = __siz.y; }
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
		w = _w; h = _h; x= _x; y= _y;
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

	Vector2 middle_point() {

		Vector2 midPoint;
		double mpx = x + 0.5 * w;
		double mpy = y + 0.5 * h;
		midPoint.x = (mpx);
		midPoint.y = (mpy);
		return(midPoint);
	}
	void scale(double amount) {
		Vector2 _mp = this->middle_point();

		w = amount * w;
		h = amount * h;
		// make sure width/height is non-zero, as collision will fail if it is.
		if (w == 0) { w = 1; }
		if (h == 0) { h = 1; }
		x = (_mp.x - 0.5 * w);
		y = (_mp.y - 0.5 * h);
	}
};
SDL_Rect VectorRect::_emptyRect = SDL_Rect{ 0, 0, 0, 0 };
struct Vector3 {
	double x = 0;
	double y = 0;
	double z = 0;
	Vector3() {}
	Vector3(double _x, double _y, double _z) {
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
	double angle = 0;
};
#endif