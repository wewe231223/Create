#include "pch.h"
#include "Game/scripts/SCRIPT_Player.h"


SCRIPT_Player::SCRIPT_Player(std::shared_ptr<GameObject> owner,std::shared_ptr<ResourceManager>& resourceMgr, PlayerColor color)
	: Script(owner)
{
	switch (color)
	{
	case PlayerColor_R:
		mOwner->SetMaterial({ resourceMgr->GetMaterial("TankMaterialRed") });
		break;
	case PlayerColor_G:
		mOwner->SetMaterial({ resourceMgr->GetMaterial("TankMaterialGreen") });
		break;
	case PlayerColor_B:
		mOwner->SetMaterial({ resourceMgr->GetMaterial("TankMaterialBlue") });
		break;
	default:
		break;
	}

	int sign{ NrSampler.Sample() };

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::Q, sign, [this]() { mOwner->GetTransform().Rotate(-Time.GetSmoothDeltaTime<float>()); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::E, sign, [this]() { mOwner->GetTransform().Rotate(Time.GetSmoothDeltaTime<float>()); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::W, sign, [this]() { mOwner->GetTransform().Translate(mOwner->GetTransform().GetForward() * Time.GetSmoothDeltaTime<float>() * 10.f); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::S, sign, [this]() { mOwner->GetTransform().Translate(mOwner->GetTransform().GetForward() * -Time.GetSmoothDeltaTime<float>() * 10.f); });

	mOwner->GetTransform().SetPosition({ 10.f,100.f,10.f });
	mOwner->GetTransform().Scale({ 0.1f,0.1f,0.1f });


}

SCRIPT_Player::~SCRIPT_Player()
{
}

void SCRIPT_Player::Awake()
{
}

void SCRIPT_Player::Update()
{
	if (Input.GetMouseState().rightButton) {
		mOwner->GetChild(1)->GetTransform().Rotate(Input.GetDeltaMouseX() * Time.GetSmoothDeltaTime<float>() * 0.3f, 0.f, 0.f);
	}
}

void SCRIPT_Player::OnEnable()
{
}

void SCRIPT_Player::OnDisable()
{
}