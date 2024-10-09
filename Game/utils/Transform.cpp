#include "pch.h"
#include "Game/utils/Transform.h"





Transform::Transform()
{
}

Transform::~Transform()
{
}

void Transform::Translate(const DirectX::SimpleMath::Vector3& translation)
{
	mPosition += translation;
}

void Transform::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	mPosition = position;
}

void Transform::SetRotate(const DirectX::SimpleMath::Quaternion& rotation)
{
	mRotation = rotation;
	mRotation.Normalize();
}

void Transform::RotateSmoothly(const DirectX::SimpleMath::Quaternion& rotation, float lerpFactor)
{
	mRotation = DirectX::SimpleMath::Quaternion::Slerp(mRotation, rotation, lerpFactor);
	mRotation.Normalize();
}

void Transform::Rotate(float yaw, float pitch, float roll)
{
	auto newRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	mRotation = DirectX::SimpleMath::Quaternion::Concatenate(mRotation, newRotation);
	mRotation.Normalize();
}

void Transform::Rotate(const DirectX::SimpleMath::Quaternion& rotation)
{
	mRotation = DirectX::SimpleMath::Quaternion::Concatenate(mRotation, rotation);
	mRotation.Normalize();
}

void Transform::Scale(const DirectX::SimpleMath::Vector3& scale)
{
	mScale = scale;
}

void Transform::Scale(float x, float y, float z)
{
	mScale = DirectX::SimpleMath::Vector3(x, y, z);
}

void Transform::LookAt(const Transform& target)
{
	DirectX::SimpleMath::Vector3 direction = target.GetPosition() - mPosition;
	direction.Normalize();

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Forward;
	float dot = forward.Dot(direction);

	if (fabs(dot - (-1.0f)) < 0.000001f)
	{
		mRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, DirectX::XM_PI);
		mRotation.Normalize();
	}
	else if (fabs(dot - (1.0f)) < 0.000001f)
	{
		mRotation = DirectX::SimpleMath::Quaternion::Identity;
		mRotation.Normalize();
	}
	else
	{
		float angle = acosf(dot);
		DirectX::SimpleMath::Vector3 axis = forward.Cross(direction);
		axis.Normalize();
		mRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
		mRotation.Normalize();
	}

}

void Transform::LookAt(const DirectX::SimpleMath::Vector3& worldPosition)
{
	DirectX::SimpleMath::Vector3 direction = worldPosition - mPosition;
	direction.Normalize();

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Forward;
	float dot = forward.Dot(direction);

	DirectX::SimpleMath::Quaternion quat{};

	if (fabs(dot - (-1.0f)) < 0.000001f)
	{
		quat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, DirectX::XM_PI);
		quat.Normalize();
	}
	else if (fabs(dot - (1.0f)) < 0.000001f)
	{
		quat = DirectX::SimpleMath::Quaternion::Identity;
		quat.Normalize();
	}
	else
	{
		float angle = acosf(dot);
		DirectX::SimpleMath::Vector3 axis = forward.Cross(direction);
		axis.Normalize();
		quat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
		quat.Normalize();
	}

	mRotation.Concatenate(mRotation, quat);
	mRotation.Normalize();
}

void Transform::SetParent(Transform* parent)
{
}

void Transform::SetOrientedBoundingBox(DirectX::BoundingOrientedBox box)
{
	mOrientedBoundingBox = box;
}

DirectX::SimpleMath::Vector3 Transform::GetPosition() const 
{
	return mPosition;
}

DirectX::SimpleMath::Quaternion Transform::GetRotation() const
{
	return mRotation;
}

DirectX::SimpleMath::Vector3 Transform::GetScale() const 
{
	return mScale;
}

DirectX::SimpleMath::Vector3 Transform::GetForward() const
{
	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, mRotation);
}

DirectX::SimpleMath::Vector3 Transform::GetRight() const
{
	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, mRotation);
}

DirectX::SimpleMath::Vector3 Transform::GetUp() const
{
	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, mRotation);
}

DirectX::BoundingOrientedBox& Transform::GetBB() 
{
	return mWorldBoundingBox;
}

DirectX::SimpleMath::Matrix& Transform::CreateWorldMatrix()
{
	if (mParent != nullptr) {
		mWorldMatrix = DirectX::SimpleMath::Matrix::CreateScale(mScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(mRotation) * DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) * mParent->GetWorldMatrix();
	}
	else {
		mWorldMatrix = DirectX::SimpleMath::Matrix::CreateScale(mScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(mRotation) * DirectX::SimpleMath::Matrix::CreateTranslation(mPosition);
	}


	mOrientedBoundingBox.Transform(mWorldBoundingBox, mWorldMatrix);

	return mWorldMatrix;
}

DirectX::SimpleMath::Matrix Transform::GetWorldMatrix()
{
	return mWorldMatrix;
}
