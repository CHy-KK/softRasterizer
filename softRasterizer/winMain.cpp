#ifndef  UNICODE
#define UNICODE
#endif // ! UNICODE

// ���Ҫʹ��windows������Ŀ�ǵø���Ŀ���Է�����б������
// ����������ڣ��һ���Ŀ��ѡ������->����->����->ϵͳ������ϵͳ�Ŀ���̨(/ SUBSYSTEM:CONSOLE)���óɴ���(/ SUBSYSTEM:WINDOWS)
// ѡ������->����->C/C++ -> Ԥ����������Ԥ�����������е�_CONSOLE���ó�_CONSOLE_WINDOWS

#include <time.h>
#include "tgaimage.h"
#include "Rasterizer.h"
#include "Win32/BaseWindow.h"

void DrawCall(HWND hwnd) {
	//consoleLog("render");
	RECT rc;
	GetClientRect(hwnd, &rc);
	InvalidateRect(hwnd, &rc, FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	/***** Initialize Renderer /*****/

	const int Width = 500;
	const int Height = 500;
	const bool useMSAA = true;

	// Camera Settings
	extern Vec3f cameraPos;
	cameraPos = Vec3f(0.f, 0.f, 1.f);
	const Vec3f lookAt(0.f, 0.f, 0.f);
	const float fov = 40.f;


	Rasterizer* renderer = Rasterizer::GetInstance(Width, Height, useMSAA);
	shared_ptr<Camera> camera = make_shared<Camera>(cameraPos, lookAt, fov, float(Width) / Height, 0.1f, 100.f);
	renderer->camera = camera;
	shared_ptr<TGAImage> texHeadDiffuse(new TGAImage());
	texHeadDiffuse->read_tga_file("african_head_diffuse.tga");

	shared_ptr<Model> head = make_shared<Model>("head.obj");
	renderer->models.emplace_back(head);

	shared_ptr<Shader> headShader(new BlinnPhong(texHeadDiffuse));
	renderer->setShader(headShader);

	//head.scale(2.f);
	//head.rotate(Vec3f(0, 90, 0));
	
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
		angle += float(cur - last) / 1000.f * 5.f;
		angle %= 360;
		last = cur;
		head->rotate(Vec3f(0, angle, 0));
		// �����Ϣ�����д���WM_PAINT��ô���������ƣ�����Ϊ�������InvalidateRect����һ��WM_PAINT��Ϣ
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
