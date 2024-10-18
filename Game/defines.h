#pragma once 

enum EGameObjectTag : size_t {
	Tag_Player,
	END,
};

enum ECameraType : UINT {
	CT_FreeCamera = 0,
	CT_FirstPersonCamera = 1,
	CT_ThirdPersonCamera = 2,
	CT_END,
};

struct ModelInfo {
	std::shared_ptr<I3DRenderable> model{ nullptr };
	std::vector<MaterialIndex> material{};
};
