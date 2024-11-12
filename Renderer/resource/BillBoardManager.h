#pragma once 

// 
//	문제 1 - UV Tranform 을 어디서 계산? ( GPU ? , CPU ? ) 
//  문제 2 - 빌보드 정점을 버퍼로 복사하는 것을 줄이기 위해 변경되지 않으면 이전 복사 결과를 그대로 사용하는 것은? ( Decayed ) 
// 
//	그게 아니라면, 게임 시작부터 지금까지의 시간을 32bit 상수로 업로드하고, 셰이더에서 이를 활용하여 현재 스프라이트 위치를 결정하는 것.
//



class BillBoardManager {
	constexpr static size_t BillboardCount = 1000;

	struct VertexBuffer {
		ComPtr<ID3D12Resource> buffer{};
		BillBoardVertex* bufferptr{};
	};
public:
	BillBoardManager(ComPtr<ID3D12Device> device,ComPtr<ID3D12CommandList> loadCommandList);
	~BillBoardManager();
public:

private: 
	std::unique_ptr<IGraphicsShader> mShader{ nullptr };

	std::vector<BillBoardVertex> mVertices{};
	ComPtr<ID3D12Resource> mVertexBuffer{ nullptr };
	bool mDecayed{ true };

	std::array<VertexBuffer,static_cast<size_t>(GC_FrameCount)> mVertexBuffers{};
};