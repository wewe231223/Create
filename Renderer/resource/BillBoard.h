#pragma once 
class BillBoard {
	constexpr static size_t BillboardCount = 1000;
	struct VertexBuffer {
		ComPtr<ID3D12Resource> buffer{};
		BillBoardVertex* bufferptr{};
	};
public:
	BillBoard(ComPtr<ID3D12Device> device,ComPtr<ID3D12CommandList> loadCommandList);
	~BillBoard();
public:
	// 기본 빌보드 생성 함수 
	void MakeBillBoard(const BillBoardVertex& vertex);
	// 빌보드 여러개 생성 
	void MakeBillBoard(const std::vector<BillBoardVertex>& vertices);
	// 움직이지 않는 빌보드 생성 ( NonSpritable ) 
	void MakeBillBoard(const DirectX::XMFLOAT3& position,UINT halfWidth, UINT height, TextureIndex image);

	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList, D3D12_GPU_DESCRIPTOR_HANDLE texHeap, D3D12_GPU_VIRTUAL_ADDRESS cameraBuffer);
private: 
	std::unique_ptr<IGraphicsShader> mShader{ nullptr };

	std::vector<BillBoardVertex> mVertices{};
	bool mDecayed{ true };

	std::array<VertexBuffer,static_cast<size_t>(GC_FrameCount)> mVertexBuffers{};
	UINT mCurrentBuffer{ 0 };
};