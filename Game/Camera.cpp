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
	// 카메라의 방향 벡터 계산
	DirectX::SimpleMath::Vector3 direction = target - mPosition;
	direction.Normalize();

	// 쿼터니언을 사용하여 회전 계산
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Forward;
	float dot = forward.Dot(direction);

	if (fabs(dot - (-1.0f)) < 0.000001f) {
		// 180도 회전
		mRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, DirectX::XM_PI);
		mRotation.Normalize();
	}
	else if (fabs(dot - (1.0f)) < 0.000001f) {
		// 0도 회전
		mRotation = DirectX::SimpleMath::Quaternion::Identity;
		mRotation.Normalize();
	}
	else {
		float angle = acosf(dot);
		DirectX::SimpleMath::Vector3 axis = forward.Cross(direction);
		axis.Normalize();
		mRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
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
	// 뷰 행렬 생성
	mViewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(mPosition, mPosition + GetForward() , GetUp() );
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

	Camera::UpdateDynamicVariables();

	context.mBufferPtr->View = mViewMatrix.Transpose();
	context.mBufferPtr->Projection = mProjectionMatrix.Transpose();
	context.mBufferPtr->ViewProjection = (mViewMatrix * mProjectionMatrix).Transpose();

	commandList->SetGraphicsRootConstantBufferView(GRP_CameraConstants, context.mBuffer->GetGPUVirtualAddress());
	mMemoryIndex = (mMemoryIndex + 1) % GC_FrameCount;
}
