#pragma once 

class BillBoardManager {
	constexpr static size_t BillboardCount = 1000;

	struct VertexBuffer {
		ComPtr<ID3D12Resource> buffer{};
		BillBoardVertex* bufferptr{};
	};
public:
	BillBoardManager(ComPtr<ID3D12Device> device,ComPtr<ID3D12CommandList> loadCommandList);
	~BillBoardManager();

private:
	ComPtr<ID3D12Resource> mVertexBuffer{ nullptr };

	std::vector<BillBoardVertex> mVertices{};
};