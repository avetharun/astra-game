#ifndef PHYSICS_HPP 
#define PHYSICS_HPP
#pragma once
#include <functional>
#include "Vectors.hpp"
#include "utils.hpp"
#ifndef ENGINE_HPP
#include "engine.hpp"
#endif


enum _ColIDs : unsigned char{
	COL_EMPTY =			B8(00000000),
	COL_PLAYER =		B8(00000010),
	COL_ENT =			B8(00001000),
	COL_OBJ =			B8(00010000),
	COL_TRG =			B8(00100000),
	COL_SOLID =			B8(01000000)
};
#include <chrono>
struct MeshLine {
	Vector2 start;
	Vector2 end;
	unsigned char layer;
};

struct MeshCollider2d {

	std::function< void(MeshCollider2d*) > OnColliderHit = [](MeshCollider2d* other) {};
	std::vector<MeshLine> lines;
	static std::vector<MeshCollider2d*> _mGlobalColArr;
	MeshCollider2d(std::vector<MeshLine> Lines, int Layer = 1) {
		lines = Lines; _mGlobalColArr.push_back(this);
	}
	MeshCollider2d() {
		_mGlobalColArr.push_back(this);
	}
	void operator ~() {
		auto position = std::find(_mGlobalColArr.begin(), _mGlobalColArr.end(), this);
		if (position != _mGlobalColArr.end()) // Make sure it exists first!
			_mGlobalColArr.erase(position); // Remove the collider from global array.
		_mGlobalColArr.shrink_to_fit();
		lines.clear();
	}
	void free() { ~*this; }

};

std::vector<MeshCollider2d*> MeshCollider2d::_mGlobalColArr = std::vector<MeshCollider2d*>();
struct RectCollider2d {

	int layer = -1;
	SDL_Rect* rect = {};
	std::function< void(RectCollider2d* ) > OnColliderHit = [](RectCollider2d* other) {};
	static std::vector<RectCollider2d*> _mGlobalColArr;
	RectCollider2d(SDL_Rect* Rect, int Layer = 1) {
		rect = Rect;
		if (rect == nullptr) {
			rect = new SDL_Rect();
		}
		_mGlobalColArr.push_back(this);
		layer = Layer;
	}
	RectCollider2d() {
		_mGlobalColArr.push_back(this);
	}
	void operator ~() {
		auto position = std::find(_mGlobalColArr.begin(), _mGlobalColArr.end(), this);
		if (position != _mGlobalColArr.end()) // Make sure it exists first!
			_mGlobalColArr.erase(position); // Remove the collider from global array.
		_mGlobalColArr.shrink_to_fit();
	}
	void free() { ~*this; }
private:

	bool isStayState=false;
	bool isEnterState=false;
	bool isExitState=false;

public:


};
std::vector<RectCollider2d*> RectCollider2d::_mGlobalColArr = std::vector<RectCollider2d*>();






/**
 *  \brief Raycast a "line"
 *	\param start (vec2) : Where raycast starts
 *	\param end   (vec2) : Where raycast ends
 *
 *  \param ..
 */
struct Raycast2D {

	struct RaycastHit {
		Vector2 pos;
		void* object;
		Vector2 start;
		Vector2 end;
		float length;
		bool hasHit = false;
		operator bool() {
			return hasHit;
		}
	}; 
	static inline RaycastHit hit{};

	/* \brief Check if point (pos) is in rect (&rect)*/
	static bool pointRect(SDL_Rect* rect, Vector2 pos)
	{
		if (pos.x > rect->x && pos.x < rect->w && pos.y > rect->y && pos.y < rect->h)
			return true;

		return false;
	}

	// visit http://www.jeffreythompson.org/collision-detection/line-rect.php for help, this site really nice, yknow
private:

	// LINE/RECTANGLE
	static bool lineRect(Vector2 start, Vector2 end, SDL_Rect* rect) {
		
		
		// check if the line has hit any of the rectangle's sides
		
		
		// uses the Line/Line function below
		bool left = lineLine(start.x, start.y, end.x, end.y, rect->x, rect->y, rect->x, rect->y + rect->h);
		bool right = lineLine(start.x, start.y, end.x, end.y, rect->x + rect->w, rect->y, rect->x + rect->w, rect->y + rect->h);
		bool top = lineLine(start.x, start.y, end.x, end.y, rect->x, rect->y, rect->x + rect->w, rect->y);
		bool bottom = lineLine(start.x, start.y, end.x, end.y, rect->x, rect->y + rect->h, rect->x + rect->w, rect->y + rect->h);
		// use conditionals to disable reassigning hit.pos (see lineLine)
		

		// if ANY of the above are true, the line
		// has hit the rectangle
		if (left || right || top || bottom) {
			return true;
		}
		return false;
	}
	static bool lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

		// calculate the direction of the lines
		float uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
		float uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

		// if uA and uB are between 0-1, lines are colliding
		if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {

			// set hit.pos to where line hit
			float intersectionX = x1 + (uA * (x2 - x1));
			float intersectionY = y1 + (uA * (y2 - y1));
			hit.pos = Vector2(intersectionX, intersectionY);
			return true;
		}
		return false;
	}
	static bool lineLineV(Vector2 A, Vector2 B, Vector2 c, Vector2 d) {
		return lineLine(A.x, A.y, B.x, B.y, c.x, c.y, d.x, d.y);
	}
public:
	/** \brief Test to see if line hits. 
	*/
	static bool Test(Vector2 start, Vector2 end, int32_t layer, bool overridedebug = false) {
		hit.hasHit = false;
		for (unsigned int li = 0; li < MeshCollider2d::_mGlobalColArr.size(); li++) {
			MeshCollider2d* m = MeshCollider2d::_mGlobalColArr[li];
			for (size_t lo = 0; lo < m->lines.size(); lo++) {
				MeshLine l = m->lines[lo];
				if (!(l.layer & layer)) {
					continue;
				}
				Vector2 s = l.start + -*Camera::GetInstance()->m_target;
				Vector2 e = l.end + -*Camera::GetInstance()->m_target;
				bool o = lineLineV(start, end, s, e);
				if (o) {
					hit.object = MeshCollider2d::_mGlobalColArr[li];
					hit.start = start;
					hit.end = end;
					hit.hasHit = true; 
					break;
				}
			}
		}
		for (unsigned int i = 0; i < RectCollider2d::_mGlobalColArr.size() && !hit.hasHit; i++) {
			if (RectCollider2d::_mGlobalColArr[i]->layer == layer || layer == -1) {
				bool o = lineRect(start, end, RectCollider2d::_mGlobalColArr[i]->rect);
				if (o) {
					hit.object = RectCollider2d::_mGlobalColArr[i];
					hit.start = start;
					hit.end = end;
					hit.hasHit = true;
					break;
				}
			}
		}
		return hit.hasHit;
	};
	/** \brief Test to see if line hits. */
	static bool TestRelative(Vector2 start, Vector2 offset, Vector length, int32_t layer, bool overridedebug = false) {
		hit.hasHit = false;
		Vector2 end = (start.x <= 0 || start.y <= 0) ?
			(offset * length) :
			(offset * -length)
			+ start + offset;
		for (unsigned int li = 0; li < MeshCollider2d::_mGlobalColArr.size(); li++) {
			MeshCollider2d* m = MeshCollider2d::_mGlobalColArr[li];
			for (size_t lo = 0; lo < m->lines.size(); lo++) {
				MeshLine l = m->lines[lo];
				if (!(l.layer & layer)) {
					continue;
				}
				Vector2 s = l.start + -*Camera::GetInstance()->m_target;
				Vector2 e = l.end + -*Camera::GetInstance()->m_target;
				bool o = lineLineV(start, end, s, e);
				if (o) {
					hit.object = MeshCollider2d::_mGlobalColArr[li];
					hit.start = start;
					hit.end = end;
					hit.hasHit = true;
					break;
				}
			}
		}
		for (unsigned int i = 0; i < RectCollider2d::_mGlobalColArr.size() && !hit.hasHit; i++) {
			if (RectCollider2d::_mGlobalColArr[i]->layer == layer || layer == -1) {
				bool o = lineRect(start, end, RectCollider2d::_mGlobalColArr[i]->rect);
				if (o) {
					hit.object = RectCollider2d::_mGlobalColArr[i];
					hit.start = start;
					hit.end = end;
					hit.hasHit = true;
					break;
				}
			}
		}
		return hit.hasHit;
	}; 

	static bool TestCircle(Vector2 start, int diameter, int32_t layer, int speed = 1000) {
		for (float a = 0; a < 360; a+=.05f*speed ) {
			Vector2 i(
				// multiply angle by 57.296*f (see https://www.desmos.com/calculator/bjtb0ojtqk for an explanation)
				start.x + (diameter * cos(a / 57.296f)),
				start.y + (diameter * sin(a / 57.296f))
			);
			if (Test(start, i, layer)) {
				return true;
			}
		}
		return false;
	
	}
	static bool TestCone(Vector2 start, int angle, int offset, int dist, int32_t layer, int speed = 1000) {
		for (float a = 0 - angle+offset; a < angle+offset; a+=.05f*speed ) {
		
			Vector2 i(
				// divide angle by 57.296*f in sin-cos functions. (see https://www.desmos.com/calculator/jgv7e1ecut for an explanation)
				start.x+(dist*cos(a / 57.296f)),
				start.y+(dist*sin(a / 57.296f))
			);
			if (Test(start, i, layer)) {
				return true;
			}
		}
		return false;
	}


};


struct Collider {
	typedef MeshLine Line;
	typedef Raycast2D Raycast;
	typedef Raycast2D::RaycastHit Hit;
	typedef RectCollider2d Rect;
	typedef MeshCollider2d Mesh;
};


#endif
