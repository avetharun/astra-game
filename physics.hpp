#ifndef PHYSICS_HPP 
#define PHYSICS_HPP

#include <functional>
#include "Vectors.hpp"
#include "utils.hpp"
#ifndef ENGINE_HPP
#include "engine.hpp"
#endif
#include "lua.hpp"
#ifndef cwlib_cwl_error_handler_hpp
#include "cwlib/cwerror.h"
#endif
enum _collLayers : int{
	COL_EMPTY =			B8(00000001),
	COL_PLAYER =		B8(00000010),
	COL_CENTER =		B8(00000100),
	COL_STATIC =		B8(00000100),
	COL_ENT =			B8(00001000),
	COL_OBJ =			B8(00010000),
	COL_TRG =			B8(00100000),
	COL_SOLID =			B8(01000000),
	COL_FUNC =			B8(10000000)
};
const int hrtocol(std::string id) {

	if (alib_streq(id, "COL_ENT")) { return COL_ENT; }
	if (alib_streq(id, "COL_OBJ")) { return COL_OBJ; }
	if (alib_streq(id, "COL_TRG")) { return COL_TRG; }
	if (alib_streq(id, "COL_SOLID")) { return COL_SOLID; }
	if (alib_streq(id, "COL_CENTER")) { return COL_CENTER; }
	if (alib_streq(id, "COL_STATIC")) { return COL_STATIC; }
	if (alib_streq(id, "COL_FUNC")) { return COL_FUNC; }
	if (alib_streq(id, "COL_PLAYER")) { return COL_PLAYER; }
	if (alib_streq(id, "COL_EMPTY")) { return COL_EMPTY; }
	return COL_SOLID;
}
const char* coltohr(unsigned char id) {
	switch (id) {
	default:
	case COL_EMPTY:
		return "COL_EMPTY";
	case COL_PLAYER:
		return "COL_PLAYER";
	case COL_ENT:
		return "COL_ENT";
	case COL_OBJ:
		return "COL_OBJ";
	case COL_TRG:
		return "COL_TRG";
	case COL_SOLID:
		return "COL_SOLID";
	case COL_FUNC:
		return "COL_FUNC";
	case COL_CENTER:
		return "COL_CENTER";
	}
}
#include <chrono>
struct MeshCollider2d; struct RectCollider2d;
bool __phys_is_debug;
SDL_Renderer* __phys_internal_renderer;
void initRenderer__PHYS(SDL_Renderer* r) { __phys_internal_renderer = r; }


struct MeshLine {
	bool is_editing = false;
	static SDL_Rect bounding_boxi(int sx, int sy, int ex, int ey) { 
		VectorRect _r = {
			alib_min(sx, ex) - 2,
			alib_min(sy, ey) - 2,
			alib_max(4,abs(ex - sx)),
			alib_max(4,abs(ey - sy))
		};
		_r.scale(2);
		return {
			static_cast<int>(_r.x), static_cast<int>(_r.y), static_cast<int>(_r.w), static_cast<int>(_r.h)
		};
	}
	static SDL_Rect bounding_box(MeshLine m) {
		return bounding_boxi(m.start.x, m.start.y, m.end.x, m.end.y);
	}
	
	Vector2 start = {};
	Vector2 end = {};
	Vector2 midpoint() {
		return { (start.x + end.x) * 0.5f, (start.y + end.y) * 0.5f };
	}
	static Vector2 midpoint(Vector2 s, Vector2 e) {
		return { (s.x + e.x) * 0.5f, (s.y + e.y) * 0.5f };
	}
	int layer = COL_EMPTY;
	std::string id = "";
	static inline const int ID_LENGTH = 48;
	int coll_id = 0;
	static MeshLine* lu_new(double x1, double y1, double x2, double y2, int _layer) {
		MeshLine* _m = new MeshLine();
		_m->start = { x1, y1 };
		_m->end = { x2, y2 };
		_m->layer = _layer;
		return _m;
	}
	MeshLine() {}
	MeshLine(Vector2 _s, Vector2 _e) {
		this->start = _s; this->end = _e;
	}
	MeshLine(double x1, double y1, double x2, double y2) {
		this->start = { x1, y1 };
		this->end = { x2, y2 };
	}

	Vector2* lu_getStart() { return this->start.lu_get(); }
	Vector2* lu_getEnd() { return this->end.lu_get(); }
	int* lu_getLayer() { return &this->layer; }
	int* lu_getID() { return &this->coll_id; }
	void lu_setID(uint64_t _id) { this->coll_id = _id; }
	void lu_setLayer(unsigned char _l) { this->layer = _l; }
	void lu_setStart(Vector2 _start) { this->start = _start; }
	void lu_setEnd(Vector2 _end) { this->end = _end; }
	void set_freestanding();
	void erase_freestanding();
	void __gc() {
		
	}
};



struct MeshCollider2d {
	bool isCWLScriptable = false;
	std::function< void(MeshLine*) > OnColliderHit = [](MeshLine* other) {};

	static size_t lu_gsz() { return _mGlobalColArr.size(); }
	static std::string dump_debug() {
		return alib_strfmt(R"(
--RectCollider2d--
Amount of elements: %zi
Memory allocated: %zi bytes
)", lu_gsz(), (lu_gsz() * sizeof(MeshCollider2d)));
	}
	void c_collider_hit(MeshLine* o) {
		//if (SUCCEEDED(&lua_colhit)) { lua_colhit.execute(o); }
		OnColliderHit(o);
	}
	std::vector<MeshLine*> lines;
	static std::vector<MeshCollider2d*> _mGlobalColArr;
	static MeshCollider2d* freestandingLinesMesh;
	MeshCollider2d(std::vector<MeshLine*> Lines, int Layer = 1) {
		lines = Lines; _mGlobalColArr.push_back(this);
	}
	MeshCollider2d() {
		_mGlobalColArr.push_back(this);
	}
	static MeshCollider2d* lu_new(std::vector<MeshLine*> __lines) {
		MeshCollider2d* _this = new MeshCollider2d();
		size_t amt = __lines.size();
		for (int i = 0; i < amt; i++) {
			// copy
			MeshLine __cur_of = *__lines[i];
			_this->lines.push_back(&__cur_of);
		}
		return _this;
	}
	void operator ~() {
		alib_remove_any_of(_mGlobalColArr, this);
	}
	void free() { ~*this; }
};
std::vector<MeshCollider2d*> MeshCollider2d::_mGlobalColArr = std::vector<MeshCollider2d*>();
MeshCollider2d* MeshCollider2d::freestandingLinesMesh = new MeshCollider2d();

void MeshLine::set_freestanding() { 
	if (MeshCollider2d::freestandingLinesMesh->lines.size() >= 6000) {
		printf("Standalone lines are slow, please consider creating a mesh collider instead!");
	}
	MeshCollider2d::freestandingLinesMesh->lines.push_back(this); 
}
void MeshLine::erase_freestanding() {
	alib_remove_any_of(MeshCollider2d::freestandingLinesMesh->lines, this);
}

struct RectCollider2d {
	bool centered = false;
	bool isCWLScriptable = false;
	int layer = -1;
	uint64_t coll_id;
	SDL_Rect* ws_rect = new SDL_Rect();
	SDL_Rect* rect = new SDL_Rect();

	std::string id = "";
	static inline const int ID_LENGTH = 48;

	SDL_Rect* lu_getRect() {
		return rect;
	}
	static size_t lu_gsz() { return _mGlobalColArr.size(); }
	static std::string dump_debug() {
		return alib_strfmt(R"(
--RectCollider2d--
Amount of elements: %zi
Memory allocated: %zi bytes
)", lu_gsz(), (lu_gsz() * sizeof(RectCollider2d)));
	}
	int lu_gx() {return this->rect->x;}
	int lu_gy() {return this->rect->y;}
	int lu_gw() {return this->rect->w;}
	int lu_gh() {return this->rect->h;}

	int lu_sx(int v) {this->rect->x=v;}
	int lu_sy(int v) {this->rect->y=v;}
	int lu_sw(int v) {this->rect->w=v;}
	int lu_sh(int v) {this->rect->h=v;}

	static SDL_Rect recalc(RectCollider2d out) {
		if (Camera::GetInstance()->m_target == nullptr) { return *out.rect; } // Camera hasn't been put to a target. It'll inherit world space until it does.
		SDL_Rect m_out{0,0,out.ws_rect->w, out.ws_rect->h};
		if (out.centered || out.layer == COL_CENTER) {
			m_out.x = ((int)( ( Camera::GetInstance()->m_Viewport.x * 0.5f) - (out.ws_rect->w * 0.5f)) + Camera::GetInstance()->m_Offset.x);
			m_out.y = ((int)( ( Camera::GetInstance()->m_Viewport.y * 0.5f) - (out.ws_rect->h * 0.5f)) + Camera::GetInstance()->m_Offset.y);
		}
		else {
			m_out.x = -Camera::GetInstance()->m_target->x + Camera::GetInstance()->m_Offset.x + out.ws_rect->x;
			m_out.y = -Camera::GetInstance()->m_target->y + Camera::GetInstance()->m_Offset.y + out.ws_rect->y;
		}
		return m_out;
	}
	static std::function< void(RectCollider2d*, RectCollider2d*) > OnColliderHit;
	static std::function< void(RectCollider2d*, MeshLine*) > OnLineHitRect;

	void recalc() {
		*this->rect = recalc(*this);
	}

	static RectCollider2d* lu_new_fromr(VectorRect r, int __layer) {
		RectCollider2d* r_re = new RectCollider2d();
		*r_re->rect = { (int)r.x, (int)r.y, (int)r.w, (int)r.h };
		*r_re->ws_rect = { (int)r.x, (int)r.y, (int)r.w, (int)r.h };
		r_re->recalc();
		r_re->layer = __layer;
		return r_re;
	}
	static RectCollider2d* lu_new_fromi(int x, int y, int w, int h, int __layer) {
		RectCollider2d* r_re = new RectCollider2d();
		*r_re->rect = { x, y, w, h };
		*r_re->ws_rect = { x, y, w, h };
		r_re->recalc();
		r_re->layer = __layer;
		return r_re;
	}

	static RectCollider2d* lu_new_fromic(int x, int y, int w, int h, int __layer) {
		RectCollider2d* r_re = new RectCollider2d();
		*r_re->rect = { x, y, w, h };
		*r_re->ws_rect = { x, y, w, h };
		r_re->recalc();
		r_re->layer = __layer;
		r_re->centered = true;
		return r_re;
	}

	void c_collider_hit(RectCollider2d* o) {
		//if (SUCCEEDED(&lua_colhit)) { lua_colhit.execute(o); }
		OnColliderHit(this, o);
	}

	static std::vector<RectCollider2d*> _mGlobalColArr;
	RectCollider2d(SDL_Rect* Rect, int Layer = 1) {
		*rect = { Rect->x, Rect->y, Rect->w, Rect->h };
		*ws_rect = {Rect->x, Rect->y, Rect->w, Rect->h};
		if (ws_rect == nullptr) {
			ws_rect = new SDL_Rect();
		}
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
	static bool RectIntersects(SDL_Rect* r1, SDL_Rect* r2) {
		SDL_Rect _r = {};
		return SDL_IntersectRect(r1, r2, &_r);
	}
	struct RaycastHit {
		static inline Vector2 pos = {};
		static inline void* object = nullptr;
		static inline Vector2 start = {};
		static inline Vector2 end = {};
		static inline float length = 0;
		static inline bool hasHit = false;
		static inline std::string col_id = "";
		static inline int layer = 0;
	}; 

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
		int x1 = start.x, y1 = start.y, x2 = end.x, y2 = end.y;
		return SDL_IntersectRectAndLine(rect, &x1, &y1, &x2, &y2);
		/*
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
		return false;*/
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
			RaycastHit::pos = Vector2(intersectionX, intersectionY);
			return true;
		}
		return false;
	}
public:
	static bool TestLineImplV(Vector2 start1, Vector2 end1, Vector2 start2, Vector2 end2) {
		return lineLine(start1.x, start1.y, end1.x, end1.y, start2.x, start2.y, end2.x, end2.y);
	}
	static bool TestLineImpl(MeshLine first, MeshLine second) {
		return lineLine(first.start.x, first.start.y, first.end.x, first.end.y, second.start.x, second.start.y, second.end.x, second.end.y);
	}
	/** \brief Test to see if line hits. 
	*/
	static bool Test(Vector2 start, Vector2 end, int layer) {
		RaycastHit::hasHit = false;
		//SDL_SetRenderDrawColor(__phys_internal_renderer, 255, 64, 255, 255);
		//SDL_RenderDrawLine(__phys_internal_renderer, start.x, start.y, end.x, end.y);
		for (unsigned int i = 0; i < RectCollider2d::_mGlobalColArr.size() && !RaycastHit::hasHit; i++) {
			RectCollider2d* _thiscol = RectCollider2d::_mGlobalColArr[i];
			if (_thiscol->layer& layer || layer == -1){
				_thiscol->recalc();
				//printf("Calculating rect with collider type %s : %d\n", coltohr(RectCollider2d::_mGlobalColArr[i]->layer), i);
				SDL_Rect* r = _thiscol->rect;
				SDL_Rect lr = MeshLine::bounding_boxi(start.x, start.y, end.x, end.y);
				bool o = lineRect(start, end, r);
				if (o) {
					RaycastHit::object = _thiscol;
					RaycastHit::start = start;
					RaycastHit::end = end;
					RaycastHit::hasHit = true;
					RaycastHit::col_id = _thiscol->id;
					RaycastHit::layer = _thiscol->layer;
					return true;
				}
			}
		}
		for (unsigned int li = 0; li < MeshCollider2d::_mGlobalColArr.size(); li++) {
			MeshCollider2d* m = MeshCollider2d::_mGlobalColArr[li];
			for (size_t lo = 0; lo < m->lines.size(); lo++) {
				MeshLine l = *m->lines[lo];
				if (!(l.layer & layer) && (layer != -1)) {
					continue;
				}
				Vector2 s = l.start - *Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
				Vector2 e = l.end - *Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
				SDL_Rect lr = MeshLine::bounding_boxi(s.x, s.y, e.x, e.y );
				SDL_Rect c_lr = MeshLine::bounding_boxi(start.x, start.y, end.x, end.y);
				if (__phys_is_debug) {
					SDL_SetRenderDrawColor(__phys_internal_renderer, 255, 255, 255, 32);
					SDL_RenderDrawRect(__phys_internal_renderer, &lr);
					SDL_SetRenderDrawColor(__phys_internal_renderer, 128, 255, 128, 32);
					SDL_RenderDrawRect(__phys_internal_renderer, &c_lr);
					SDL_SetRenderDrawColor(__phys_internal_renderer, 128, 255, 255, 128);
					SDL_RenderDrawLine(__phys_internal_renderer, s.x, s.y, e.x, e.y);
				}
				if (!VectorRect::checkCollision(&c_lr, &lr)) { continue; }
				
				bool o = TestLineImplV(start, end, s, e);
				if (o) {
					RaycastHit::object = MeshCollider2d::_mGlobalColArr[li];
					RaycastHit::start = start;
					RaycastHit::end = end;
					RaycastHit::hasHit = true;
					RaycastHit::col_id = l.id;
					RaycastHit::layer = l.layer;
					return true;
				}
			}
		}
		return false;
	};
	static bool TestExcept(Vector2 start, Vector2 end, int layer) {
		RaycastHit::hasHit = false;
		//SDL_SetRenderDrawColor(__phys_internal_renderer, 255, 64, 255, 255);
		//SDL_RenderDrawLine(__phys_internal_renderer, start.x, start.y, end.x, end.y);
		for (unsigned int i = 0; i < RectCollider2d::_mGlobalColArr.size() && !RaycastHit::hasHit; i++) {
			RectCollider2d* _thiscol = RectCollider2d::_mGlobalColArr[i];
			if (_thiscol->layer == layer || _thiscol->layer == COL_CENTER) {
				continue;
			}
				_thiscol->recalc();
				//printf("Calculating rect with collider type %s : %d\n", coltohr(RectCollider2d::_mGlobalColArr[i]->layer), i);
				SDL_Rect* r = _thiscol->rect;
				SDL_Rect lr = MeshLine::bounding_boxi(start.x, start.y, end.x, end.y);
				bool o = lineRect(start, end, r);
				if (o) {
					RaycastHit::object = _thiscol;
					RaycastHit::start = start;
					RaycastHit::end = end;
					RaycastHit::hasHit = true;
					RaycastHit::col_id = _thiscol->id;
					RaycastHit::layer = _thiscol->layer;
					return true;
				}
		}
		for (unsigned int li = 0; li < MeshCollider2d::_mGlobalColArr.size(); li++) {
			MeshCollider2d* m = MeshCollider2d::_mGlobalColArr[li];
			for (size_t lo = 0; lo < m->lines.size(); lo++) {
				MeshLine l = *m->lines[lo];
				if (l.layer == layer || l.layer == COL_CENTER) {
					continue;
				}
				Vector2 s = l.start - *Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
				Vector2 e = l.end - *Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
				SDL_Rect lr = MeshLine::bounding_boxi(s.x, s.y, e.x, e.y);
				SDL_Rect c_lr = MeshLine::bounding_boxi(start.x, start.y, end.x, end.y);
				if (__phys_is_debug) {
					SDL_SetRenderDrawColor(__phys_internal_renderer, 255, 255, 255, 32);
					SDL_RenderDrawRect(__phys_internal_renderer, &lr);
					SDL_SetRenderDrawColor(__phys_internal_renderer, 128, 255, 128, 32);
					SDL_RenderDrawRect(__phys_internal_renderer, &c_lr);
					SDL_SetRenderDrawColor(__phys_internal_renderer, 128, 255, 255, 128);
					SDL_RenderDrawLine(__phys_internal_renderer, s.x, s.y, e.x, e.y);
				}
				if (!VectorRect::checkCollision(&c_lr, &lr)) { continue; }

				bool o = TestLineImplV(start, end, s, e);
				if (o) {
					RaycastHit::object = MeshCollider2d::_mGlobalColArr[li];
					RaycastHit::start = start;
					RaycastHit::end = end;
					RaycastHit::hasHit = true;
					RaycastHit::col_id = l.id;
					RaycastHit::layer = l.layer;
					return true;
				}
			}
		}
		return false;
	};
	static bool TestRect(Vector2 start, Vector2 end, int layer) {
		RaycastHit::hasHit = false;
		//SDL_SetRenderDrawColor(__phys_internal_renderer, 255, 64, 255, 255);
		//SDL_RenderDrawLine(__phys_internal_renderer, start.x, start.y, end.x, end.y);
		for (unsigned int i = 0; i < RectCollider2d::_mGlobalColArr.size() && !RaycastHit::hasHit; i++) {
			RectCollider2d* _thiscol = RectCollider2d::_mGlobalColArr[i];
			if (_thiscol->layer & layer || layer == -1) {
				_thiscol->recalc();
				//printf("Calculating rect with collider type %s : %d\n", coltohr(RectCollider2d::_mGlobalColArr[i]->layer), i);
				SDL_Rect* r = _thiscol->ws_rect;
				SDL_Rect lr = MeshLine::bounding_boxi(start.x, start.y, end.x, end.y);
				bool o = RectIntersects(r, &lr);
				if (o) {
					RaycastHit::object = _thiscol;
					RaycastHit::start = start;
					RaycastHit::end = end;
					RaycastHit::hasHit = true;
					RaycastHit::col_id = _thiscol->id;
					RaycastHit::layer = _thiscol->layer;
					return true;
				}
			}
		}
		for (unsigned int li = 0; li < MeshCollider2d::_mGlobalColArr.size(); li++) {
			MeshCollider2d* m = MeshCollider2d::_mGlobalColArr[li];
			for (size_t lo = 0; lo < m->lines.size(); lo++) {
				MeshLine l = *m->lines[lo];
				if (!(l.layer & layer) && (layer != -1)) {
					continue;
				}
				Vector2 s = l.start - *Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
				Vector2 e = l.end - *Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
				SDL_Rect lr = MeshLine::bounding_boxi(s.x, s.y, e.x, e.y);
				SDL_Rect c_lr = MeshLine::bounding_boxi(start.x, start.y, end.x, end.y);

				bool o = VectorRect::checkCollision(&c_lr, &lr);
				if (o) {
					RaycastHit::object = MeshCollider2d::_mGlobalColArr[li];
					RaycastHit::start = start;
					RaycastHit::end = end;
					RaycastHit::hasHit = true;
					RaycastHit::col_id = l.id;
					RaycastHit::layer = l.layer;
					return true;
				}
			}
		}
		return false;
	};
	// This function name is counter-intuitive, it actually checks to see if rect R collides with anything *except* a specific collider. 
	// This is used internally to check for player collisions.
	static bool TestRectExcept(SDL_Rect* rect, int layer, RectCollider2d* ignored_col) {
		RaycastHit::hasHit = false;
		//SDL_SetRenderDrawColor(__phys_internal_renderer, 255, 64, 255, 255);
		//SDL_RenderDrawLine(__phys_internal_renderer, start.x, start.y, end.x, end.y);
		for (unsigned int i = 0; i < RectCollider2d::_mGlobalColArr.size() && !RaycastHit::hasHit; i++) {
			RectCollider2d* _thiscol = RectCollider2d::_mGlobalColArr[i];
			if (ignored_col == _thiscol) { continue; }
			if (_thiscol->layer & layer || layer == -1) {
				_thiscol->recalc();
				//printf("Calculating rect with collider type %s : %d\n", coltohr(RectCollider2d::_mGlobalColArr[i]->layer), i);
				bool o = RectIntersects(_thiscol->rect,rect);
				if (o) {
					RaycastHit::object = _thiscol;
					RaycastHit::hasHit = true;
					RaycastHit::start = { (Vector)_thiscol->rect->x, (Vector)_thiscol->rect->y};
					RaycastHit::end = RaycastHit::start + Vector2{ (Vector)_thiscol->rect->w, (Vector)_thiscol->rect->h};
					RaycastHit::col_id = _thiscol->id;
					RaycastHit::layer = _thiscol->layer;
					return true;
				}
			}
		}
		for (unsigned int li = 0; li < MeshCollider2d::_mGlobalColArr.size(); li++) {
			MeshCollider2d* m = MeshCollider2d::_mGlobalColArr[li];
			for (size_t lo = 0; lo < m->lines.size(); lo++) {
				MeshLine l = *m->lines[lo];
				if (!(l.layer & layer) && (layer != -1)) {
					continue;
				}
				Vector2 s = l.start - *Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
				Vector2 e = l.end - *Camera::GetInstance()->m_target + Camera::GetInstance()->m_Offset;
				SDL_Rect lr = MeshLine::bounding_boxi(s.x, s.y, e.x, e.y);

				bool o = lineRect(s,e,rect);
				if (o) {
					RaycastHit::object = MeshCollider2d::_mGlobalColArr[li];
					RaycastHit::start = e;
					RaycastHit::end = s;
					RaycastHit::hasHit = true;
					RaycastHit::col_id = l.id;
					RaycastHit::layer = l.layer;
					return true;
				}
			}
		}
		return false;
	};
	static bool TestCircle(Vector2 start, int diameter, int layer) {
		for (float a = 0; a < 360; a+=(360.0f/ diameter) / 512.0f) {
			Vector2 i(
				start.x + (diameter * cos(alib_deg2rad(a))),
				start.y + (diameter * sin(alib_deg2rad(a)))
			);
			if (Test(start, i, layer)) {
				return true;
			}
		}
		return false;
	}
	static bool TestCone(Vector2 start, int angle, int offset, int dist, int layer) {
		for (float a = 0 - angle+offset; a < angle+offset; a+= (360.0f + angle + offset) / 512.0f) {
			Vector2 i(
				start.x+(dist*cos(alib_deg2rad(a))),
				start.y+(dist*sin(alib_deg2rad(a)))
			);
			if (Test(start, i, layer)) {
				return true;
			}
		}
		return false;
	}
	static bool TestAnyCone(Vector2 start, int angle, int offset, int dist) {
		return TestCone(start, angle, offset, dist, -1);
	}
	static bool TestAnyLine(Vector2 start, Vector2 end) {
		return Test(start, end, -1);
	}
	static bool TestAnyCircle(Vector2 start, int diameter) {
		return TestCircle(start, diameter, -1);
	}
	static bool TestConeExcept(Vector2 start, int angle, int offset, int dist, int layer) {
		for (float a = 0 - angle + offset; a < angle + offset; a += (360.0f + angle + offset) / 512.0f) {
			Vector2 i(
				start.x + (dist * cos(alib_deg2rad(a))),
				start.y + (dist * sin(alib_deg2rad(a)))
			);
			if (TestExcept(start, i, layer)) {
				return true;
			}
		}
		return false;
	}
	static bool TestCircleExcept(Vector2 start, int diameter, int layer) {
		for (float a = 0; a < 360; a += (360.0f / diameter) / 512.0f) {
			Vector2 i(
				start.x + (diameter * cos(alib_deg2rad(a))),
				start.y + (diameter * sin(alib_deg2rad(a)))
			);
			if (TestExcept(start, i, layer)) {
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
