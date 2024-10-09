#pragma once 

class RigidBody {
public:
	RigidBody(DirectX::BoundingOrientedBox orign);
	~RigidBody();

public:

private:
	DirectX::BoundingOrientedBox mOrientedBox{};
	DirectX::BoundingOrientedBox mBoundingBox{};
};