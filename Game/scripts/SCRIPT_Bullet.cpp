#include "pch.h"
#include "Game/scripts/SCRIPT_Bullet.h"
#include "Game/gameobject/TerrainObject.h"


std::shared_ptr<GameObject> SCRIPT_Bullet::mPlayer = nullptr;
std::shared_ptr<TerrainCollider> SCRIPT_Bullet::mTerrainCollider = nullptr;


SCRIPT_Bullet::SCRIPT_Bullet(std::shared_ptr<GameObject> owner)
	: Script(owner)
{
	owner->GetTransform().Scale({ 0.1f,0.1f,0.1f });
}

SCRIPT_Bullet::~SCRIPT_Bullet()
{
}

void SCRIPT_Bullet::Awake()
{
}

constexpr float bulletSpeed = 100.f;

void SCRIPT_Bullet::Update()
{

	if (mTimeOut < 0.f) {
		mOwner->SetActive(false);
	}

	if (mTerrainCollider->TerrainCollision(mOwner->GetTransform())) {
		mOwner->SetActive(false);
	}

	mOwner->GetTransform().Translate(mDirection * Time.GetSmoothDeltaTime<float>() * 100.f);
	mTimeOut -= Time.GetSmoothDeltaTime<float>();
}

void SCRIPT_Bullet::OnEnable()
{
	mOwner->GetTransform().SetPosition(mPlayer->GetChild(1)->GetTransform().GetPosition());
	mDirection = mPlayer->GetChild(1)->GetTransform().GetForward();
}

void SCRIPT_Bullet::OnDisable()
{
	mTimeOut = 5.f;
}
