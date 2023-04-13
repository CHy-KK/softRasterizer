//#include "tgaimage.h"
//#include "Rasterizer.h"
//#include "Camera.h"
//
////const TGAColor white = TGAColor(255, 255, 255, 255);
////const TGAColor red = TGAColor(255, 0, 0, 255);
//
//// TODO: win32��Ϣѭ��дһ����ʵ��̬���е���Ⱦ��
//// TODO: ����triangle��fragment shader��ʹ�ö��߳̽�vertex shader��fragment shader��������
//// �������������������ģʽ��triangle�Ӹ���vertex����fragment����ƬԪ�����У�fragment shader��ƬԪ������ȡ��fragment������Ⱦ
//
//
//int main(int argc, char** argv) {
//
//	extern Vec3f lightDir;
//	extern Matrix transformObjToWorld;
//	extern Matrix transformObjToWorldNormal;
//	extern Matrix transformWorldToView;
//	extern Matrix transformViewToClip;
//	extern Vec3f cameraPos;
//
//	lightDir = Vec3f(0.f, 0.f, 1.f);
//	const int Width = 500;
//	const int Height = 500;
//	const bool useMSAA = true;
//
//	// Camera Settings
//	cameraPos = Vec3f(0.f, 0.f, 1.f);
//	const Vec3f lookAt(0.f, 0.f, 0.f);
//	const float fov = 40.f;
//
//	TGAImage image(Width, Height, TGAImage::RGB);
//	Model head("head.obj");
//
//	Rasterizer* renderer = Rasterizer::GetInstance(Width, Height, useMSAA);
//	Camera camera(cameraPos, lookAt, fov, float(Width) / Height, 0.1f, 100.f);
//
//	shared_ptr<TGAImage> texHeadDiffuse(new TGAImage());
//	texHeadDiffuse->read_tga_file("african_head_diffuse.tga");
//
//	shared_ptr<Shader> headShader(new BlinnPhong(texHeadDiffuse));
//	renderer->setShader(headShader);
//
//	//head.scale(2.f);
//	//head.rotate(Vec3f(0, 90, 0));
//
//	transformObjToWorld = head.transformMatrix();
//	// ������ת���󰴵�������mat3(model.inverse().transpose())����Ϊ���������Ŵ�����Ӱ�죬��ʵ���Ϻ�ֱ����rotation�����һ�µ�
//	transformObjToWorldNormal = head.rotationMatrix();
//	transformWorldToView = camera.transformMatrix();
//	transformViewToClip = camera.perspectiveMatrix();
//
//	renderer->Render(head);
//
//	renderer->writeImg(image);
//
//	image.flip_vertically();
//	image.write_tga_file("output1.tga");
//
//	cout << renderer->drawTriangleNum << endl;
//
//
//	return 0;
//}
