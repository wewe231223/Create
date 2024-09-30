#pragma once 

class GInput {
public:
	GInput();
	~GInput();

	void Update();


	DirectX::Keyboard::State& GetKeyboardState();
	DirectX::Mouse::State& GetMouseState();
	DirectX::Keyboard::KeyboardStateTracker& GetKeyboardTracker();
	DirectX::Mouse::ButtonStateTracker& GetMouseTracker();
private:
	std::unique_ptr<DirectX::Keyboard> mKeyboard{ std::make_unique<DirectX::Keyboard>() };
	std::unique_ptr<DirectX::Mouse> mMouse{ std::make_unique<DirectX::Mouse>() };

	DirectX::Keyboard::State mKeyboardState{};
	DirectX::Mouse::State mMouseState{};

	DirectX::Keyboard::KeyboardStateTracker mKeyboardTracker{};
	DirectX::Mouse::ButtonStateTracker mMouseTracker{};
};


extern GInput Input;