#include "pch.h"
#include "resource/ParticleSystem.h"

ParticleSystem::ParticleSystem(ComPtr<ID3D12Device>& device)
{

	CD3DX12_HEAP_PROPERTIES	uploadHeap { D3D12_HEAP_TYPE_UPLOAD };
	CD3DX12_HEAP_PROPERTIES readbackHeap{ D3D12_HEAP_TYPE_READBACK };
		
	CD3DX12_RESOURCE_DESC counterBufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64)) };
	CD3DX12_RESOURCE_DESC newParticleBufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(MAX_PARTICLE_COUNT_UPLOAD_ONCE * sizeof(ParticleVertex)) };

	CheckHR(
		device->CreateCommittedResource(
			std::addressof(uploadHeap),
			D3D12_HEAP_FLAG_NONE,
			std::addressof(counterBufferDesc),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mStreamCounterUploadBuffer.GetAddressOf())
		)
	);

	CheckHR(
		device->CreateCommittedResource(
			std::addressof(readbackHeap),
			D3D12_HEAP_FLAG_NONE,
			std::addressof(counterBufferDesc),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(mStreamCounterReadBackBuffer.GetAddressOf())
		)
	);

	CheckHR(
		device->CreateCommittedResource(
			std::addressof(uploadHeap),
			D3D12_HEAP_FLAG_NONE,
			std::addressof(newParticleBufferDesc),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mNewParticleUploadBuffer.GetAddressOf())
		)
	);

	CreateDefaultBuffer(device, mVertexBuffer.GetAddressOf(),					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	CreateDefaultBuffer(device, mParticleSOBuffer.GetAddressOf(),				D3D12_RESOURCE_STATE_STREAM_OUT);
	CreateDefaultBuffer(device, mStreamCounterDefaultBuffer.GetAddressOf(),		D3D12_RESOURCE_STATE_STREAM_OUT, sizeof(UINT64));
	
	mStreamCounterUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(std::addressof(mStreamCounterUploadPtr)));
}

void ParticleSystem::SyncBuffer(ComPtr<ID3D12GraphicsCommandList>& commandlist, ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES prev, D3D12_RESOURCE_STATES target)
{
	D3D12_RESOURCE_BARRIER barrier{ CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), prev, target)};
	commandlist->ResourceBarrier(1, std::addressof(barrier));
}

void ParticleSystem::CreateDefaultBuffer(ComPtr<ID3D12Device>& device, ID3D12Resource** buffer, D3D12_RESOURCE_STATES initialState, size_t size)
{
	CD3DX12_HEAP_PROPERTIES defaultHeap{ D3D12_HEAP_TYPE_DEFAULT };
	CD3DX12_RESOURCE_DESC particleBufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(size) };

	CheckHR(
		device->CreateCommittedResource(
			std::addressof(defaultHeap),
			D3D12_HEAP_FLAG_NONE,
			std::addressof(particleBufferDesc),
			initialState,
			nullptr,
			IID_PPV_ARGS(buffer)
		)
	);
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::RenderSO(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	// SO 렌더 단계 
	// 1. 만약 새롭게 추가된 정점이 있다면 이것을 정점 버퍼에 복사한다. 
	// 2. 스트림 출력 카운터에 0을 복사한다. 
	// 3. SO 버퍼를 set 하고 그린다. 
	
	// 새로운 파티클이 있다면, SO 패스를 거칠 버퍼에 복사한다. 
	if (mNewParticlePos != mNewParticles.begin()) {
		ParticleSystem::UpdateParticleVertices(commandList);
	}

	// 스트림 출력 카운터 디폴트 버퍼에 업로드 버퍼를 통해 0을 넣는다. 
	*mStreamCounterUploadPtr = 0;
	ParticleSystem::SyncBuffer(commandList, mStreamCounterDefaultBuffer, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_COPY_DEST);
	commandList->CopyResource(mStreamCounterDefaultBuffer.Get(), mStreamCounterUploadBuffer.Get());
	ParticleSystem::SyncBuffer(commandList, mStreamCounterDefaultBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_STREAM_OUT);

	// 셰이더 바인딩 
	mSOPassShader->SetShader(commandList);

	// 리소스 Set 
	mSOBufferView.BufferLocation = mParticleSOBuffer->GetGPUVirtualAddress();
	mSOBufferView.SizeInBytes = sizeof(ParticleVertex) * ParticleSystem::MAX_PARTICLE_COUNT; 
	mSOBufferView.BufferFilledSizeLocation = mStreamCounterDefaultBuffer->GetGPUVirtualAddress();

	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.SizeInBytes = sizeof(ParticleVertex) * ParticleSystem::MAX_PARTICLE_COUNT;
	mVertexBufferView.StrideInBytes = sizeof(ParticleVertex);

	commandList->SOSetTargets(0, 1, std::addressof(mSOBufferView));
	commandList->IASetVertexBuffers(0, 1, std::addressof(mVertexBufferView));

	// Draw Call 
	commandList->DrawInstanced(mParticleCount, 1, 0, 0);
}

void ParticleSystem::RenderGS(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	// 렌더 타겟 출력 단계 
	// 1. 이전 pass 의 버퍼를 서로 swap 한다. ( 그렇다면 이전 pass 의 결과는 이번 pass의 정점 버퍼가 되고, SO 버퍼는 이전 pass 의 정점 입력 버퍼가 된다 ) 
	// 이렇게 두 버퍼를 swap 해야 하는 이유는 SO Pass 에 Stream Output Target 으로 사용된 버퍼는 이번 Pass 의 정점 버퍼로 사용되어야 할 뿐만 아니라, 다음 프레임의 SO Pass 의 정점 버퍼로도 사용되어야 하기 때문이다. 
	// 2. SO 단계에 빈 버퍼를 set 한다.
	// 3. 정점 버퍼를 set 하고 그린다. 
	// 4. ReadBack 힙으로부터 스트림 출력 카운터를 읽어온다. 

	ParticleSystem::SyncBuffer(commandList, mVertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_STREAM_OUT);
	ParticleSystem::SyncBuffer(commandList, mParticleSOBuffer, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	ParticleSystem::SwapBufferPointer(mVertexBuffer, mParticleSOBuffer);

	mGSPassShader->SetShader(commandList);

	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.SizeInBytes = mParticleCount * sizeof(mParticleCount);
	mVertexBufferView.StrideInBytes = sizeof(ParticleVertex);
	// 이전 Target 이 set 된 상태로 존재하므로 set 을 해제 
	commandList->SOSetTargets(0, 1, nullptr);
	commandList->IASetVertexBuffers(0, 1, std::addressof(mVertexBufferView));

	// Draw Call 
	commandList->DrawInstanced(mParticleCount, 1, 0, 0);

	// ReadBack 힙에 스트림 출력 카운터 결과를 복사한다. 
	ParticleSystem::SyncBuffer(commandList, mStreamCounterDefaultBuffer, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->CopyResource(mStreamCounterReadBackBuffer.Get(), mStreamCounterDefaultBuffer.Get());
	ParticleSystem::SyncBuffer(commandList, mStreamCounterDefaultBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_STREAM_OUT);

}

void ParticleSystem::UpdateParticleVertices(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto newParticleCount = std::distance(mNewParticles.begin(), mNewParticlePos);
	// 현재 파티클 개수 + 새롭게 추가된 파티클 개수 >= 최대 파티클 개수 라면 추가하지 않는다  
	if (mParticleCount + newParticleCount >= ParticleSystem::MAX_PARTICLE_COUNT) return;

	// 새로운 파티클들을 업로드 힙에 복사 
	void* map = nullptr;
	mNewParticleUploadBuffer->Map(0, nullptr, std::addressof(map));
	::memcpy(map, mNewParticles.data(), newParticleCount);
	mNewParticleUploadBuffer->Unmap(0, nullptr);

	// 정점 버퍼를 복사할 상태가 되도록 전이 
	ParticleSystem::SyncBuffer(commandList, mVertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);

	// 업로드 힙에 복사한 파티클 데이터들을 디폴트 힙에 복사 
	commandList->CopyBufferRegion(mVertexBuffer.Get(), mParticleCount, mNewParticleUploadBuffer.Get(), 0, static_cast<UINT64>(newParticleCount) * sizeof(ParticleVertex));
	
	// 다시 원상태로 복구 
	ParticleSystem::SyncBuffer(commandList, mVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	
	// 파티클 개수 업데이트 
	mParticleCount += static_cast<UINT32>(newParticleCount);
	//입력 피봇을 맨 앞으로 초기화 
	mNewParticlePos = mNewParticles.begin();
}

void ParticleSystem::ReadBackFromBuffer(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	UINT64* streamCount = nullptr;
 	CheckHR(mStreamCounterReadBackBuffer->Map(0, nullptr, reinterpret_cast<void**>(std::addressof(streamCount))));
	mParticleCount = static_cast<UINT32>((*streamCount) / sizeof(ParticleVertex));
	mStreamCounterReadBackBuffer->Unmap(0, nullptr);
}

void ParticleSystem::SwapBufferPointer(ComPtr<ID3D12Resource>& resourceA, ComPtr<ID3D12Resource>& resourceB)
{
	std::swap(resourceA, resourceB);
}
