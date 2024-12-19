#pragma once 
enum PlayerColor {
	PlayerColor_R,
	PlayerColor_G,
	PlayerColor_B,
	PlayerColor_Y,
};


class SCRIPT_Player : public Script {
public:
	SCRIPT_Player(std::shared_ptr<GameObject> owner, std::shared_ptr<ResourceManager>& resourceMgr, PlayerColor color);
	virtual ~SCRIPT_Player();
public:
	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	static std::shared_ptr<Slider> HPBar;
	static std::shared_ptr<Slider> CoolTimeBar;
	static ObjectPool<GameObject, 64>* BulletPool;
	static Light Light;
	static std::shared_ptr<class ParticleSystem> Particle;
	static TextureIndex ParticleTexture;
private:
	bool mIsMovingForward{ false };
	bool mIsMovingBackward{ false };
	float mHP{ 100.f };
};