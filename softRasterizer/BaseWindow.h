#pragma once

#ifndef _BASE_WINDOW_H_
#define _BASE_WINDOW_H_

#include <iostream>
#include <windows.h>
#include "Rasterizer.h"
#include <time.h>
#include <string>

template<class DERIVED_TYPE>
class BaseWindow
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		DERIVED_TYPE* pThis = NULL;
		if (uMsg == WM_CREATE) {	// 在创建窗口事件时保存我们创建的数据指针
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);	// 在窗口中传入的pstate会存储在lParam中，我们需要从中获取原始的void*指针（即我们传入CreateWindowEx的指针）
			pThis = reinterpret_cast<DERIVED_TYPE*>(pCreate->lpCreateParams);	// PS:面经里说轻易不要使用的强制转换在这里看到了
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);	// 将获取的指针存储在窗口的实例数据中，然后我们以后就可以通过调用GetWindowLongPtr()来直接获取指针了
			pThis->m_hwnd = hwnd;	// 注意这里的pThis是BaseWindow类自身，因为我们传入CreateWindowEx的指针是this，当然pThis和this应该是指向同一个地址的不同指针。
			// 那么为什么pThis的指针是模板类型DERIVED_TYPE呢？可以看下面的派生类，实际上这个模板类型就是BaseWindow的派生类型，所以this的类型和pThis的类型是一样的。
		}
		else {
			pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		if (pThis) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

	}
	BaseWindow() :m_hwnd(NULL) {}

	BOOL Create(
		PCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		HWND hWndParent = 0,
		HMENU hMenu = 0
	) {
		WNDCLASS wc = { 0 };

		wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
		wc.hInstance = GetModuleHandle(NULL);	// 初始化为空
		wc.lpszClassName = ClassName();

		RegisterClass(&wc);

		m_hwnd = CreateWindowEx(dwExStyle, ClassName(), lpWindowName, dwStyle,
			x, y, nWidth, nHeight,
			hWndParent, hMenu, GetModuleHandle(NULL), this	// 这个类将windowProc封装在类中，同时维护该窗口的句柄，直接将该类自身传入创建窗口函数作为传递数据的指针，而真正包裹需要传递数据的DERIVED_TYPE在WindowProc方法中
		);

		return (m_hwnd ? TRUE : FALSE);
	}

	HWND hwnd() const { return m_hwnd; }

protected:
	virtual PCWSTR ClassName() const = 0;	// 返回窗口CLASS_NAME，即标识窗口的字符串
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wparam, LPARAM lparam) = 0;

	HWND m_hwnd;
};

class MainWindow : public BaseWindow<MainWindow>
{
public:
	time_t lastT = 0;
	PCWSTR ClassName() const { return L"Sample Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

template <typename T>
inline void consoleLog(T t)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::cout << t << std::endl;
}


inline void OnSize(HWND hwnd, UINT flag, int width, int height)
{
}

void Draw(HDC hdc);

#endif // !_BASE_WINDOW_H_



