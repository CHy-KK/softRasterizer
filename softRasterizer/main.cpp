#include "tgaimage.h"
#include "Vec.h"
#include "Model.h"
#include "Rasterizer.h"
#include<cassert>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Vec3f lightDir = Vec3f(0.f, 0.f, -1.f);

int Width = 500;
int Height = 500;
bool useMSAA = true;

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




int main(int argc, char** argv) {
	TGAImage image(Width, Height, TGAImage::RGB);
	
	Model head("head.obj");

	Rasterizer renderer(Width, Height, useMSAA);

	for (int i = 0; i < head.nfaces(); i++) {
		auto f = head.face(i);
		Vec3f pos1 = (head.vert(f[0].x) + Vec3f_one) / 2;
		Vec3f pos2 = (head.vert(f[1].x) + Vec3f_one) / 2;
		Vec3f pos3 = (head.vert(f[2].x) + Vec3f_one) / 2;
		Vec2f uv1 = head.uv(f[0].y);
		Vec2f uv2 = head.uv(f[1].y);
		Vec2f uv3 = head.uv(f[2].y);
		Vec3f n1 = head.vert(f[0].z);
		Vec3f n2 = head.vert(f[1].z);
		Vec3f n3 = head.vert(f[2].z);
		// TODO: 把triangle输入改为vertex形式的
		vertex v1({pos1, uv1, n1});
		vertex v2({pos2, uv2, n2});
		vertex v3({pos3, uv3, n3});
		renderer.triangle(v1, v2, v3, image);
	}

	renderer.writeImg(image);

	image.flip_vertically();
	image.write_tga_file("output1.tga");


	return 0;
}
