#include "pch.h"
#include "Game/Input.h"

std::array<unsigned char, DirectX::Keyboard::Keys::END>  keys{};
GInput::GInput()
{
	std::iota(keys.begin(), keys.end(), 0);
}

GInput::~GInput()
{
}

void GInput::Update()
{
	mKeyboardState = mKeyboard->GetState();
	mMouseState = mMouse->GetState();

	mKeyboardTracker.Update(mKeyboardState);
	mMouseTracker.Update(mMouseState);

	for (auto& key : keys) {
		if (mKeyboardState.IsKeyDown(static_cast<DirectX::Keyboard::Keys>(key))) {
			if (mKeyboardTracker.IsKeyPressed(static_cast<DirectX::Keyboard::Keys>(key))) {
				for (auto& callback : mKeyDownCallbacks[key]) {
					std::invoke(callback);
				}
			}
			else {
				for (auto& callback : mKeyPressCallbacks[key]) {
					std::invoke(callback);
				}
			}
		} else if (mKeyboardTracker.IsKeyReleased(static_cast<DirectX::Keyboard::Keys>(key))) {
			for (auto& callback : mKeyReleaseCallbacks[key]) {
				std::invoke(callback);
			}
		}
	}

}

DirectX::Keyboard::State& GInput::GetKeyboardState()
{
	return std::ref(mKeyboardState);
}

DirectX::Mouse::State& GInput::GetMouseState()
{
	return std::ref(mMouseState);
}

DirectX::Keyboard::KeyboardStateTracker& GInput::GetKeyboardTracker()
{
	return std::ref(mKeyboardTracker);
}

DirectX::Mouse::ButtonStateTracker& GInput::GetMouseTracker()
{
	return std::ref(mMouseTracker);
}

void GInput::RegisterKeyPressCallBack(DirectX::Keyboard::Keys key, std::function<void()>&& callback)
{
	mKeyPressCallbacks[key].emplace_back(callback);
}

void GInput::RegisterKeyDownCallBack(DirectX::Keyboard::Keys key, std::function<void()>&& callback)
{
	mKeyDownCallbacks[key].emplace_back(callback);
}

void GInput::RegisterKeyReleaseCallBack(DirectX::Keyboard::Keys key, std::function<void()>&& callback)
{
	mKeyReleaseCallbacks[key].emplace_back(callback);
}

GInput Input{};
