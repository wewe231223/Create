#pragma once 

class SceneManager {
	SceneManager();
	~SceneManager();
public:
	static SceneManager& GetInstance();
public:
	void Initialize(DxRenderer* renderer);

	void Update();

	template<typename T> 
		requires std::derived_from<T, Scene>
	void ChangeScene();
private:
	std::shared_ptr<Scene> mCurrentScene{};
	DxRenderer* mDxRenderer{};
};

template<typename T> requires std::derived_from<T, Scene>
inline void SceneManager::ChangeScene()
{
	if (mCurrentScene) {
		mCurrentScene.reset();
		mCurrentScene = std::make_shared<T>();
		mDxRenderer->LoadScene(mCurrentScene);
	}
	else {
		mCurrentScene = std::make_shared<T>();
		mDxRenderer->LoadScene(mCurrentScene);
	}
}
