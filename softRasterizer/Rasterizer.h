#pragma once
#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "Shader.h"
#include "Model.h"

using std::vector;
using std::min;
using std::max;

class Rasterizer
{
private:
	int width;
	int height;
	bool useMSAA;
	vector<Vec4f> frameBuffer;
	vector<float> zBuffer;
	vector<Vec2f> x4MSAAoffset{ Vec2f(.25f, .25f), Vec2f(.25f, .75f), Vec2f(.75f, .25f), Vec2f(.75f, .75f) };

	// 是否使用textures列表取决于想把textures保存在Rasterizer类中还是main函数中，原理上来说没区别，且多此一举
	// 但保存在类中更安全
	//vector<shared_ptr<TGAImage>> textures;
	shared_ptr<Shader> shader;

	Rasterizer(int w, int h, bool msaa = false);
	static Rasterizer* renderer;

public:
	static Rasterizer* GetInstance(int w, int h, bool msaa = false);
	void Render(const Model& m);
	bool barycentric(float params[], const Vec2i& v3, const Vec2f& p, float& u, float& v);
	Vec3f shaderNormal(Vec3f pos, Vec2f uv, Vec3f normal) { return normal; }	// shader 里面最好还是传值，因为可能修改？

	void triangle(const vout& vert1, const vout& vert2, const vout& vert3);
	void writeImg(TGAImage& img);
	//int addTexture(const char* filename);
	void setShader(shared_ptr<Shader> s) { shader = s; }
	shared_ptr<Shader> getShader() const { return shader; }

	int drawTriangleNum = 0;

	~Rasterizer() {
		// TODO:后面把framebuffer和depthbuffer改成指针动态分配内存，而不是使用vector
	}
};                                                        



#endif