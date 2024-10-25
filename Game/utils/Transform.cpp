#include "pch.h"
#include "Game/utils/Transform.h"

Transform::Transform()
{
}

Transform::~Transform()
{
}

Transform::Transform(const Transform& other)
{
	mLocalTransform = other.mLocalTransform;
	mOrientedBoundingBox = other.mOrientedBoundingBox;
	mParent = other.mParent;
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
	mRotation = mRotation.Concatenate(mRotation, newRotation);
	mRotation.Normalize();
}	

void Transform::Rotate(float yaw, float pitch, float roll)
{
	mRotation = DirectX::SimpleMath::Quaternion::Concatenate(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll), mRotation);
	mRotation.Normalize();
}

void Transform::Rotate(const DirectX::SimpleMath::Quaternion& rotation)
{
	mRotation = DirectX::SimpleMath::Quaternion::Concatenate(rotation, mRotation);
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
	Transform::LookAt(target.GetPosition());
}

void Transform::LookAt(const DirectX::SimpleMath::Vector3& worldPosition)
{
	DirectX::SimpleMath::Vector3 direction = worldPosition - mPosition;
	direction.Normalize();

	auto look = DirectX::SimpleMath::Quaternion::FromToRotation(Transform::GetForward(), direction);
	look.Normalize();
	mRotation = DirectX::SimpleMath::Quaternion::Concatenate(look, mRotation);

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
	if (mParent != nullptr) {
		auto parentPos = mParent->GetPosition();
		auto parentScale = mParent->GetScale();
		auto localPos = DirectX::SimpleMath::Vector3{ mLocalTransform._41 * parentScale.x ,mLocalTransform._42 * parentScale.y,mLocalTransform._43 * parentScale.z };
		
		return mPosition + parentPos + localPos;
	}
	return mPosition;
}

DirectX::SimpleMath::Quaternion Transform::GetRotation() const
{
	if (mParent != nullptr)
		return DirectX::SimpleMath::Quaternion::Concatenate(mParent->GetRotation(), mRotation);
	return mRotation;
}

DirectX::SimpleMath::Vector3 Transform::GetScale() const 
{
	return mScale;
}

DirectX::SimpleMath::Vector3 Transform::GetForward() const 
{
	if (mParent != nullptr) {
		auto quat = mParent->GetRotation();
		quat = DirectX::SimpleMath::Quaternion::Concatenate(quat, mRotation);
		return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, quat);
	}

	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, mRotation);
}

DirectX::SimpleMath::Vector3 Transform::GetRight() const
{
	if (mParent != nullptr) {
		auto quat = mParent->GetRotation();
		quat = DirectX::SimpleMath::Quaternion::Concatenate(quat, mRotation);
		return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, quat);
	}
	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, mRotation);
}

DirectX::SimpleMath::Vector3 Transform::GetUp() const
{
	if (mParent != nullptr) {
		auto quat = mParent->GetRotation();
		quat = DirectX::SimpleMath::Quaternion::Concatenate(quat, mRotation);
		return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, quat);
	}
	return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, mRotation);
}

DirectX::BoundingOrientedBox& Transform::GetBB() 
{
	return mWorldBoundingBox;
}

DirectX::SimpleMath::Matrix& Transform::CreateWorldMatrix()
{
	if (mParent != nullptr) {
		mWorldMatrix =  DirectX::SimpleMath::Matrix::CreateScale(mScale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(mRotation) * DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) *  ( mLocalTransform * mParent->GetWorldMatrix() );
	}
	else {
		mWorldMatrix = mLocalTransform *  DirectX::SimpleMath::Matrix::CreateScale(mScale) *  DirectX::SimpleMath::Matrix::CreateFromQuaternion(mRotation) * DirectX::SimpleMath::Matrix::CreateTranslation(mPosition) ;
	}

	mOrientedBoundingBox.Transform(mWorldBoundingBox, mWorldMatrix);

	return mWorldMatrix;
}

DirectX::SimpleMath::Matrix Transform::GetWorldMatrix()
{
	return mWorldMatrix;
}
