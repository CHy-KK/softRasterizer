#include "tgaimage.h"
#include "Vec.h"
#include "Model.h"
#include<cassert>
using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Vec3f lightDir = Vec3f(0.f, 0.f, -1.f);

int Width = 500;
int Height = 500;
bool useMSAA = true;
vector<Vec3f> frameBuffer;
vector<float> zBuffer;
vector<Vec2f> x4MSAAoffset{ Vec2f(.25f, .25f), Vec2f(.25f, .75f), Vec2f(.75f, .25f), Vec2f(.75f, .75f) };

void line(int x0, int y0, int x1, int y1, TGAImage& img, TGAColor color) {
	if (x0 == x1 && y0 == y1) {
		img.set(x0, y0, color);
		return;
	}

	float xStep = x1 - x0;
	float yStep = y1 - y0;
	float dir = xStep * yStep;
	int isReverse = dir > 0 ? 1 : (dir < 0 ? -1 : 0);

	if (abs(xStep) > abs(yStep)) {
		if (xStep < 0) {
			swap(x0, x1);
			swap(y0, y1);
			xStep *= -1;
			yStep *= -1;
		}
		for (int x = x0; x <= x1; x++) {
			float t = static_cast<float>(x - x0) / xStep;
			img.set(x, y0 + t * yStep, color);
		}
	}
	else {
		if (yStep < 0) {
			swap(x0, x1);
			swap(y0, y1);
			xStep *= -1;
			yStep *= -1;
		}
		for (int y = y0; y <= y1; y++) {
			float t = static_cast<float>(y - y0) / yStep;
			img.set(x0 + t * xStep, y, color);
		}
	}
}

bool barycentric(float params[], const Vec2i& v3, const Vec2f& p, float& u, float& v) {
	float r1 = p.x - v3.x;
	float r2 = p.y - v3.y;
	u = (r1 * params[3] - params[2] * r2) / params[4];
	v = (params[0] * r2 - r1 * params[1]) / params[4];
	return u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1;
}

void triangle(Vec3f vert1, Vec3f vert2, Vec3f vert3, TGAImage& img, TGAColor color) {

	// 如果这里用vecf，会导致因为精度问题，有的边界上像素不被两边三角形覆盖
	Vec2i v1(vert1.x * Width, vert1.y * Height);
	Vec2i v2(vert2.x * Width, vert2.y * Height);
	Vec2i v3(vert3.x * Width, vert3.y * Height);

	Vec3f flatNormal =  normalize(cross(vert3 - vert1, vert2 - vert1));
	float intensity = dot(flatNormal, lightDir);
	//intensity = 1;
	/*if (intensity < 0)
		return;*/
	Vec3f col1 = (Vec3f_one * intensity * 255);
	Vec3f col2 = (Vec3f_one * intensity * 255);
	Vec3f col3 = (Vec3f_one * intensity * 255);
	//cout << col1 << endl;

	//assert(v1.x != v2.x || v2.x != v3.x);
	//assert(v1.y != v2.y || v2.y != v3.y);
	if (v1.x == v2.x && v1.x == v3.x)
		return;
	if (v1.y == v2.y && v1.y == v3.y)
		return;

	float params[5];
	params[0] = v1.x - v3.x;
	params[1] = v1.y - v3.y;
	params[2] = v2.x - v3.x;
	params[3] = v2.y - v3.y;
	params[4] = params[0] * params[3] - params[2] * params[1];

	int minx = min(v1.x, min(v2.x, v3.x));
	int maxx = max(v1.x, max(v2.x, v3.x));
	int miny = min(v1.y, min(v2.y, v3.y));
	int maxy = max(v1.y, max(v2.y, v3.y));
	for (int y = miny; y < maxy; y++) {
		for (int x = minx; x < maxx; x++) {
			Vec2f p(x, y);
			float u, v;
			Vec3f colorVec;
			if (useMSAA) {
				bool sample = false;
				for (int i = 0; i < 4; i++) {
					if (barycentric(params, v3, p + x4MSAAoffset[i], u, v)) {
						float w = 1 - u - v;
						float depth = u * vert1.z + v * vert2.z + w * vert3.z;
						if (!sample) {
							colorVec = u * col1 + v * col2 + w * col3;
							sample = true;
						}
						if (depth > zBuffer[(x + y * Width) * 4 + i]) {
							frameBuffer[(x + y * Width) * 4 + i] = colorVec;
							zBuffer[(x + y * Width) * 4 + i] = depth;
						}
					}
				}
			}
			else {
				if (barycentric(params, v3, p, u, v)) {
					float w = 1 - u - v;
					colorVec = u * col1 + v * col2 + w * col3;
					frameBuffer[x + y * Width] = colorVec;
				}
			}
		}
	}
}


int main(int argc, char** argv) {
	TGAImage image(Width, Height, TGAImage::RGB);
	if (useMSAA) {
		frameBuffer.resize(Width * Height * 4, Vec3f(0, 0, 0));
		zBuffer.resize(Width * Height * 4, 0);
	}
	else {
		frameBuffer.resize(Width * Height, Vec3f(0, 0, 0));
		zBuffer.resize(Width * Height, 1);
	}
	Model head("head.obj");
	//triangle(Vec2i(32, 54), Vec2i(125, 441), Vec2i(398, 159), image, white);


	for (int i = 0; i < head.nfaces(); i++) {
		auto f = head.face(i);
		Vec3f v1 = (head.vert(f[0].x) + Vec3f_one) / 2;
		Vec3f v2 = (head.vert(f[1].x) + Vec3f_one) / 2;
		Vec3f v3 = (head.vert(f[2].x) + Vec3f_one) / 2;
		triangle(v1, v2, v3, image, red);
	}

	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			if (useMSAA) {
				Vec3f colorVec;
				for (int i = 0; i < 4; i++) {
					colorVec += 0.25 * frameBuffer[(y * Width + x) * 4 + i];
				}
				image.set(x, y, TGAColor(colorVec, 255));
			}
			else {
				image.set(x, y, TGAColor(frameBuffer[y * Width + x], 255));
			}
		}
	}

	image.flip_vertically();
	image.write_tga_file("output1.tga");


	return 0;
}
