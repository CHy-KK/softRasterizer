#pragma once
#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include "Matrix.h"
class GameObject
{
public:
    Vec3f worldPos = Vec3f(0.f, 0.f, 0.f);
    Vec3f eulaAngle = Vec3f(0.f, 0.f, 0.f);
    float scaleSize = 1.f;

    GameObject() {}

    void translate(Vec3f offset) { worldPos += offset; }
    void rotate(Vec3f r) { 
		eulaAngle += r; 
		eulaAngle.x = fmod(eulaAngle.x, 360);
		eulaAngle.y = fmod(eulaAngle.y, 360);
		eulaAngle.z = fmod(eulaAngle.z, 360);
	}
    void scale(float s) { scaleSize = s; }

	Matrix rotationMatrix() const {
		float alpha = degrees_to_radians(eulaAngle.x);
		float beta = degrees_to_radians(eulaAngle.y);
		float gamma = degrees_to_radians(eulaAngle.z);

		Matrix rx({
			{1, 0, 0},
			{0, cos(alpha), -sin(alpha)},
			{0, sin(alpha), cos(alpha)},
		});

		Matrix ry({
			{cos(beta), 0, sin(beta)},
			{0, 1, 0},
			{-sin(beta), 0, cos(beta)}
		});

		Matrix rz({
			{cos(gamma), -sin(gamma), 0},
			{sin(gamma), cos(gamma), 0},
			{0, 0, 1}
		});

		return rz * ry * rx;
	}

    virtual Matrix transformMatrix() const {

		Matrix scale = Matrix::identity(3) * scaleSize;

		scale = rotationMatrix() * scale;

		return Matrix({
			{ scale[0][0], scale[0][1], scale[0][2], worldPos.x },
			{ scale[1][0], scale[1][1], scale[1][2], worldPos.y },
			{ scale[2][0], scale[2][1], scale[2][2], worldPos.z },
			{0, 0, 0, 1}
		});
    }

};

#endif