#pragma once

#include <iostream>
typedef int Vector;
struct Vector2 {
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
	Vector2 operator -(int other) {
		return { x - other, y - other };
	}
	Vector2 operator +(int other) {
		return { x + other, y + other };
	}
	Vector2 operator /(int other) {
		return { x / other, y / other };
	}
	Vector2 operator *(int other) {
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
	bool operator ==(int other) {
		return (x == other && y == other) ? true : false;
	}
	bool operator > (int other) {
		return (x > other && y > other) ? true : false;
	}
	bool operator < (int other) {
		return (x < other && y < other) ? true : false;
	}
	static Vector2 up;
	static Vector2 right;
	friend std::ostream& operator<<(std::ostream& os, const Vector2& o)
	{
		os << o.x << ", " << o.y;
		return os;
	}
	static Vector2 distance(Vector2 one, Vector2 two) {
		return one - two;
	}
};

Vector2 Vector2::up = Vector2{ 0,1 };
Vector2 Vector2::right = Vector2{ 1,0 };
struct VectorRect {
	Vector w = 1;
	Vector h = 1;
	Vector x = 0;
	Vector y = 0;
	VectorRect() {}
	VectorRect(int _w, int _h, int _x, int _y) {
		w = _w; h = _h; x = _x; y = _y;
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
	Vector2 operator +(Vector2 other) {

	};

	static bool checkCollision(VectorRect* a, VectorRect* b)
	{
		return SDL_HasIntersection((SDL_Rect*)a, (SDL_Rect*)b);;
	}
	static bool checkCollision(SDL_Rect* a, SDL_Rect* b)
	{
		return SDL_HasIntersection(a, b);;
	}
	friend std::ostream& operator<<(std::ostream& os, const VectorRect& o)
	{
		os << o.x << ", " << o.y << " (" << o.w << ':' << o.h << ")";
		return os;
	}
};
struct Vector3 {
	Vector x;
	Vector y;
	Vector z;
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
	Vector2 position;
	Vector angle = 0;
	SDL_Rect* rect;
};
