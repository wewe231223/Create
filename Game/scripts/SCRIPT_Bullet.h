#pragma once 

class SCRIPT_Bullet : public Script {
public:
	SCRIPT_Bullet(std::shared_ptr<GameObject> owner);
	virtual ~SCRIPT_Bullet();
public:
	static std::shared_ptr<GameObject> mPlayer;
	static std::shared_ptr<class TerrainCollider> mTerrainCollider;
public:
	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
private:
	DirectX::SimpleMath::Vector3 mDirection{ 0.f,0.f,1.f };
	float mTimeOut{ 5.f };

};