#include "pch.h"
#include "core/window.h"

Window::Window(HWND windowHandle)
	:mWindowHandle(windowHandle)
{

}

Window::~Window()
{

}

HWND Window::GetWindowHandle() const
{
	return mWindowHandle;
}

EWindowStyle Window::GetWindowStyle() const
{
	return mWindowStyle;
}

template<typename T>
T Window::GetWindowWidth() const
{
	return static_cast<T>(mWidth);
}

template<typename T>
T Window::GetWindowHeight() const
{
	return static_cast<T>(mHeight);
}


void Window::ChangeWindowStyle(EWindowStyle style, size_t width, size_t height)
{
	RECT adjWindowRect{ 0,0,static_cast<long>(width), static_cast<long>(height) };
	RECT windowRect{};
	GetWindowRect(mWindowHandle, &windowRect);

	switch (style) {
	case EWindowStyle::Windowed:
		::AdjustWindowRectEx(&adjWindowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
		SetWindowLong(mWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowLong(mWindowHandle, GWL_EXSTYLE, 0);
		SetWindowPos(mWindowHandle, HWND_NOTOPMOST, windowRect.left, windowRect.top, adjWindowRect.right - adjWindowRect.left, adjWindowRect.bottom - adjWindowRect.top, NULL);
		ShowWindow(mWindowHandle, SW_SHOW);
		mWindowStyle = style;

		mWidth = static_cast<size_t>(adjWindowRect.right - adjWindowRect.left);
		mHeight = static_cast<size_t>(adjWindowRect.left - adjWindowRect.top);
		break;
	case EWindowStyle::Borderless:
		::AdjustWindowRectEx(&adjWindowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
		SetWindowLong(mWindowHandle, GWL_STYLE, WS_POPUP);
		SetWindowLong(mWindowHandle, GWL_EXSTYLE, 0);
		SetWindowPos(mWindowHandle, HWND_NOTOPMOST, windowRect.left, windowRect.top, adjWindowRect.right - adjWindowRect.left, adjWindowRect.bottom - adjWindowRect.top, NULL);
		ShowWindow(mWindowHandle, SW_SHOW);
		mWindowStyle = style;


		mWidth = static_cast<size_t>(adjWindowRect.right - adjWindowRect.left);
		mHeight = static_cast<size_t>(adjWindowRect.left - adjWindowRect.top);
		break;
	case EWindowStyle::Fullscreen:
		SetWindowLong(mWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		SetWindowLong(mWindowHandle, GWL_EXSTYLE, WS_EX_TOPMOST);
		SetWindowPos(mWindowHandle, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN), NULL);
		ShowWindow(mWindowHandle, SW_SHOWMAXIMIZED);
		mWindowStyle = style;

		mWidth = static_cast<size_t>(::GetSystemMetrics(SM_CXSCREEN));
		mHeight = static_cast<size_t>(::GetSystemMetrics(SM_CYSCREEN));
		break;
	case EWindowStyle::None:
		break;
	default:
		break;
	}

}
