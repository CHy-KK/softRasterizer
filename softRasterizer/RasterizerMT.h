#pragma once
#ifndef _RASTERIZER_MT_H_
#define _RASTERIZER_MT_H_

// Rasterizer MultiThread

#include "Model.h"
#include "Camera.h"
#include <algorithm> 
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <Windows.h>

using std::vector;
using std::min;
using std::max;
using std::queue;
using std::ref;
using std::thread;

class RasterizerMT
{
private:
	int width;
	int height;
	bool useMSAA;
	vector<Vec4f> frameBuffer;
	unsigned char* screenBuffer;
	float* zBuffer;
	shared_ptr<Shader> shader;
	vector<Vec2f> x4MSAAoffset{ Vec2f(.25f, .25f), Vec2f(.25f, .75f), Vec2f(.75f, .25f), Vec2f(.75f, .75f) };

	// �Ƿ�ʹ��textures�б�ȡ�������textures������Rasterizer���л���main�����У�ԭ������˵û�����Ҷ��һ��
	// �����������и���ȫ
	//vector<shared_ptr<TGAImage>> textures;

	RasterizerMT(int w, int h, bool msaa = false);
	static RasterizerMT* renderer;

	// MultiThread
	queue<fin> fqueue;
	mutex m;
	condition_variable condFrag;
	condition_variable condTri;
	const int maxCacheDataLength;

public:
	shared_ptr<Camera> camera;
	// TODO:ģ��Ӧ���ÿռ�����㷨�洢
	vector<shared_ptr<Model>> models;
	int drawTriangleNum = 0;

	static RasterizerMT* GetInstance(int w, int h, bool msaa = false);
	static RasterizerMT* GetInstance();	// ���ô˷�����ȡʵ��ʱʵ�������Ѿ�����
	unsigned char* Draw();
	//void Render(shared_ptr<Model> m);
	void Render();
	void triangle(const vout& vert1, const vout& vert2, const vout& vert3, unique_lock<mutex>* lockTri, bool dirtyData = false);
	void fragmentThread();
	bool fragmentProc();
	bool barycentric(float params[], const Vec2i& v3, const Vec2f& p, float& u, float& v);

	Vec3f shaderNormal(Vec3f pos, Vec2f uv, Vec3f normal) { return normal; }	// shader ������û��Ǵ�ֵ����Ϊ�����޸ģ�
	void writeImg(TGAImage& img);

	int Width() const { return width; }
	int Height() const { return height; }

	//int addTexture(const char* filename);

	~RasterizerMT() {
		delete[] screenBuffer;
		delete[] zBuffer;
	}
};

#endif