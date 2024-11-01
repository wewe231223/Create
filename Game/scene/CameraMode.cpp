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

constexpr float FREE_CAMERA_SPEED = 0.2f;
void FreeCameraMode::Enter()
{
	mInputCallBackSign = NrSampler.Sample();

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::W, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(mCamera->GetTransform().GetForward() * FREE_CAMERA_SPEED);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::S, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(mCamera->GetTransform().GetForward() * -FREE_CAMERA_SPEED);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::A, mInputCallBackSign, [this]() {
		auto right = mCamera->GetTransform().GetRight();
		right.y = 0.f;
		mCamera->GetTransform().Translate(right * FREE_CAMERA_SPEED);
		//mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Right * -0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::D, mInputCallBackSign, [this]() {
		auto right = mCamera->GetTransform().GetRight();
		right.y = 0.f;
		mCamera->GetTransform().Translate(right * -FREE_CAMERA_SPEED);
		//mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Right * 0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::Q, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Up * -FREE_CAMERA_SPEED);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::E, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Up * FREE_CAMERA_SPEED);
		});

	mCamera->GetTransform().ResetRotation();
}

void FreeCameraMode::Exit()
{
	Input.EraseCallBack(mInputCallBackSign);
}



// DeltaMouse 만큼의 회전을 했을 때, Forward 축이 +UP 이나 -UP 에 근접하면 회전을 하지 않아야 한다 
void FreeCameraMode::Update()
{
	mCamera->GetTransform().ResetRotation();

	static float yaw = 0.f;
	static float pitch = 0.f;

	yaw += Input.GetDeltaMouseX() * Time.GetSmoothDeltaTime<float>() * 0.2f ;
	pitch += Input.GetDeltaMouseY() * Time.GetSmoothDeltaTime<float>() * -0.2f;

	if (pitch > DirectX::XMConvertToRadians(80.f)) {
		pitch = DirectX::XMConvertToRadians(80.f);
	} 
	if (pitch < DirectX::XMConvertToRadians(-80.f)) {
		pitch = DirectX::XMConvertToRadians(-80.f);
	}
	
	mCamera->GetTransform().Rotate(yaw, pitch, 0.f);

}






//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							TPP Camera Mode								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


TPPCameraMode::TPPCameraMode(std::shared_ptr<Camera> camera,Transform& transform,const DirectX::SimpleMath::Vector3& offset)
	: CameraMode(camera), mOffset(offset), mTargetTransform(transform)
{

}

TPPCameraMode::~TPPCameraMode()
{
}

void TPPCameraMode::Enter()
{
	mCamera->GetTransform().ResetRotation();
}

void TPPCameraMode::Exit()
{
}

void TPPCameraMode::Update()
{
	auto forward = mTargetTransform.GetForward();
	auto right = mTargetTransform.GetRight();
	auto up = mTargetTransform.GetUp();

	mCamera->GetTransform().ResetRotation();
	mCamera->GetTransform().SetPosition(mTargetTransform.GetPosition() + DirectX::SimpleMath::Vector3{ mOffset.x* right + mOffset.y * up + mOffset.z * forward });
	mCamera->GetTransform().LookAt(mTargetTransform);
}
