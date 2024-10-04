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

	// TODO : 콜백을 삭제할 방법을 만들자 
	void RegisterKeyPressCallBack	(DirectX::Keyboard::Keys key,int sign,std::function<void()>&& callback);
	void RegisterKeyDownCallBack	(DirectX::Keyboard::Keys key,int sign, std::function<void()>&& callback);
	void RegisterKeyReleaseCallBack	(DirectX::Keyboard::Keys key,int sign, std::function<void()>&& callback);

	void EraseCallBack(int sign);
private:
	std::unique_ptr<DirectX::Keyboard> mKeyboard{ std::make_unique<DirectX::Keyboard>() };
	std::unique_ptr<DirectX::Mouse> mMouse{ std::make_unique<DirectX::Mouse>() };

	DirectX::Keyboard::State mKeyboardState{};
	DirectX::Mouse::State mMouseState{};

	DirectX::Keyboard::KeyboardStateTracker mKeyboardTracker{};
	DirectX::Mouse::ButtonStateTracker mMouseTracker{};
	
	std::array<std::vector<std::pair<std::function<void()>, int>>, DirectX::Keyboard::Keys::END> mKeyDownCallbacks{};
	std::array<std::vector<std::pair<std::function<void()>, int>>, DirectX::Keyboard::Keys::END> mKeyPressCallbacks{};
	std::array<std::vector<std::pair<std::function<void()>, int>>, DirectX::Keyboard::Keys::END> mKeyReleaseCallbacks{};
};


extern GInput Input;