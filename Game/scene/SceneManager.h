#pragma once 

class SceneManager {
	SceneManager();
	~SceneManager();
public:
	static SceneManager& GetInstance();
public:
	void Update();
	void Render();
	
	template<typename T> 
		requires std::derived_from<T, Scene>
	void ChangeScene();

};