#pragma once 

enum EGameObjectTag : size_t {
	Tag_Player,
	END,
};

enum ECameraType : UINT {
	FreeCamera = 0,
	FirstPersonCamera = 1,
	ThirdPersonCamera = 2,
};

struct ModelInfo {
	std::shared_ptr<IRendererEntity> model{ nullptr };
	std::vector<MaterialIndex> material{};
};
