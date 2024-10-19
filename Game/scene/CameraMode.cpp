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
		mCamera->GetTransform().Translate(right * 0.1f);
		//mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Right * -0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::D, mInputCallBackSign, [this]() {
		auto right = mCamera->GetTransform().GetRight();
		right.y = 0.f;
		mCamera->GetTransform().Translate(right * -0.1f);
		//mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Right * 0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::Q, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Up * -0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::E, mInputCallBackSign, [this]() {
		mCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Up * 0.1f);
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
	auto cameraRotation = mCamera->GetTransform().GetRotation();
	auto dx = DirectX::XMConvertToRadians(Input.GetDeltaMouseX() * Time.GetSmoothDeltaTime<float>() * 30.f);
	auto dy = DirectX::XMConvertToRadians(Input.GetDeltaMouseY() * Time.GetDeltaTime<float>() * - 30.f );
	auto rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(dx,dy,0.f);
	
	rotation.Normalize();
	cameraRotation = cameraRotation.Concatenate(cameraRotation, rotation);
	cameraRotation.Normalize();

	auto forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, cameraRotation);

	auto dot = DirectX::SimpleMath::Vector3::Up.Dot(forward);

	if (dot < 0.9f and dot > -0.9f) {
		mCamera->GetTransform().Rotate(rotation);
	}

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
	mCamera->GetTransform().LookAt(mTargetTransform.GetPosition());
}
