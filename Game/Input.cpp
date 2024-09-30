#include "pch.h"
#include "Game/Input.h"

GInput::GInput()
{
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

GInput Input{};
