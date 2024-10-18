#include "pch.h"
#include "Game/scene/CameraMode.h"
#include "Game/scene/Camera.h"


//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							CameraMode Base								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

CameraMode::CameraMode(std::shared_ptr<class Camera> camera)
	: mCamera(camera), mInputCallBackSign(std::numeric_limits<int>::min())
{
}

CameraMode::~CameraMode()
{
	NrSampler.Free(mInputCallBackSign);
}

std::shared_ptr<CameraMode> CameraMode::ChangeCameraMode(std::shared_ptr<CameraMode>& mode)
{
	(this)->Exit();
	mode->Enter();
	return mode;
}








//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Free Camera Mode							//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////



FreeCameraMode::FreeCameraMode(std::shared_ptr<class Camera> camera)
	: CameraMode(camera)
{
}

FreeCameraMode::~FreeCameraMode()
{
}

void FreeCameraMode::Enter()
{
	mInputCallBackSign = NrSampler.Sample();

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::W, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(mCamera->GetTransform().GetForward() * 0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::S, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(mCamera->GetTransform().GetForward() * -0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::A, mInputCallBackSign, [this]() {
		auto right = mCamera->GetTransform().GetRight();
		right.y = 0.f;
		mCamera->GetTransform().Translate(right * -0.1f);
		//mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Right * -0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::D, mInputCallBackSign, [this]() {
		auto right = mCamera->GetTransform().GetRight();
		right.y = 0.f;
		mCamera->GetTransform().Translate(right * 0.1f);
		//mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Right * 0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::Q, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Up * -0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::E, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Up * 0.1f);
		});
}

void FreeCameraMode::Exit()
{
	Input.EraseCallBack(mInputCallBackSign);
	NrSampler.Free(mInputCallBackSign);
}

void FreeCameraMode::Update()
{
	mCamera->GetTransform().Rotate(DirectX::XMConvertToRadians(Input.GetDeltaMouseX() * -0.15f), DirectX::XMConvertToRadians(Input.GetDeltaMouseY() * 0.15f), 0.f);

}
