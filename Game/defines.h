#pragma once 

enum EGameObjectTag : size_t{
	Player,
	END,
};

struct ModelInfo {
	std::shared_ptr<IRendererEntity> model{ nullptr };
	std::vector<MaterialIndex> material{};
};
