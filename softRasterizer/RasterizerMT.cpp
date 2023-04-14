#include "RasterizerMT.h"

RasterizerMT* RasterizerMT::renderer = nullptr;

RasterizerMT::RasterizerMT(int w, int h, bool msaa) :width(w), height(h), useMSAA(msaa), maxCacheDataLength(50) {
	if (useMSAA) {
		frameBuffer.resize(width * height * 4, Vec4f(0, 0, 0, 0));
		screenBuffer = new unsigned char[width * height * 4];
		zBuffer = new float[width * height * 4];
		std::fill_n(zBuffer, width * height * 4, 1.f);
	}
	else {
		frameBuffer.resize(width * height, Vec4f(0, 0, 0, 0));
		screenBuffer = new unsigned char[width * height * 4];
		zBuffer = new float[width * height];
		std::fill_n(zBuffer, width * height, 1.f);
	}
}

RasterizerMT* RasterizerMT::GetInstance(int w, int h, bool msaa)
{
	if (renderer == nullptr)
		renderer = new RasterizerMT(w, h, msaa);
	return renderer;
}

RasterizerMT* RasterizerMT::GetInstance()
{
	assert(renderer != nullptr);
	return renderer;
}

unsigned char* RasterizerMT::Draw()
{
	extern Vec3f lightDir;
	extern Matrix transformWorldToView;
	extern Matrix transformViewToClip;
	lightDir = Vec3f(0.f, 0.f, 1.f);

	transformWorldToView = camera->transformMatrix();
	transformViewToClip = camera->perspectiveMatrix();

	while (!fqueue.empty())
		fqueue.pop();
	
	thread thTri([this]() {
		Render();
	});
	vector<thread> thFrg;
	for (int i = 0; i < 4; i++)
		thFrg.emplace_back([this]() {
			fragmentThread();
		});
		
	thTri.join();
	for (int i = 0; i < 4; i++) {
		thFrg[i].join();
	}
	cout << "finish one round" << endl;
	


	if (useMSAA) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Vec4f colorVec;
				for (int i = 0; i < 4; i++) {
					colorVec += 0.25 * frameBuffer[(y * width + x) * 4 + i];
					// 顺便清空缓存，否则会有颜色残留
					frameBuffer[(y * width + x) * 4 + i] = Vec4f(0, 0, 0, 0);
					zBuffer[(y * width + x) * 4 + i] = 1;
				}
				screenBuffer[(y * width + x) * 4 + 0] = clamp(colorVec.z, 0, 255);
				screenBuffer[(y * width + x) * 4 + 1] = clamp(colorVec.y, 0, 255);
				screenBuffer[(y * width + x) * 4 + 2] = clamp(colorVec.x, 0, 255);
				screenBuffer[(y * width + x) * 4 + 3] = 0;
			}
		}
	}
	else {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Vec4f colorVec = frameBuffer[y * width + x];
				// 顺便清空缓存，否则会有颜色残留
				frameBuffer[y * width + x] = Vec4f(0, 0, 0, 0);
				zBuffer[y * width + x] = 1;
				screenBuffer[(y * width + x) * 4 + 0] = clamp(colorVec.z, 0, 255);
				screenBuffer[(y * width + x) * 4 + 1] = clamp(colorVec.y, 0, 255);
				screenBuffer[(y * width + x) * 4 + 2] = clamp(colorVec.x, 0, 255);
				screenBuffer[(y * width + x) * 4 + 3] = 0;
			}
		}
	}
	return screenBuffer;
}

void RasterizerMT::Render()
{
	unique_lock<mutex> lockTri(m);
	// 这里的模型变化矩阵后面最好改成shader内置变量，因为如果使用多线程的话多个模型并行渲染 不可能都使用同一个模型变换
	// 当然多个模型使用同一个光照方向、View和projection矩阵应该是ok的
	for (shared_ptr<Model> mod: models) {
		shader = mod->getShader();
		shader->transformObjToWorld = mod->transformMatrix();
		// 法线旋转矩阵按道理是用mat3(model->inverse()->transpose())，是为了消除缩放带来的影响，但实际上和直接用rotation结果是一致的
		shader->transformObjToWorldNormal = mod->rotationMatrix();
		for (int i = 0; i < mod->nfaces(); i++) {
			auto f = mod->face(i);
			vout v1 = shader->vertex(vin(Vec4f(mod->vert(f[0].x), 1), mod->uv(f[0].y), mod->normal(f[0].z)));
			vout v2 = shader->vertex(vin(Vec4f(mod->vert(f[1].x), 1), mod->uv(f[1].y), mod->normal(f[1].z)));
			vout v3 = shader->vertex(vin(Vec4f(mod->vert(f[2].x), 1), mod->uv(f[2].y), mod->normal(f[2].z)));
			renderer->triangle(v1, v2, v3, &lockTri);
		}
		cout << "finish render\n";
	}
	renderer->triangle(vout(), vout(), vout(), &lockTri, true);
}

bool RasterizerMT::barycentric(float params[], const Vec2i& v3, const Vec2f& p, float& u, float& v)
{
	float r1 = p.x - v3.x;
	float r2 = p.y - v3.y;
	u = (r1 * params[3] - params[2] * r2) / params[4];
	v = (params[0] * r2 - r1 * params[1]) / params[4];
	return u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1;
}

void RasterizerMT::triangle(const vout& vert1, const vout& vert2, const vout& vert3, unique_lock<mutex>* lockTri, bool dirtyData)
{
	if (dirtyData) {
		while (fqueue.size() >= maxCacheDataLength)
			condTri.wait(*lockTri);
		fqueue.emplace(vout(), 0, shader, -1, false);
		condFrag.notify_one();
		return;
	}

	// 如果这里用vecf，会导致因为精度问题，有的边界上像素不被两边三角形覆盖
	Vec2i v1 = Vec2i((vert1.position.x + 1) * .5f * width, (vert1.position.y + 1) * .5f * height);
	Vec2i v2 = Vec2i((vert2.position.x + 1) * .5f * width, (vert2.position.y + 1) * .5f * height);
	Vec2i v3 = Vec2i((vert3.position.x + 1) * .5f * width, (vert3.position.y + 1) * .5f * height);

	Vec2i e12 = v2 - v1;
	Vec2i e23 = v3 - v2;
	if (cross(e12, e23) < 0)
		return;
	//drawTriangleNum++;

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

			while (fqueue.size() >= maxCacheDataLength) {
				condTri.wait(*lockTri);
			}

			Vec2f p(x, y);
			float u, v;
			Vec4f colorVec;
			if (useMSAA) {
				int refCol = -1;
				for (int i = 0; i < 4; i++) {
					if (barycentric(params, v3, p + x4MSAAoffset[i], u, v)) {
						float w = 1 - u - v;
						Vec3f pos = Vec3f(u * vert1.position + v * vert2.position + w * vert3.position);
						Vec2f uv = u * vert1.uv + v * vert2.uv + w * vert3.uv;
						Vec3f n = u * vert1.normal + v * vert2.normal + w * vert3.normal;
						int idx = (x + y * width) * 4 + i;
						if (refCol < 0) {
							//colorVec = shader->fragment(vout(pos, uv, n)) * 255;
							fqueue.emplace(vout(pos, uv, n), idx, shader);
							refCol = idx;
						}
						else {
							fqueue.emplace(vout(pos, uv, n), idx, shader, refCol);
						}
						condFrag.notify_one();
						//if (pos.z > 0 && pos.z < zBuffer[(x + y * width) * 4 + i]) {
						//	frameBuffer[(x + y * width) * 4 + i] = colorVec;
						//	zBuffer[(x + y * width) * 4 + i] = pos.z;
						//}
					}
				}
			}
			//else {
			//	if (barycentric(params, v3, p, u, v)) {
			//		float w = 1 - u - v;
			//		colorVec = Vec4f(u * vert1.normal + v * vert2.normal + w * vert3.normal, 1);
			//		frameBuffer[x + y * width] = colorVec;
			//	}
			//}
		}
	}
}

void RasterizerMT::fragmentThread()
{
	while (fragmentProc());
}

bool RasterizerMT::fragmentProc()
{
	unique_lock<mutex> lockFrg(m);
	while (fqueue.empty()) {
		condFrag.wait(lockFrg);
	}
	auto& frag = fqueue.front();
	if (!frag.valid) {
		return false;
	}
	fqueue.pop();

	//cout << "fragments" << endl;
	// 片元处理
	// 脏数据则直接退出
	
	if (frag.ref > 0) {
		frameBuffer[frag.idx] = frameBuffer[frag.ref];
	}
	else {
		float z = frag.v.position.z;
		int idx = frag.idx;
		if (z > 0 && z < zBuffer[idx]) {
			zBuffer[idx] = z;
			auto color =  frag.shader->fragment(frag.v);
			frameBuffer[idx] = frag.shader->fragment(frag.v) * 255;
		}
	}

	condTri.notify_one();
	return true;
}

void RasterizerMT::writeImg(TGAImage& img)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (useMSAA) {
				Vec4f colorVec;
				for (int i = 0; i < 4; i++) {
					colorVec += 0.25 * frameBuffer[(y * width + x) * 4 + i];
				}
				img.set(x, y, TGAColor(colorVec));
			}
			else {
				img.set(x, y, TGAColor(frameBuffer[y * width + x]));
			}
		}
	}
}


//int RasterizerMT::addTexture(const char* filename)
//{
//	int id = textures.size();
//	TGAImage* img = new TGAImage();
//	img->read_tga_file(filename);
//	textures.emplace_back(img);
//
//	return id;
//}
