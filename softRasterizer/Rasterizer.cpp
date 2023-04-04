#include "Rasterizer.h"

bool Rasterizer::barycentric(float params[], const Vec2i& v3, const Vec2f& p, float& u, float& v)
{
	float r1 = p.x - v3.x;
	float r2 = p.y - v3.y;
	u = (r1 * params[3] - params[2] * r2) / params[4];
	v = (params[0] * r2 - r1 * params[1]) / params[4];
	return u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1;
}

void Rasterizer::triangle(const vertex& vert1, const vertex& vert2, const vertex& vert3, TGAImage& img)
{
	// 如果这里用vecf，会导致因为精度问题，有的边界上像素不被两边三角形覆盖
	Vec2i v1(vert1.position.x * width, vert1.position.y * height);
	Vec2i v2(vert2.position.x * width, vert2.position.y * height);
	Vec2i v3(vert3.position.x * width, vert3.position.y * height);

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

	int minx = max(0, min(v1.x, min(v2.x, v3.x)));
	int maxx = min(width, max(v1.x, max(v2.x, v3.x)));
	int miny = max(0, min(v1.y, min(v2.y, v3.y)));
	int maxy = min(height, max(v1.y, max(v2.y, v3.y)));
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
						Vec3f pos = Vec3f(u * vert1.position + v * vert2.position + w * vert3.position);
						Vec2f uv = u * vert1.uv + v * vert2.uv + w * vert3.uv;
						Vec3f n = u * vert1.normal + v * vert2.normal + w * vert3.normal;
						if (!sample) {
							colorVec = shaderBaseLight(pos, uv, n) * 255;
							sample = true;
						}
						if (pos.z > 0 && pos.z < zBuffer[(x + y * width) * 4 + i]) {
							frameBuffer[(x + y * width) * 4 + i] = colorVec;
							zBuffer[(x + y * width) * 4 + i] = pos.z;
						}
					}
				}
			}
			else {
				if (barycentric(params, v3, p, u, v)) {
					float w = 1 - u - v;
					colorVec = u * vert1.normal + v * vert2.normal + w * vert3.normal;
					frameBuffer[x + y * width] = colorVec;
				}
			}
		}
	}
}

void Rasterizer::writeImg(TGAImage& img)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (useMSAA) {
				Vec3f colorVec;
				for (int i = 0; i < 4; i++) {
					colorVec += 0.25 * frameBuffer[(y * width + x) * 4 + i];
				}
				img.set(x, y, TGAColor(colorVec, 255));
			}
			else {
				img.set(x, y, TGAColor(frameBuffer[y * width + x], 255));
			}
		}
	}
}
