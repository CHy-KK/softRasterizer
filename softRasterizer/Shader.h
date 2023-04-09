#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include "Matrix.h"
#include "tgaimage.h"
using std::shared_ptr;

extern Matrix transformObjToWorld;
extern Matrix transformObjToWorldNormal;
extern Matrix transformWorldToView;
extern Matrix transformViewToClip;
extern Vec3f lightDir;
extern Vec3f cameraPos;

struct vin
{
	vin() {}
	vin(const Vec4f& _pos, const Vec2f& _uv, const Vec3f& _n)
		: position(_pos), uv(_uv), normal(_n) {}
	Vec4f position;	// object space position
	Vec2f uv;
	Vec3f normal;	// object space normal
};

struct vout
{
	vout() {}
	vout (const Vec3f& _pos, const Vec2f& _uv, const Vec3f& _n)
		: position(_pos), uv(_uv), normal(_n){}
	Vec3f position;	// clip space position
	Vec2f uv;
	Vec3f normal;	// world space normal
};

class Shader
{
public:
	Shader() {}
	virtual vout vertex(vin i) = 0;
	virtual Vec4f fragment(vout i) = 0;
};

class BlinnPhong :public Shader {
private:
	shared_ptr<TGAImage> baseMap;
public:
	BlinnPhong(shared_ptr<TGAImage> _baseMap):baseMap(_baseMap) {}

	virtual vout vertex(vin i) {
		vout o;
		o.position = transformViewToClip * transformWorldToView * transformObjToWorld * i.position;
		o.normal = transformObjToWorldNormal * i.normal;
		// TODO:后面加上uv变换uv * scale + translation
		o.uv = i.uv;
		return o;
	}

	virtual Vec4f fragment(vout i) {
		float light = dot(i.normal, lightDir);
		Vec4f color = baseMap->sample(i.uv) * light;
		return Vec4f(Vec3f(color), 1.f);
	}
};

#endif // !_SHADER_H_