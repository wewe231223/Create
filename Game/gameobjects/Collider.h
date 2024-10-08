#pragma once 

class Collider {
public:
	Collider();
	~Collider();

public:

private:
	DirectX::BoundingBox mBoundingBox{};
};