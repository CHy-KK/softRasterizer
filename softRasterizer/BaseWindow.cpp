#include "BaseWindow.h"
LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:	// ������Դ���Ϣ��������ô���ص�DefWindowProc�л��WM_CLOSEĬ�ϵ���DestroyWindow
		if (MessageBox(m_hwnd, L"quit?", L"Keep open", MB_OKCANCEL) == IDOK) {
			DestroyWindow(m_hwnd);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);	// ���յ��رմ��ڵ��������Ϣ���л��յ�WM_DESTROY��Ϣ����ʱ��Ҫ����PostQuitMessage(0)����Ϣ�����з���WM_QUIT��Ϣ��֪��Ϣѭ������������������򴰿ڹرյ������������	
		return 0;
	case WM_PAINT:
	{
		//consoleLog("WM_PAINT");
		PAINTSTRUCT ps;		// ps.rcPaint�����˴λ��Ƶ������С��top��leftĬ��Ϊ0��bottom��rightĬ��Ϊ����height��width�������޸��⼸������������������
		HDC hdc = BeginPaint(m_hwnd, &ps);	// ��ʼ����

		Draw(hdc);

		time_t curT = clock();
		int fps = 1000 / (curT - lastT);
		lastT = curT;
		DrawText(hdc, to_wstring(fps).c_str(), -1, &ps.rcPaint, DT_CENTER);

		EndPaint(m_hwnd, &ps);	// ��������
	}
	return 0;
	case WM_SIZE:	// ��WM_SIZEΪ����wParam��һ����־��ָʾ��������С�� / ��� / ������С��lParam�������ڵ��¿�ߣ����ǿ��ֵ����16bitֵ��������Ҫ�øߵ������ֱ��ȡ
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		OnSize(m_hwnd, (UINT)wParam, width, height);
	}
	return 0;
	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}

void Draw(HDC hdc)
{
	int width = Rasterizer::GetInstance()->Width();
	int height = Rasterizer::GetInstance()->Height();
	BITMAPINFO bitmapInfo;
	ZeroMemory(&bitmapInfo, sizeof(BITMAPINFO));
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = width;
	bitmapInfo.bmiHeader.biHeight = height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	// ʹ��˫������ƣ��Ƚ���Ⱦ������Ƶ�backBuffer��
	// ע��WIN32��BITMAPINFO��rgbquad��ɫ��ʽΪblue green red����һ���rgb���෴��
	unsigned char* backBuffer = Rasterizer::GetInstance()->Draw();

	// ���Ƶ���Ļ��
	StretchDIBits(hdc,
		0, 0, width, height,
		0, 0, width, height,
		backBuffer,
		&bitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);
}
