#include "pch.h"
#include "Game/scripts/SCRIPT_Player.h"
#include "Renderer/resource/ParticleSystem.h"

std::shared_ptr<Slider> SCRIPT_Player::HPBar = nullptr;
std::shared_ptr<Slider> SCRIPT_Player::CoolTimeBar = nullptr;
ObjectPool<GameObject, 64>* SCRIPT_Player::BulletPool = nullptr;
Light SCRIPT_Player::Light{};
std::shared_ptr<ParticleSystem> SCRIPT_Player::Particle = nullptr;
TextureIndex SCRIPT_Player::ParticleTexture = 0;
IShadowCasterBase* SCRIPT_Player::ShadowCaster = nullptr;

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
	case PlayerColor_Y:
		mOwner->SetMaterial({ resourceMgr->GetMaterial("TankMaterialYellow") });
		break;
	default:
		break;
	}

	static constexpr int SPEED = 10.f;

	int sign{ NrSampler.Sample() };

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::Q, sign, [this]() { mOwner->GetTransform().Rotate(-Time.GetSmoothDeltaTime<float>()); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::E, sign, [this]() { mOwner->GetTransform().Rotate(Time.GetSmoothDeltaTime<float>()); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::W, sign, [this]() {  mIsMovingForward = true;   mOwner->GetTransform().Translate(mOwner->GetTransform().GetForward() * Time.GetSmoothDeltaTime<float>() * SPEED); });
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::S, sign, [this]() { mIsMovingBackward = true; mOwner->GetTransform().Translate(mOwner->GetTransform().GetForward() * -Time.GetSmoothDeltaTime<float>() * SPEED); });

	Input.RegisterKeyReleaseCallBack(DirectX::Keyboard::W, sign, [this]() { mIsMovingForward = false; });
	Input.RegisterKeyReleaseCallBack(DirectX::Keyboard::S, sign, [this]() { mIsMovingBackward = false; });

	mOwner->GetTransform().SetPosition({ 10.f,100.f,10.f });
	// mOwner->GetTransform().Scale({ 0.1f,0.1f,0.1f });
	
	Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::Space, sign, [this]() {
			if (CoolTimeBar->mValue < 99.f) return;
			auto bullet = BulletPool->Acquire();
			bullet->SetActive(true);
			CoolTimeBar->mValue = 0.f;
			
			ParticleVertex v{};
			
			v.position = mOwner->GetChild(1)->GetTransform().GetPosition();
			v.halfheight = 10.f;
			v.halfWidth = 10.f;
			v.texture = ParticleTexture;
			v.spritable = 1;
			v.spriteFrameInCol = 5;
			v.spriteFrameInRow = 5;
			v.spriteDuration = 1.f;
			v.direction = mOwner->GetChild(1)->GetTransform().GetForward();
			v.velocity = 100.f;
			v.totalLifeTime = 0.01f;
			v.lifeTime = 0.01f;
			v.type = ParticleType_emit;
			v.emitType = ParticleType_ember;
			v.remainEmit = 300;
			v.parentID = 0xFFFFFFFE;

			Particle->MakeParticle(v);
		}
		
	);

	Light->ambient = { 0.f,0.f,0.f,1.f };
	Light->diffuse = { 1.f,1.f,1.f,1.f };
	Light->specular = { 1.f,1.f,1.f,1.f };
	Light->position = { 0.f,0.f,0.f };
	Light->direction = mOwner->GetTransform().GetForward();
	Light->attenuation = { 0.1f,0.1f,0.1f };
	Light->falloff = 1.f;
	Light->internalTheta = std::cosf(DirectX::XMConvertToRadians(10.f));
	Light->externalPhi = std::cosf(DirectX::XMConvertToRadians(30.f));
	Light->enable = true;
	Light->type = LightType_Spot;
	Light->range = 200.f;

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

	Light->position = mOwner->GetTransform().GetPosition() + mOwner->GetTransform().GetUp() + mOwner->GetChild(1)->GetTransform().GetForward();
	Light->direction = mOwner->GetChild(1)->GetTransform().GetForward();
	
	ShadowCaster->SetPosition(mOwner->GetTransform().GetPosition() + DirectX::SimpleMath::Vector3{-70.f,30.f,0.f});
	ShadowCaster->SetFocus(mOwner->GetTransform().GetPosition());

}

void SCRIPT_Player::OnEnable()
{
}

void SCRIPT_Player::OnDisable()
{
}
