#pragma once 
class TerrainImage {
public:
	/// 정사각인 경우 
	TerrainImage(const fs::path& path);

	/// 정사각이 아닌 경우 
	TerrainImage(const fs::path& path,int width, int height);
	~TerrainImage();

	void CreatePatch(std::vector<DirectX::XMFLOAT3>& positions, std::vector<DirectX::XMFLOAT2>& uv1, std::vector<DirectX::XMFLOAT2>& uv2, int zStart, int zEnd, int xStart, int xEnd, int patchLength = 4);
	DirectX::SimpleMath::Vector3	GetNormal(int x, int z, DirectX::SimpleMath::Vector3 scale) const;
	float							GetHeight(float x, float z) const;
	int								GetWidth() const;
	int								GetHeight() const;
private:
	std::unique_ptr<BYTE[]> mPixels{ nullptr };
	int mWidth = 0;
	int mHeight = 0;
};



