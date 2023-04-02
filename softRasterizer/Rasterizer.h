#pragma once
#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include <iostream>
#include "tgaimage.h"
#include "Vec.h"

using std::vector;
using std::min;
using std::max;

struct vertex
{
	Vec3f position;
	Vec2f uv;
	Vec3f normal;
};

struct fragment
{
	Vec3f position;
	Vec2f uv;
	Vec3f normal;
};

class Rasterizer
{
private:
	int width;
	int height;
	bool useMSAA;
	vector<Vec3f> frameBuffer;
	vector<float> zBuffer;
	vector<Vec2f> x4MSAAoffset{ Vec2f(.25f, .25f), Vec2f(.25f, .75f), Vec2f(.75f, .25f), Vec2f(.75f, .75f) };

public:
	Rasterizer(int w, int h, bool msaa = false):width(w), height(h), useMSAA(msaa) {
		if (useMSAA) {
			frameBuffer.resize(width * height * 4, Vec3f(0, 0, 0));
			zBuffer.resize(width * height * 4, 0);
		}
		else {
			frameBuffer.resize(width * height, Vec3f(0, 0, 0));
			zBuffer.resize(width * height, 1);
		}
	}

	bool barycentric(float params[], const Vec2i& v3, const Vec2f& p, float& u, float& v);
	Vec3f shaderNormal(Vec3f pos, Vec2f uv, Vec3f normal) { return Vec3f(normal); }	// shader 里面最好还是传值，因为可能修改？
	void triangle(const vertex& vert1, const vertex& vert2, const vertex& vert3, TGAImage& img);
	void writeImg(TGAImage& img);
};

#endif