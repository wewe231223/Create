#include "pch.h"
#include "Game/scene/Camera.h"



SkyBox::SkyBox(std::shared_ptr<I3DRenderable> model, const std::vector<MaterialIndex> images)
	: mModel(model), mImages(images)
{
}

SkyBox::~SkyBox()
{
}

void SkyBox::Render(const DirectX::SimpleMath::Matrix& translation)
{
	mContext.World = translation.Transpose();
	mModel->WriteContext(&mContext, mImages);
}


Camera::Camera(ComPtr<ID3D12Device>& device, std::shared_ptr<Window> window)
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

	for (auto& cameraBuffer : mCameraBuffers) {
		CheckHR(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(cameraBuffer.buffer.GetAddressOf())
		));

		cameraBuffer.buffer->Map(0, nullptr, reinterpret_cast<void**>(std::addressof(cameraBuffer.bufferptr)));
	}

	mParam.FOV = DirectX::XMConvertToRadians(60.f);
	mParam.aspectRatio = window->GetWindowWidth<float>() / window->GetWindowHeight<float>();
	mParam.nearPlane = 0.1f;
	mParam.farPlane = 1000.f;

	Camera::MakeProjectionMatrix();
}

Camera::~Camera()
{
	for (auto& cameraBuffer : mCameraBuffers) {
		cameraBuffer.buffer->Unmap(0, nullptr);
	}

}

void Camera::MakeProjectionMatrix()
{
	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mParam.FOV, mParam.aspectRatio, mParam.nearPlane, mParam.farPlane);
	DirectX::BoundingFrustum::CreateFromMatrix(mViewFrustum, mProjectionMatrix);
}

void Camera::SetCameraSkyBox(std::shared_ptr<SkyBox>&& skybox)
{
	mSkyBox = std::move(skybox);
}

Transform& Camera::GetTransform()
{
	return mTransform;
}

bool Camera::IsInFrustum(DirectX::BoundingOrientedBox& box)
{
	return mWorldFrustum.Intersects(box);
}

void Camera::Update()
{
	mViewMatrix = DirectX::XMMatrixLookAtLH(mTransform.GetPosition(), mTransform.GetPosition() + mTransform.GetForward(),DirectX::SimpleMath::Vector3::Up);
	mViewFrustum.Transform(mWorldFrustum,mViewMatrix.Invert());
}

void Camera::RenderSkyBox()
{
	if (mSkyBox) {
		mSkyBox->Render(DirectX::SimpleMath::Matrix::CreateTranslation(mTransform.GetPosition()));
	}

}

void Camera::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{

	auto& CameraBuffer = mCameraBuffers[mMemoryIndex];

	CameraBuffer.bufferptr->View = mViewMatrix.Transpose();
	CameraBuffer.bufferptr->Projection = mProjectionMatrix.Transpose();
	CameraBuffer.bufferptr->ViewProjection = (mViewMatrix * mProjectionMatrix).Transpose();

	commandList->SetGraphicsRootConstantBufferView(GRP_CameraConstants, CameraBuffer.buffer->GetGPUVirtualAddress());

	mMemoryIndex = (mMemoryIndex + 1) % GC_FrameCount;
}
