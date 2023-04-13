#include "BaseWindow.h"
LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:	// 如果不对此消息做处理，那么返回的DefWindowProc中会对WM_CLOSE默认调用DestroyWindow
		if (MessageBox(m_hwnd, L"quit?", L"Keep open", MB_OKCANCEL) == IDOK) {
			DestroyWindow(m_hwnd);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);	// 当收到关闭窗口的输入后，消息队列会收到WM_DESTROY消息，此时需要调用PostQuitMessage(0)在消息队列中放置WM_QUIT消息告知消息循环结束。如果不调用则窗口关闭但程序继续运行	
		return 0;
	case WM_PAINT:
	{
		//consoleLog("WM_PAINT");
		PAINTSTRUCT ps;		// ps.rcPaint包含此次绘制的区域大小，top和left默认为0，bottom和right默认为窗口height和width，可以修改这几个参数调整绘制区域
		HDC hdc = BeginPaint(m_hwnd, &ps);	// 开始绘制

		Draw(hdc);

		time_t curT = clock();
		int fps = 1000 / (curT - lastT);
		lastT = curT;
		DrawText(hdc, to_wstring(fps).c_str(), -1, &ps.rcPaint, DT_CENTER);

		EndPaint(m_hwnd, &ps);	// 结束绘制
	}
	return 0;
	case WM_SIZE:	// 以WM_SIZE为例，wParam是一个标志，指示窗口是最小化 / 最大化 / 调整大小，lParam包含窗口的新宽高，但是宽高值都是16bit值，所以需要用高低字来分别获取
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

	// 使用双缓冲机制，先将渲染结果绘制到backBuffer中
	// 注意WIN32中BITMAPINFO的rgbquad颜色格式为blue green red，和一般的rgb是相反的
	unsigned char* backBuffer = Rasterizer::GetInstance()->Draw();

	// 绘制到屏幕上
	StretchDIBits(hdc,
		0, 0, width, height,
		0, 0, width, height,
		backBuffer,
		&bitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);
}
