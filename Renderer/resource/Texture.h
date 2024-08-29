#pragma once

class Texture {
public:
	Texture();
	~Texture();


private:
private:
	DirectX::ScratchImage mImage;

	ComPtr<ID3D12Resource> mResource{ nullptr };
};