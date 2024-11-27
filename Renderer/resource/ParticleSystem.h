#pragma once 

//====================================================================================================================================
//            
//                                    Stream Output 을 활용하는 ParticleSystem
// 
// 1. SO 단계를 사용하는 파티클 시스템은 2번의 draw 를 거쳐 그려지게 된다. 
// 
// 1-1. 첫 번째 단계 ( SO Pass ) 
// 첫 번재 단계는 Render Target 에 직접 그려지지 않는 단계로, 파티클들을 시뮬레이션 하여, LifeTime 을 업데이트 하고, 
// 만약 다른 파티클을 방출하는 파티클 타입의 경우 해당 정점을 추가하여, 동적으로 셰이더 단계에서 파티클 점을 추가 할 수 있도록 하여야 한다. 
// 
// 1-2. 두 번째 단계 ( GS Pass )
// 두 번째 단계는 Render Target 에 그려지는 단계로, 첫 번째 단계에 갱신된 파티클들을 기하 셰이더를 사용하여 빌보드로 그린다. 
// 
// 
// 2. ParticleSystem Pipeline 
// - SO 
// 
// -----------------------
// |	
// 
// 
//====================================================================================================================================

 
class ParticleSystem {
	struct Buffer {
		ComPtr<ID3D12Resource> mBuffer{ nullptr };
		ParticleVertex* mBufferPtr{ nullptr };
	};
public:
	ParticleSystem();
	~ParticleSystem();
public:
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	ComPtr<ID3D12Resource> mParticleSOBuffer{ nullptr };

	std::array<Buffer, static_cast<size_t>(EGlobalConstants::GC_FrameCount)> mParticleBuffer{};
};