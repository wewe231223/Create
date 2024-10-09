#include "pch.h"
#include "Game/gameobjects/CameraController.h"
#include "Game/GameWorld.h"

CameraController::CameraController()
{
	
}

CameraController::~CameraController()
{
}

void CameraController::Awake(GameWorld* world, GameObject* object)
{
	mMainCamera = world->GetMainCamera();
	
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::F1,NrSampler.Sample(), [this]() { 
		if (mCameraType != FreeCamera) {
			// 카메라 모드 변경 
			mCameraType = FreeCamera;
			// 기존 콜백 제거 
			Input.EraseCallBack(mCallBackSign);
			NrSampler.Free(mCallBackSign);

			mCallBackSign = NrSampler.Sample();
		}
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::F2, NrSampler.Sample(), [this]() {
		if (mCameraType != ThirdPersonCamera) {
			mCameraType = ThirdPersonCamera;
			Input.EraseCallBack(mCallBackSign);

			// 기존 콜백 제거 
			Input.EraseCallBack(mCallBackSign);
			NrSampler.Free(mCallBackSign);

			mCallBackSign = NrSampler.Sample();
		}
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::F3, NrSampler.Sample(), [this]() {
		if (mCameraType != FirstPersonCamera) {
			mCameraType = FirstPersonCamera;
			
			Input.EraseCallBack(mCallBackSign);

			// 기존 콜백 제거 
			Input.EraseCallBack(mCallBackSign);
			NrSampler.Free(mCallBackSign);

			mCallBackSign = NrSampler.Sample();
		}


		});

	auto player = world->GetGameObject(Tag_Player);
	// 아 몰라 나는 첫번째 플레이어만 따라갈래 
	mPlayer = player[0];
}

void CameraController::Update(GameWorld*, GameObject*)
{

}

void CameraController::LateUpdate(GameWorld*, GameObject*)
{
	switch (mCameraType)
	{
	case FreeCamera:
		break;
	case FirstPersonCamera:

		break;
	case ThirdPersonCamera:
	{
		auto transform = mMainCamera->GetComponent<Transform>();
		auto forward = mPlayer->GetComponent<Transform>()->GetFoward() * -1 * 10.f;
		transform->Translate(forward);
		transform->LookAt(mPlayer->GetComponent<Transform>()->GetPosition());
	}
		break;
	default:
		break;
	}

}

void CameraController::OnEnable(GameWorld*, GameObject*)
{
}

void CameraController::OnDisable(GameWorld*, GameObject*)
{
}

void CameraController::OnDestroy(GameWorld*, GameObject*)
{
}

void CameraController::SetCameraMode(ECameraType type)
{
	switch (type)
	{
	case FreeCamera:
		Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::W, mCallBackSign, [this]() {
			mMainCamera->GetComponent<Transform>()->Translate({ 0.f, 0.f, Time.GetDeltaTime<float>() });
			});

		Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::S, mCallBackSign, [this]() {
			mMainCamera->GetComponent<Transform>()->Translate({ 0.f, 0.f, Time.GetDeltaTime<float>() });
			});

		Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::A, mCallBackSign, [this]() {
			mMainCamera->GetComponent<Transform>()->Translate({ -Time.GetDeltaTime<float>(), 0.f, 0.f });
			});

		Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::D, mCallBackSign, [this]() {
			mMainCamera->GetComponent<Transform>()->Translate({ Time.GetDeltaTime<float>(), 0.f, 0.f });
			});

		Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::Q, mCallBackSign, [this]() {
			mMainCamera->GetComponent<Transform>()->Translate({ 0.f, -Time.GetDeltaTime<float>(), 0.f });
			});

		Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::E, mCallBackSign, [this]() {
			mMainCamera->GetComponent<Transform>()->Translate({ 0.f, Time.GetDeltaTime<float>(), 0.f });
			});
		break;
		// 마우스로만 움직이니까 딱히..? 
	case ThirdPersonCamera:
		break;
	case FirstPersonCamera:
		break;
	default:
		break;
	}
}



