#pragma once 

// 
//	문제 1 - UV Tranform 을 어디서 계산? ( GPU ? , CPU ? ) 
//  문제 2 - 빌보드 정점을 버퍼로 복사하는 것을 줄이기 위해 변경되지 않으면 이전 복사 결과를 그대로 사용하는 것은? ( Decayed ) 
// 
// 
//	문제 1 과 문제 2를 동시에 해결하는 방법으로는 정점 속성마다 time 을 누적하는 동적 변수를 하나 
// 
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