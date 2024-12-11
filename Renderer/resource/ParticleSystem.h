#pragma once 

//================================================================================================================================================
//																																				//	
//													Stream Output 을 활용하는 ParticleSystem														//
//																																				//
// 1. SO 단계를 사용하는 파티클 시스템은 2번의 draw 를 거쳐 그려지게 된다.																				//
//																																				//
// 1-1. 첫 번째 단계 ( SO Pass )																													//
// 첫 번재 단계는 Render Target 에 직접 그려지지 않는 단계로, 파티클들을 시뮬레이션 하여, LifeTime 을 업데이트 하고,											//
// 만약 다른 파티클을 방출하는 파티클 타입의 경우 해당 정점을 추가하여, 동적으로 셰이더 단계에서 파티클 점을 추가 할 수 있도록 하여야 한다.							//
//																																				//
// 1-2. 두 번째 단계 ( GS Pass )																													//
// 두 번째 단계는 Render Target 에 그려지는 단계로, 첫 번째 단계에 갱신된 파티클들을 기하 셰이더를 사용하여 빌보드로 그린다.									//
//																																				//
//																																				//
// 2. ParticleSystem Pipeline																													//
// - SO 단계에 사용하는 버퍼는 항상 ResourceBarrier 으로 동기화하고, 모든 프레임 리소스가 공유한다.														//
// - SO 단계에 사용하는 버퍼는 업로드 힙으로 구성하며, ReadBack 힙으로 읽어온 현재 파티클 개수 뒤에 각 프레임 리소스가 받아온 새로운 파티클을 복사한다.				//
// - GS 단계에는 SO 단계를 거친 SO 버퍼를 정점 입력으로 사용한다.																						//
// - 명령어의 순서는 항상 보장되므로, SO 단계의 동기화가 역전되는 현상은 고려할 필요 없다.																	//
//																																				//
// 3. ParticleSystem Flow																														//
// - 파티클 시스템은 다음과 같은 단계를 따라 작동한다.																									//
//																																				//
// CounterBuffer Clear -> SO set -> SO Render -> Read From Counter Buffer -> GS Render -> ...													// 
//																																				//
// mVertexBuffer 는 모든 pass 의 정점 입력으로 설정되고, mParticleSOBuffer 는 SO 단계에서 사용되는 스트림 출력 버퍼로 사용된다.								//
// 이를 위해 SO Pass 가 끝난 뒤에 mVertexBuffer 와 mParticleSOBuffer 의 포인터를 swap 하여 SO 버퍼에 담긴 내용을 다음 단계 정점 입력으로 사용된다 			//
// 그 다음 SO Pass 에서는 이전 GS Pass 에 사용된 정점 입력을 그대로 다시 정점 입력으로 사용한다															//			
// 																																				// 
//================================================================================================================================================


class ParticleSystem {
public:
	static constexpr size_t MAX_PARTICLE_COUNT = 9'0000;
	static constexpr size_t MAX_PARTICLE_COUNT_UPLOAD_ONCE = 300;
public:
	ParticleSystem(ComPtr<ID3D12Device>& device);
	~ParticleSystem();
public:
	void RenderSO(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void RenderGS(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	void SyncBuffer(ComPtr<ID3D12GraphicsCommandList>& commandlist, ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES prev, D3D12_RESOURCE_STATES target);
	void CreateDefaultBuffer(ComPtr<ID3D12Device>& device, ID3D12Resource** buffer, D3D12_RESOURCE_STATES initialState, size_t size = MAX_PARTICLE_COUNT * sizeof(ParticleVertex));
	void UpdateParticleVertices(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void ReadBackFromBuffer(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void SwapBufferPointer(ComPtr<ID3D12Resource>& resourceA, ComPtr<ID3D12Resource>& resourceB);
private:
	// 첫 번째 Stream Output 단계를 위한 셰이더 ( PSO ) 
	std::unique_ptr<IGraphicsShader>	mSOPassShader{ nullptr };
	// 두 번째 Render Target 에 그리는 ( Rasterizer GS ) 단계를 위한 셰이더 ( PSO ) 
	std::unique_ptr<IGraphicsShader>	mGSPassShader{ nullptr };
	// 정점 버퍼 
	ComPtr<ID3D12Resource>				mVertexBuffer{ nullptr };
	// 스트림 출력 버퍼  
	ComPtr<ID3D12Resource>				mParticleSOBuffer{ nullptr };

	// SO 단계에 set 하기 전 카운터를 초기화 시키는 버퍼 
	ComPtr<ID3D12Resource>				mStreamCounterUploadBuffer{ nullptr };
	UINT64*								mStreamCounterUploadPtr{ nullptr };

	// SO 단계에 set 하는데 사용되는 스트림 출력 카운터 버퍼 
	ComPtr<ID3D12Resource>				mStreamCounterDefaultBuffer{ nullptr };
	// SO 단계를 거쳐 생성된 점들의 개수를 읽어오는 ReadBack 버퍼 
	UINT32								mParticleCount{ 0 };
	ComPtr<ID3D12Resource>				mStreamCounterReadBackBuffer{ nullptr };
	// 새롭게 추가될 파티클들을 담는 버퍼 
	std::array<ParticleVertex, MAX_PARTICLE_COUNT_UPLOAD_ONCE>				mNewParticles{};
	std::array<ParticleVertex, MAX_PARTICLE_COUNT_UPLOAD_ONCE>::iterator	mNewParticlePos{};
	// 새롭게 추가될 파티클들을 업로드 하는 버퍼 
	ComPtr<ID3D12Resource>				mNewParticleUploadBuffer{ nullptr };

	D3D12_STREAM_OUTPUT_BUFFER_VIEW		mSOBufferView{};
	D3D12_VERTEX_BUFFER_VIEW			mVertexBufferView{};	
};