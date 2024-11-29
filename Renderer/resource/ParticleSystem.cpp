#include "pch.h"
#include "resource/ParticleSystem.h"

ParticleSystem::ParticleSystem(ComPtr<ID3D12Device>& device)
{
	mSOtoGSBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mParticleSOBuffer.Get(), D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	mGStoSOBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mParticleSOBuffer.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_STREAM_OUT);

	CD3DX12_HEAP_PROPERTIES	uploadHeap { D3D12_HEAP_TYPE_UPLOAD };
	CD3DX12_HEAP_PROPERTIES readbackHeap{ D3D12_HEAP_TYPE_READBACK };
		
	CD3DX12_RESOURCE_DESC counterBufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64)) };
	CD3DX12_RESOURCE_DESC netParticleBufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(MAX_PARTICLE_COUNT_UPLOAD_ONCE * sizeof(ParticleVertex)) };

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
			std::addressof(netParticleBufferDesc),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mNewParticleUploadBuffer.GetAddressOf())
		)
	);

	CreateParticleDefaultBuffer(device, mVertexBuffer.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	CreateParticleDefaultBuffer(device, mParticleSOBuffer.GetAddressOf(), D3D12_RESOURCE_STATE_STREAM_OUT);
	CreateParticleDefaultBuffer(device, mStreamCounterDefaultBuffer.GetAddressOf(), D3D12_RESOURCE_STATE_STREAM_OUT);
}

void ParticleSystem::CreateParticleDefaultBuffer(ComPtr<ID3D12Device>& device, ID3D12Resource** buffer, D3D12_RESOURCE_STATES initialState)
{
	UINT64 size = MAX_PARTICLE_COUNT * sizeof(ParticleVertex);
	CD3DX12_HEAP_PROPERTIES defaultHeap{ D3D12_HEAP_TYPE_DEFAULT };
	CD3DX12_RESOURCE_DESC particleBufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(MAX_PARTICLE_COUNT * sizeof(ParticleVertex)) };

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
}

void ParticleSystem::RenderGS(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
}


void ParticleSystem::UpdateParticleVertices()
{
}

void ParticleSystem::ReadBackFromBuffer(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	
}

void ParticleSystem::SwapBufferPointer(ComPtr<ID3D12Resource>& resourceA, ComPtr<ID3D12Resource>& resourceB)
{
	std::swap(resourceA, resourceB);
}
