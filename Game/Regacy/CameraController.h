#pragma once 


// 카메라 컨트롤러는 카메라를 어떻게 컨트롤 할것인지만 결정함.
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
	void SetCameraMode(ECameraType type);
private:
	std::shared_ptr<GameObject> mMainCamera{ nullptr };
	std::shared_ptr<GameObject> mPlayer{ nullptr };

	int mCallBackSign{ -1 };
	ECameraType mCameraType{ ECameraType::FreeCamera };
};