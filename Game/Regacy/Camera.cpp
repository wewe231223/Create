#include "pch.h"
#include "Game/gameobjects/Camera.h"


Camera::Camera(ComPtr<ID3D12Device>& device, std::shared_ptr<Window> window)
	: mWindow(window)
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

	for (auto& context : mCameraContexts) {
		CheckHR(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(context.mBuffer.GetAddressOf())
		));

		context.mBuffer->Map(0, nullptr, reinterpret_cast<void**>(std::addressof(context.mBufferPtr)));
	}


	mCameraParam.FOV = DirectX::XMConvertToRadians(60.f);
	mCameraParam.Aspect = mWindow->GetWindowWidth<float>() / mWindow->GetWindowHeight<float>();
	mCameraParam.NearZ = 0.1f;
	mCameraParam.FarZ = 1000.f;

	Camera::UpdateProjectionMatrix();
}

Camera::~Camera()
{
	for (auto& context : mCameraContexts) {
		context.mBuffer->Unmap(0, nullptr);
	}
}

Camera::CameraParam& Camera::GetCameraParam()
{
	return std::ref(mCameraParam);
}

bool Camera::Intersect(DirectX::BoundingOrientedBox& box)
{
	return mWorldFrustum.Intersects(box);
}

void Camera::Update(GameObject* object)
{
	auto transform = object->GetComponent<Transform>();
	
	mViewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(
		transform->GetPosition(),
		transform->GetPosition() + transform->GetFoward(),
		transform->GetUp()
	);

	mViewFrustum.Transform(mWorldFrustum, mViewMatrix.Invert());
}

void Camera::Render(GameObject* object, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto& context = mCameraContexts[mMemoryIndex];
	context.mBufferPtr->View = mViewMatrix.Transpose();
	context.mBufferPtr->Projection = mProjectionMatrix.Transpose();
	context.mBufferPtr->ViewProjection = (mViewMatrix * mProjectionMatrix).Transpose();

	commandList->SetGraphicsRootConstantBufferView(GRP_CameraConstants, context.mBuffer->GetGPUVirtualAddress());
}

void Camera::UpdateProjectionMatrix()
{
	mProjectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		mCameraParam.FOV,
		mCameraParam.Aspect,
		mCameraParam.NearZ,
		mCameraParam.FarZ
	);

	DirectX::BoundingFrustum::CreateFromMatrix(mViewFrustum, mProjectionMatrix);
}

CameraObject::CameraObject(ComPtr<ID3D12Device>& device, std::shared_ptr<Window> window)
{
	GameObject::AddComponent<Transform>();
	GameObject::AddComponent<Camera>(device, window);
}

CameraObject::~CameraObject()
{
}
