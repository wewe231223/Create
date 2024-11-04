#include "pch.h"
#include "Game/scripts/SCRIPT_Player.h"

std::shared_ptr<Slider> SCRIPT_Player::HPBar = nullptr;
std::shared_ptr<Slider> SCRIPT_Player::CoolTimeBar = nullptr;
ObjectPool<GameObject, 64>* SCRIPT_Player::BulletPool = nullptr;

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

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::A, sign, [this]() { mOwner->GetTransform().Rotate(-Time.GetSmoothDeltaTime<float>()); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::D, sign, [this]() { mOwner->GetTransform().Rotate(Time.GetSmoothDeltaTime<float>()); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::W, sign, [this]() {  mIsMovingForward = true;   mOwner->GetTransform().Translate(mOwner->GetTransform().GetForward() * Time.GetSmoothDeltaTime<float>() * 10.f); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::S, sign, [this]() { mIsMovingBackward = true; mOwner->GetTransform().Translate(mOwner->GetTransform().GetForward() * -Time.GetSmoothDeltaTime<float>() * 10.f); });

	Input.RegisterKeyReleaseCallBack(DirectX::Keyboard::W, sign, [this]() { mIsMovingForward = false; });
	Input.RegisterKeyReleaseCallBack(DirectX::Keyboard::S, sign, [this]() { mIsMovingBackward = false; });

	mOwner->GetTransform().SetPosition({ 10.f,100.f,10.f });
	mOwner->GetTransform().Scale({ 0.1f,0.1f,0.1f });
	
	Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::Space, sign, [this]() {
			if (CoolTimeBar->mValue < 99.f) return;
			auto bullet = BulletPool->Acquire();
			bullet->SetActive(true);
			CoolTimeBar->mValue = 0.f;
		}
	);
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

	HPBar->mValue = mHP;
	CoolTimeBar->mValue += Time.GetSmoothDeltaTime<float>() * 100.f;

	if (mIsMovingForward) {
		mOwner->GetChild(2)->GetTransform().Rotate(0.f, Time.GetSmoothDeltaTime<float>() * 10.f, 0.f);
		mOwner->GetChild(3)->GetTransform().Rotate(0.f, Time.GetSmoothDeltaTime<float>() * 10.f, 0.f);
		mOwner->GetChild(4)->GetTransform().Rotate(0.f, Time.GetSmoothDeltaTime<float>() * 10.f, 0.f);
		mOwner->GetChild(5)->GetTransform().Rotate(0.f, Time.GetSmoothDeltaTime<float>() * 10.f, 0.f);
	}

	if (mIsMovingBackward) {
		mOwner->GetChild(2)->GetTransform().Rotate(0.f, -Time.GetSmoothDeltaTime<float>() * 10.f, 0.f);
		mOwner->GetChild(3)->GetTransform().Rotate(0.f, -Time.GetSmoothDeltaTime<float>() * 10.f, 0.f);
		mOwner->GetChild(4)->GetTransform().Rotate(0.f, -Time.GetSmoothDeltaTime<float>() * 10.f, 0.f);
		mOwner->GetChild(5)->GetTransform().Rotate(0.f, -Time.GetSmoothDeltaTime<float>() * 10.f, 0.f);
	}

	
}

void SCRIPT_Player::OnEnable()
{
}

void SCRIPT_Player::OnDisable()
{
}
