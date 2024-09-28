#include "pch.h"
#include "Game/Camera.h"


Camera::Camera(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
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



}

Camera::~Camera()
{
	for (auto& context : mCameraContexts) {
		context.mBuffer->Unmap(0, nullptr);
	}
}

DirectX::SimpleMath::Vector3 Camera::GetForward() const
{
	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, mRotation);
}

DirectX::SimpleMath::Vector3 Camera::GetRight() const
{
	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, mRotation);
}

DirectX::SimpleMath::Vector3 Camera::GetUp() const
{
	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, mRotation);
}

DirectX::SimpleMath::Vector3& Camera::GetCameraPosition()
{
	return std::ref(mPosition);
}

Camera::CameraParam& Camera::GetCameraParam()
{
	return std::ref(mCameraParam);
}

void Camera::Rotate(float pitch, float yaw)
{
	DirectX::SimpleMath::Quaternion yawRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Forward, yaw);
	DirectX::SimpleMath::Quaternion pitchRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Right, pitch);

	mRotation = DirectX::SimpleMath::Quaternion::Concatenate(mRotation,yawRotation);
	mRotation = DirectX::SimpleMath::Quaternion::Concatenate(mRotation,pitchRotation);
	mRotation.Normalize();
}

void Camera::SetLookAt(const DirectX::SimpleMath::Vector3& target)
{
	DirectX::SimpleMath::Vector3 direction = mPosition - target;
	direction.Normalize();

	DirectX::SimpleMath::Vector3 forward = Camera::GetForward();

	DirectX::SimpleMath::Vector3 axis = forward.Cross(direction);

	if (axis.LengthSquared() > 0.0001f) {
		axis.Normalize();


		float dot = forward.Dot(direction);
		float angle = std::acosf(dot);

		DirectX::SimpleMath::Quaternion LookRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
		mRotation = DirectX::SimpleMath::Quaternion::Concatenate(mRotation, LookRotation);
		mRotation.Normalize();
	}
}

void Camera::Move(const DirectX::SimpleMath::Vector3& direction, float distance)
{
	mPosition += direction * distance;
}

bool Camera::Intersect(DirectX::BoundingOrientedBox& box)
{
	return mWorldFrustum.Intersects(box);
}

void Camera::UpdateDynamicVariables()
{
	mViewMatrix = DirectX::XMMatrixLookAtLH(mPosition, mPosition - GetForward(), GetUp());
	mViewFrustum.Transform(mWorldFrustum, mViewMatrix.Invert());
}

void Camera::UpdateStaticVariables()
{
	mProjectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(mCameraParam.FOV, mCameraParam.Aspect, mCameraParam.NearZ, mCameraParam.FarZ);
	DirectX::BoundingFrustum::CreateFromMatrix(mViewFrustum, mProjectionMatrix);

}




void Camera::SetVariables(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto& context = mCameraContexts[mMemoryIndex];

	context.mBufferPtr->View = mViewMatrix;
	context.mBufferPtr->Projection = mProjectionMatrix;
	context.mBufferPtr->ViewProjection = mViewMatrix * mProjectionMatrix;

	commandList->SetGraphicsRootConstantBufferView(GRP_CameraConstants, context.mBuffer->GetGPUVirtualAddress());
	mMemoryIndex = (mMemoryIndex + 1) % GC_FrameCount;
}
