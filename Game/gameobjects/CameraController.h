#pragma once 

class CameraController : public MonoBehavior {
public:
	CameraController();
	~CameraController();
public:
	virtual void Awake		(class GameWorld*, GameObject*) override;
	virtual void Update		(class GameWorld*, GameObject*) override;
	virtual void LateUpdate	(class GameWorld*, GameObject*) override;
	virtual void OnEnable	(class GameWorld*, GameObject*) override;
	virtual void OnDisable	(class GameWorld*, GameObject*) override;
	virtual void OnDestroy	(class GameWorld*, GameObject*) override;
private:
	std::shared_ptr<GameObject> mTPCamera{ nullptr };
	std::shared_ptr<GameObject> mFPCamera{ nullptr };
};