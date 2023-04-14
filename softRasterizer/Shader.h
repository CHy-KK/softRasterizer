#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include "Matrix.h"
#include "tgaimage.h"
using std::shared_ptr;

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
	vout(const Vec3f& _pos, const Vec2f& _uv, const Vec3f& _n)
		: position(_pos), uv(_uv), normal(_n) {}
	Vec3f position;	// clip space position
	Vec2f uv;
	Vec3f normal;	// world space normal
};

struct fin;

class Shader
{
private:

public:
	Shader() {}
	virtual vout vertex(vin i) = 0;
	virtual Vec4f fragment(vout i) = 0;
	Matrix transformObjToWorld;
	Matrix transformObjToWorldNormal;
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

/*
* idx: 该片元在framebuffer中的位置
* ref: 默认值为-1表示正常进行片元着色，如果>=0直接采用framebuffer[ref]颜色值作为结果，因为msaa采样
* valid: 是否是脏数据，如果是则片元着色线程退出本次模型渲染
*/
struct fin
{
	fin() {}
	fin(const vout& _v, int _idx, shared_ptr<Shader> _shader, int _ref = -1, bool v = true)
		: v(_v), idx(_idx), shader(_shader), ref(_ref), valid(v) {}
	vout v;
	int idx;
	shared_ptr<Shader> shader;
	int ref;
	bool valid;
};

#endif // !_SHADER_H_