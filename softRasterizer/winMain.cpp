#ifndef  UNICODE
#define UNICODE
#endif // ! UNICODE

// 如果要使用windows窗口项目记得改项目属性否则会有编译错误
// 解决方案窗口，右击项目，选择属性->配置->链接->系统，将子系统的控制台(/ SUBSYSTEM:CONSOLE)设置成窗口(/ SUBSYSTEM:WINDOWS)
// 选择属性->配置->C/C++ -> 预处理器，将预处理器定义中的_CONSOLE设置成_CONSOLE_WINDOWS

#include "tgaimage.h"
#include "Rasterizer.h"
#include "RasterizerMT.h"
#include "BaseWindow.h"

void DrawCall(HWND hwnd) {
	//consoleLog("render");
	RECT rc;
	GetClientRect(hwnd, &rc);
	//rc.right = 100;
	InvalidateRect(hwnd, &rc, FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	/***** Initialize Renderer /*****/

	const int Width = 200;
	const int Height = 200;
	const bool useMSAA = true;

	// Camera Settings
	extern Vec3f cameraPos;
	cameraPos = Vec3f(0.f, 0.f, 1.f);
	const Vec3f lookAt(0.f, 0.f, 0.f);
	const float fov = 40.f;


	RasterizerMT* renderer = RasterizerMT::GetInstance(Width, Height, useMSAA);
	shared_ptr<Camera> camera = make_shared<Camera>(cameraPos, lookAt, fov, float(Width) / Height, 0.1f, 100.f);
	renderer->camera = camera;
	// TODO:异步加载纹理，在使用时添加，而不是在最开始就将所有纹理加入内存
	shared_ptr<TGAImage> texHeadDiffuse(new TGAImage());
	texHeadDiffuse->read_tga_file("african_head_diffuse.tga");

	shared_ptr<Model> head = make_shared<Model>("head.obj");
	renderer->models.emplace_back(head);

	shared_ptr<Shader> headShader(new BlinnPhong(texHeadDiffuse));
	head->setShader(headShader);
	
	/***** Create Window /*****/
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	MainWindow win;
	if (!win.Create(L"Learn Win32 window", WS_OVERLAPPEDWINDOW, 0, 
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height)) {
		return 0;
	}

	ShowWindow(win.hwnd(), nCmdShow);

	MSG msg = {};
	time_t last = clock();
	time_t cur;
	int angle = 0;

	while (true)
	{
		cur = clock();
		angle = float(cur - last) / 1000.f * 20.f;
		last = cur;
		head->rotate(Vec3f(0, angle, 0));
		// 如果消息队列中存在WM_PAINT那么就正常绘制，队列为空则调用InvalidateRect加入一个WM_PAINT消息
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			DrawCall(win.hwnd());
		}

	}

	FreeConsole();
	return 0;

}
