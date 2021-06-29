#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include "Vectors.hpp"
#include "utils.hpp"



class Camera {
public:
	static Camera* s_Instance;
	Vector2 m_Viewport = Vector2(720,720);
	Vector2 m_Offset;
	Vector2* m_target = nullptr;
	static Camera* GetInstance(){ return s_Instance = (s_Instance != nullptr) ? s_Instance : new Camera(); }
	Vector2 getViewport() {return m_Viewport;}
	void SetTarget(Vector2* targ) {
		m_target = targ;
	};
	void Update() {
	}
	Vector2 Vector2ToCameraSpace(Vector2 v) {
		return (v + -*m_target);
	}
	SDL_Rect* RectToCameraSpace(SDL_Rect* v) {
		Vector2 _mtargetv(m_target->x + v->x, m_target->y + v->y);
		v->x -= _mtargetv.x;
		v->y -= _mtargetv.y;
		return v;
	}
	Camera() {};
	Camera(Vector2* targ) { m_target = targ; };
private:
};
Camera* Camera::s_Instance = nullptr;
#define V2CS Camera::GetInstance()->Vector2ToCameraSpace
#define R2CS Camera::GetInstance()->RectToCameraSpace
#define C_TG Camera::GetInstance()->m_target

#endif // CAMERA_H