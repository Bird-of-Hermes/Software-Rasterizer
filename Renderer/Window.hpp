#ifndef WINDOW_HPP
#define WINDOW_HPP

#ifndef NO_MIN_MAX
#define NO_MIN_MAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Windows.h"
#include <iostream>

class Application;
namespace Platform
{
	class Window
	{
		friend class Application;
	public:
		explicit Window(uint16_t Width = 800, uint16_t Height = 600, std::wstring_view windowName = L"Default Window");

		inline static int16_t GetMouseX() noexcept { return s_mousePos[0]; }
		inline static int16_t GetMouseY() noexcept { return s_mousePos[1]; }
		inline static int32_t GetMouseWheelTurn() noexcept { return s_mouseWheel; }
		inline static const bool KeyDown(uint8_t vkcode) noexcept { return s_key[vkcode]; }

	private:
		uint16_t m_Width;
		uint16_t m_Height;
		HINSTANCE m_hInstance = nullptr;
		HWND m_hwnd = nullptr;
		HDC m_hdc = nullptr;

		static LRESULT CALLBACK MsgHandling(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

		inline static bool s_key[256] = {};
		inline static int16_t s_mousePos[2] = {};
		inline static int32_t s_mouseWheel = {};
	};
};

#endif