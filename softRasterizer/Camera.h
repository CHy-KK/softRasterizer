#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GameObject.h"
class Camera : public GameObject
{
private:
	Vec3f cameraForward;
	Vec3f cameraUp;
	Vec3f cameraRight;
	float fov;	//	¥À¥¶fov»°∞ÎΩ«
	float aspect;
	float n;
	float f;

public:
	Camera(Vec3f _pos, Vec3f _lookAt, float _fov, float _aspect, float _near=0.1f, float _far=100.f)
		: n(_near), f(_far), fov(_fov), aspect(_aspect) {
		worldPos = _pos;
		Vec3f up(0.f, 1.f, 0.f);
		cameraForward = normalize(_pos - _lookAt);
		cameraRight = normalize(cross(up, cameraForward));
		cameraUp = normalize(cross(cameraForward, cameraRight));
	}

	virtual Matrix transformMatrix();
	Matrix perspectiveMatrix();
	Matrix othographicMatrix();
};

#endif