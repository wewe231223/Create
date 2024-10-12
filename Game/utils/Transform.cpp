#include "pch.h"
#include "Game/utils/Transform.h"


// TODO : 축 둘을 받아서 a -> b 시키는 회전을 계산하는 함수를 만들자., 


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

void Transform::SetLocalTransform(const DirectX::SimpleMath::Matrix& localTransform)
{
	mLocalTransform = localTransform;

}

void Transform::ResetRotation()
{
	mRotation = DirectX::SimpleMath::Quaternion::Identity;
}

void Transform::RotateSmoothly(const DirectX::SimpleMath::Quaternion& rotation, float lerpFactor)
{
	auto newRotation = DirectX::SimpleMath::Quaternion::Slerp(mRotation, rotation, lerpFactor);
	newRotation.Normalize();
	mRotation = mRotation.Concatenate(mRotation, rotation);
	mRotation.Normalize();
}

void Transform::Rotate(float yaw, float pitch, float roll)
{
	auto newRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	newRotation.Normalize();
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

	DirectX::SimpleMath::Quaternion rotation = DirectX::SimpleMath::Quaternion::Identity;

	if (fabs(dot - (-1.0f)) < 0.0001f)
	{
		rotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, DirectX::XM_PI);
		rotation.Normalize();
	}
	else if (fabs(dot - (1.0f)) < 0.0001f)
	{
		rotation = DirectX::SimpleMath::Quaternion::Identity;
		rotation.Normalize();
	}
	else
	{
		float angle = acosf(dot);
		DirectX::SimpleMath::Vector3 axis = forward.Cross(direction);
		axis.Normalize();
		rotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, angle);
		rotation.Normalize();
	}

	mRotation = mRotation.Concatenate(mRotation, rotation);
	//mRotation.Normalize();
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

	mRotation = mRotation.Concatenate(mRotation, quat);
	mRotation.Normalize();
}

void Transform::SetParent(Transform* parent)
{
	mParent = parent;
	parent->SetChild(this);
}

void Transform::SetChild(Transform* child)
{
	mChildren.push_back(child);
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
		mWorldMatrix =  DirectX::SimpleMath::Matrix::CreateScale(mScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(mRotation)  *  ( mLocalTransform * mParent->GetWorldMatrix() );
	}
	else {
		mWorldMatrix =   DirectX::SimpleMath::Matrix::CreateScale(mScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(mRotation) * DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) * mLocalTransform;
	}

	mOrientedBoundingBox.Transform(mWorldBoundingBox, mWorldMatrix);
	mRotation = DirectX::SimpleMath::Quaternion::Identity;

	return mWorldMatrix;
}

DirectX::SimpleMath::Matrix Transform::GetWorldMatrix()
{
	return mWorldMatrix;
}
