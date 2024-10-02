#pragma once 

// ref 당할 때마다 재질 정보를 저장하고, uploadmaterials 페이즈에 디폴트 버퍼로 올리자. 
class Mesh {
public:
	// 경계값 포함. 
	Mesh(D3D12_PRIMITIVE_TOPOLOGY topology,UINT indexBegin,UINT indexCount);
	~Mesh();

	void Ref(const MaterialIndex& material);
	void UploadMaterialInfo(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList);
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList,UINT instance) const;
private:
	D3D12_PRIMITIVE_TOPOLOGY mTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	UINT mIndexBegin{ 0 };
	UINT mIndexCount{ 0 };
	UINT mMaterialIndex{ 0 };

	std::vector<MaterialIndex> mMaterials{};
	std::unique_ptr<class DefaultBuffer> mMaterialBuffer{ nullptr };
};

