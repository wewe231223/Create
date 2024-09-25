#pragma once 

class Transform {
public:
	Transform(DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3{ 0.f,0.f,0.f }, DirectX::SimpleMath::Vector3 rotation = DirectX::SimpleMath::Vector3{ 0.f,0.f,0.f }, DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3{ 1.f,1.f,1.f });
	~Transform();

private:
	DirectX::SimpleMath::Vector3 mPosition{};
	DirectX::SimpleMath::Vector3 mRotation{};
	DirectX::SimpleMath::Vector3 mScale{};
};