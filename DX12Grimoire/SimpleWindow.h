#pragma once
#include <WiNDoWS.h>

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class SSimpleWindow
{
public:
	SSimpleWindow(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow) : AppInstanceHandle(hInstance), pCmdLine(lpCmdLine), nCmdShow(nCmdShow) {};

	HRESULT Initialize(const int Window_Width, const int Window_Height);

	const HRESULT Show_Window();

	void Release();

	const HWND GetWindowHandle() { return hWindow; }

private:
	HINSTANCE AppInstanceHandle;
	LPSTR pCmdLine;
	int nCmdShow;

	WNDCLASSEX WindowClass = {};
	HWND hWindow = nullptr;
};