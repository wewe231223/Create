#include "pch.h"
#include "ShadowCaster.h"

DirectionalShadowCaster::DirectionalShadowCaster(ComPtr<ID3D12Device>& device, UINT64 shadowMapWidth, UINT shadowMapHeight, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction)
	: mPosition(position), mDirection(direction), mShadowMapWidth(static_cast<UINT>(shadowMapWidth)), mShadowMapHeight(shadowMapHeight)
{
    D3D12_RESOURCE_DESC desc{ CD3DX12_RESOURCE_DESC::Tex2D(
                                                            DXGI_FORMAT_D32_FLOAT,
                                                            shadowMapWidth,
                                                            shadowMapHeight,
                                                            1,
                                                            1,
                                                            1,
                                                            0,
                                                            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) 
    };


    D3D12_HEAP_PROPERTIES heapProperties{ CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT) };
    CD3DX12_CLEAR_VALUE clearValue{ DXGI_FORMAT_D32_FLOAT, 1.0f, 0 };
    
    
	CheckHR(device->CreateCommittedResource(
        std::addressof(heapProperties), 
        D3D12_HEAP_FLAG_NONE, 
        &desc, 
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
        std::addressof(clearValue), 
        IID_PPV_ARGS(mShadowMap.GetAddressOf()))
    );

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	CheckHR(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mShadowMapHeap.GetAddressOf())));

	device->CreateDepthStencilView(mShadowMap.Get(), nullptr, mShadowMapHeap->GetCPUDescriptorHandleForHeapStart());
    {
        D3D12_RESOURCE_DESC desc{};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Alignment = 0;
        desc.Width = static_cast<UINT64>(GetCBVSize<CameraContext>());
        desc.Height = 1;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        CD3DX12_HEAP_PROPERTIES heapProperties{ D3D12_HEAP_TYPE_UPLOAD };

		CheckHR(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mShadowCameraBuffer.GetAddressOf()))
		);

        CheckHR(mShadowCameraBuffer->Map(0, nullptr, reinterpret_cast<void**>(std::addressof(mShadowCameraBufferPtr))));
    }
}

DirectionalShadowCaster::~DirectionalShadowCaster()
{
    mShadowCameraBuffer->Unmap(0, nullptr);
}

void DirectionalShadowCaster::Clear(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	D3D12_RESOURCE_BARRIER resourceBarrier{ CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE) };
	commandList->ResourceBarrier(1, &resourceBarrier);
	commandList->ClearDepthStencilView(mShadowMapHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectionalShadowCaster::SetShadowMap(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle{ mShadowMapHeap->GetCPUDescriptorHandleForHeapStart() };
	commandList->OMSetRenderTargets(0, nullptr, false, std::addressof(handle));
}

ComPtr<ID3D12Resource>& DirectionalShadowCaster::GetShadowMap()
{
    return mShadowMap;
}

void DirectionalShadowCaster::Sync(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    D3D12_RESOURCE_BARRIER resourceBarrier{ CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap.Get(),D3D12_RESOURCE_STATE_DEPTH_WRITE,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) };
	commandList->ResourceBarrier(1, &resourceBarrier);
}


void DirectionalShadowCaster::UpdateCameraContext()
{
    mViewMatrix = DirectX::XMMatrixLookAtLH(mPosition, mDirection, DirectX::SimpleMath::Vector3::Up);
    mProjectionMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(mShadowMapWidth), static_cast<float>(mShadowMapHeight), 1.f, 500.f);

    mCameraContext.CameraPosition = mPosition;
    mCameraContext.View = mViewMatrix.Transpose();
    mCameraContext.Projection = mProjectionMatrix.Transpose();
    mCameraContext.ViewProjection = (mViewMatrix * mProjectionMatrix).Transpose();
}

void DirectionalShadowCaster::SetCameraContext(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    ::memcpy(mShadowCameraBufferPtr, std::addressof(mCameraContext), sizeof(CameraContext));
    commandList->SetGraphicsRootConstantBufferView(GRP_CameraConstants, mShadowCameraBuffer->GetGPUVirtualAddress());
}

void DirectionalShadowCaster::SetShadowTransform(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    commandList->SetGraphicsRootConstantBufferView(GRP_ShadowTransform, mShadowCameraBuffer->GetGPUVirtualAddress());
}

void DirectionalShadowCaster::SetFocus(DirectX::SimpleMath::Vector3 focus)
{
    mDirection = focus;
}

void DirectionalShadowCaster::SetPosition(DirectX::SimpleMath::Vector3 position)
{
    mPosition = position;
}


