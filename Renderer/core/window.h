#pragma once 

enum class EWindowStyle {
	Windowed,
	Borderless,
	Fullscreen,
	None
};


constexpr size_t gDefaultWidth = 800;
constexpr size_t gDefaultHeight = 600;

class Window {
public:
	Window(HWND windowHandle);
	~Window();
	Window(const Window&)						= delete;
	Window& operator=(const Window&)			= delete;
	Window(Window&& other) noexcept				= delete;
	Window& operator=(Window&& other) noexcept	= delete;

	HWND GetWindowHandle() const;
	EWindowStyle GetWindowStyle() const;
	
	template<typename T>
	T GetWindowWidth() const 
	{
		return static_cast<T>(mWidth);
	}

	template<typename T>
	T GetWindowHeight() const
	{
		return static_cast<T>(mHeight);
	}


	void ChangeWindowStyle(EWindowStyle style, size_t width = gDefaultWidth, size_t height = gDefaultHeight);
private:
	HWND mWindowHandle{ nullptr };

	EWindowStyle mWindowStyle{ EWindowStyle::Windowed };

	size_t mWidth{ 0 };
	size_t mHeight{ 0 };
};

