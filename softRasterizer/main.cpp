#include "tgaimage.h"
#include "Vec.h"
#include "Model.h"
#include "Rasterizer.h"
#include "Matrix.h"
#include "Camera.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

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

	int Width = 500;
	int Height = 500;
	bool useMSAA = true;
	Vec3f lightDir = Vec3f(0.f, 0.f, 1.f);

	// Camera Settings
	Vec3f cameraPos(0.f, 0.f, 2.f);
	Vec3f lookAt(0.f, 0.f, 0.f);
	float fov = 40.f;

	TGAImage image(Width, Height, TGAImage::RGB);
	
	Model head("head.obj");

	Rasterizer renderer(Width, Height, lightDir, useMSAA);
	Camera camera(cameraPos, lookAt, fov, float(Width) / Height, 0.1f, 100.f);

	head.scale(2.f);
	//head.rotate(Vec3f(0, 180, 0));
	//camera.worldPos = Vec3f(2.f, 0.f, 0.f);
	//camera.rotate(Vec3f(0, 90, 0));
	Matrix transformObjToWorld = head.transformMatrix();
	// TODO:修正transformObjToWorldNormal，向量的旋转并非直接取rotation矩阵。
	Matrix transformObjToWorldNormal = head.rotationMatrix();
	Matrix transformWorldToView = camera.transformMatrix();
	Matrix transformViewToClip = camera.perspectiveMatrix();
	//cout << transformWorldToView << endl;
	for (int i = 0; i < head.nfaces(); i++) {
		auto f = head.face(i);
		Vec4f pos1 = transformViewToClip * transformWorldToView * transformObjToWorld * Vec4f(head.vert(f[0].x), 1);
		Vec4f pos2 = transformViewToClip * transformWorldToView * transformObjToWorld * Vec4f(head.vert(f[1].x), 1);
		Vec4f pos3 = transformViewToClip * transformWorldToView * transformObjToWorld * Vec4f(head.vert(f[2].x), 1);
		pos1 /= pos1.w;
		pos2 /= pos2.w;
		pos3 /= pos3.w;
		Vec3f normal1 = transformObjToWorldNormal * head.vert(f[0].z);
		Vec3f normal2 = transformObjToWorldNormal * head.vert(f[1].z);
		Vec3f normal3 = transformObjToWorldNormal * head.vert(f[2].z);

		// TODO:这里pos1的x y z应该是在clipspace中，范围为-1~1，然后需要把zbuffer初始化为1
		vertex v1({ (pos1 + Vec4f_one) / 2, head.uv(f[0].y), normal1 });
		vertex v2({ (pos2 + Vec4f_one) / 2, head.uv(f[1].y), normal2 });
		vertex v3({ (pos3 + Vec4f_one) / 2, head.uv(f[2].y), normal3 });
		renderer.triangle(v1, v2, v3, image);
	}

	renderer.writeImg(image);

	image.flip_vertically();
	image.write_tga_file("output1.tga");


	return 0;
}
