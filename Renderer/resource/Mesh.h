#pragma once 

// ref 당할 때마다 재질 정보를 저장하고, uploadmaterials 페이즈에 디폴트 버퍼로 올리자. 
class Mesh {
	struct MeshContext {
		ComPtr<ID3D12Resource> mBuffer{ nullptr };
		MaterialIndex* mBufferPtr{ nullptr };
	};
public:
	// 경계값 포함. 
	Mesh(ComPtr<ID3D12Device>& device,D3D12_PRIMITIVE_TOPOLOGY topology,UINT indexBegin,UINT indexCount);
	~Mesh();

	void WriteContext(MaterialIndex mat);
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	D3D12_PRIMITIVE_TOPOLOGY mTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT mIndexBegin{ 0 };
	UINT mIndexCount{ 0 };

	std::array<MeshContext, static_cast<size_t>(GC_FrameCount)> mMeshContexts{};
	UINT mMemoryIndex{ 0 };
	UINT mInstanceCount{ 0 };
};

