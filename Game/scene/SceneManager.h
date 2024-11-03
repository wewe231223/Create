#pragma once 

class SceneManager {
	SceneManager();
	~SceneManager();
public:
	static SceneManager& GetInstance();
public:
	void Initialize(DxRenderer* renderer);

	void Update();

	void CheckSceneChanged();

	template<typename T> 
		requires std::derived_from<T, Scene>
	void ChangeScene();
private:
	std::shared_ptr<Scene> mCurrentScene{ nullptr };
	std::shared_ptr<Scene> mNextScene{ nullptr };
	DxRenderer* mDxRenderer{};
};

template<typename T> requires std::derived_from<T, Scene>
inline void SceneManager::ChangeScene()
{
	if (not mNextScene) {
		mNextScene = std::make_shared<T>();
	}
}
