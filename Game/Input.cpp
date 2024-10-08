#include "pch.h"
#include "Game/Input.h"
#include "Game/utils/NonReplacementSampler.h"

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
	mPrevX = mMouseState.x;
	mPrevY = mMouseState.y;

	mKeyboardState = mKeyboard->GetState();
	mMouseState = mMouse->GetState();

	mKeyboardTracker.Update(mKeyboardState);
	mMouseTracker.Update(mMouseState);

	for (auto& key : keys) {
		if (mKeyboardState.IsKeyDown(static_cast<DirectX::Keyboard::Keys>(key))) {
			if (mKeyboardTracker.IsKeyPressed(static_cast<DirectX::Keyboard::Keys>(key))) {
				for (auto& [ callback,sign ] : mKeyDownCallbacks[key]) {
					std::invoke(callback);
				}
			}
			else {
				for (auto& [callback, sign] : mKeyPressCallbacks[key]) {
					std::invoke(callback);
				}
			}
		} else if (mKeyboardTracker.IsKeyReleased(static_cast<DirectX::Keyboard::Keys>(key))) {
			for (auto& [callback, sign] : mKeyReleaseCallbacks[key]) {
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

void GInput::RegisterKeyPressCallBack(DirectX::Keyboard::Keys key, int sign, std::function<void()>&& callback)
{
	mKeyPressCallbacks[key].emplace_back(callback,sign);

}

void GInput::RegisterKeyDownCallBack(DirectX::Keyboard::Keys key, int sign, std::function<void()>&& callback)
{
	mKeyDownCallbacks[key].emplace_back(callback,sign);
}

void GInput::RegisterKeyReleaseCallBack(DirectX::Keyboard::Keys key, int sign, std::function<void()>&& callback)
{
	mKeyReleaseCallbacks[key].emplace_back(callback,sign);
}

float GInput::GetDeltaMouseX() const
{
	return static_cast<float>(mMouseState.x - mPrevX);
}

float GInput::GetDeltaMouseY() const
{
	return static_cast<float>(mMouseState.y - mPrevY);
}

void GInput::EraseCallBack(int sign)
{
	for (auto& callbacks : mKeyPressCallbacks) {
		std::erase_if(callbacks, [sign](auto& callback) {return callback.second == sign; });
	}

	for (auto& callbacks : mKeyDownCallbacks) {
		std::erase_if(callbacks, [sign](auto& callback) {return callback.second == sign; });
	}

	for (auto& callbacks : mKeyReleaseCallbacks) {
		std::erase_if(callbacks, [sign](auto& callback) {return callback.second == sign; });
	}
}



GInput Input{};
