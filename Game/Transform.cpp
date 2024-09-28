#include "pch.h"
#include "Game/Transform.h"

Transform::Transform(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 rotation, DirectX::SimpleMath::Vector3 scale)
	: mPosition(position), mRotation(rotation), mScale(scale)
{
}

Transform::~Transform()
{

}
