#include "Window.hpp"
#include <windowsx.h>

namespace Platform
{
	Window::Window(uint16_t width, uint16_t height, std::wstring_view windowName) : m_Height(height), m_Width(width)
	{
		WNDCLASSEX wc = { };

		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = MsgHandling;
		wc.hInstance = this->m_hInstance;
		wc.lpszClassName = L"ApplicationProgram";

		RegisterClassEx(&wc);
        m_hwnd = CreateWindowExW(0, L"ApplicationProgram", windowName.data(), WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, this->m_hInstance, nullptr);

        ShowWindow(m_hwnd, SW_SHOWNORMAL);

		m_hdc = GetDC(m_hwnd);
	}
	
	LRESULT Window::MsgHandling(_In_ HWND hWND, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
	{
		switch (Msg)
		{
			case WM_DESTROY:
			{
				PostQuitMessage(0); 
				return 0;
			}
			case WM_CLOSE:
			{
				PostQuitMessage(0);
				return 0;
			}
			case WM_QUIT:
			{
				PostQuitMessage(0);
				return 0;
			}

			// keyboard
			case WM_KEYDOWN: s_key[wParam] = true; return 0;

			case WM_KEYUP: s_key[wParam] = false; return 0;

			// mouse
			case WM_MOUSEMOVE: s_mousePos[0] = GET_X_LPARAM(lParam); s_mousePos[1] = GET_Y_LPARAM(lParam); return 0;

			case WM_LBUTTONDOWN: s_key[VK_LBUTTON] = true; return 0;

			case WM_LBUTTONUP: s_key[VK_LBUTTON] = false; return 0;

			case WM_MBUTTONDOWN: s_key[VK_MBUTTON] = true; return 0;

			case WM_MBUTTONUP: s_key[VK_MBUTTON] = false; return 0;

			case WM_RBUTTONDOWN: s_key[VK_RBUTTON] = true; return 0;

			case WM_RBUTTONUP: s_key[VK_RBUTTON] = false; return 0;

			case WM_MOUSEWHEEL: s_mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam); return 0;

		}
		return DefWindowProc(hWND, Msg, wParam, lParam);
	}
};
