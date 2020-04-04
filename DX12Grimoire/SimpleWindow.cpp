#include "SimpleWindow.h"

HRESULT SSimpleWindow::Initialize(const int Window_Width, const int Window_Height)
{
	// �E�B���h�E�N���X�̐ݒ�A�o�^
	WindowClass = {};

	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.lpfnWndProc = WindowProcedure;
	WindowClass.lpszClassName = L"Grimoire";
	WindowClass.hInstance = AppInstanceHandle;

	RegisterClassEx(&WindowClass);


	// �E�B���h�E�̑傫����ݒ�AAdjust�ŋ��E���␳
	RECT wrc = { 0, 0, Window_Width, Window_Height };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);


	// �E�B���h�E�����
	hWindow = CreateWindow(
		WindowClass.lpszClassName,
		L"Grimoire Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		WindowClass.hInstance,
		nullptr);

	return S_OK;
}

const HRESULT SSimpleWindow::Show_Window()
{
	ShowWindow(hWindow, SW_SHOW);
	return S_OK;
}

void SSimpleWindow::Release()
{
	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
}


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}