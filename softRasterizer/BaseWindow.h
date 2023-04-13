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
		if (uMsg == WM_CREATE) {	// �ڴ��������¼�ʱ�������Ǵ���������ָ��
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);	// �ڴ����д����pstate��洢��lParam�У�������Ҫ���л�ȡԭʼ��void*ָ�루�����Ǵ���CreateWindowEx��ָ�룩
			pThis = reinterpret_cast<DERIVED_TYPE*>(pCreate->lpCreateParams);	// PS:�澭��˵���ײ�Ҫʹ�õ�ǿ��ת�������￴����
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);	// ����ȡ��ָ��洢�ڴ��ڵ�ʵ�������У�Ȼ�������Ժ�Ϳ���ͨ������GetWindowLongPtr()��ֱ�ӻ�ȡָ����
			pThis->m_hwnd = hwnd;	// ע�������pThis��BaseWindow��������Ϊ���Ǵ���CreateWindowEx��ָ����this����ȻpThis��thisӦ����ָ��ͬһ����ַ�Ĳ�ָͬ�롣
			// ��ôΪʲôpThis��ָ����ģ������DERIVED_TYPE�أ����Կ�����������࣬ʵ�������ģ�����;���BaseWindow���������ͣ�����this�����ͺ�pThis��������һ���ġ�
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
		wc.hInstance = GetModuleHandle(NULL);	// ��ʼ��Ϊ��
		wc.lpszClassName = ClassName();

		RegisterClass(&wc);

		m_hwnd = CreateWindowEx(dwExStyle, ClassName(), lpWindowName, dwStyle,
			x, y, nWidth, nHeight,
			hWndParent, hMenu, GetModuleHandle(NULL), this	// ����ཫwindowProc��װ�����У�ͬʱά���ô��ڵľ����ֱ�ӽ����������봴�����ں�����Ϊ�������ݵ�ָ�룬������������Ҫ�������ݵ�DERIVED_TYPE��WindowProc������
		);

		return (m_hwnd ? TRUE : FALSE);
	}

	HWND hwnd() const { return m_hwnd; }

protected:
	virtual PCWSTR ClassName() const = 0;	// ���ش���CLASS_NAME������ʶ���ڵ��ַ���
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



