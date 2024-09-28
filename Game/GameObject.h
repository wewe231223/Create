#pragma once 

class GameObject {
public:
	// 비어있는 게임 오브젝트도 있을 수 있다. 카메라? 
	GameObject();
	~GameObject() = default;
	
	void Update();

};

