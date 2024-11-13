#pragma once 

class Transform {
public:
	Transform();
	~Transform();
	Transform(const Transform& other);
public:
	Transform& operator=(const Transform& other);

	void Translate(const DirectX::SimpleMath::Vector3& translation);
	// 월드 내 위치이기도 하지만, 자식 노드의 경우, 오프셋 위치이기도 하다. 
	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	void SetRotate(const DirectX::SimpleMath::Quaternion& rotation);
	void SetLocalTransform(const DirectX::SimpleMath::Matrix& localTransform);

	void ResetRotation();
	void RotateSmoothly(const DirectX::SimpleMath::Quaternion& rotation, float lerpFactor = 0.5f);
	void Rotate(float yaw = 0.f, float pitch = 0.f, float roll = 0.f);
	void Rotate(const DirectX::SimpleMath::Quaternion& rotation);

	void Scale(const DirectX::SimpleMath::Vector3& scale);
	void Scale(float x = 1.f, float y = 1.f, float z = 1.f);
	
	void LookAt(const Transform& target);
	void LookAt(const DirectX::SimpleMath::Vector3& worldPosition);

	void SetParent(Transform* parent);
	void SetChild(Transform* child);

	void SetOrientedBoundingBox(DirectX::BoundingOrientedBox box);

	DirectX::SimpleMath::Vector3 GetPosition() const;
	DirectX::SimpleMath::Quaternion GetRotation() const;
	DirectX::SimpleMath::Vector3 GetScale() const;

	DirectX::SimpleMath::Vector3 GetForward() const;
	DirectX::SimpleMath::Vector3 GetRight() const;
	DirectX::SimpleMath::Vector3 GetUp() const;

	DirectX::BoundingOrientedBox& GetBB();

	DirectX::SimpleMath::Matrix& CreateWorldMatrix();
	DirectX::SimpleMath::Matrix GetWorldMatrix();
private:
	DirectX::SimpleMath::Vector3	mPosition{ DirectX::SimpleMath::Vector3::Zero };
	DirectX::SimpleMath::Quaternion mRotation{ DirectX::SimpleMath::Quaternion::Identity };
	DirectX::SimpleMath::Vector3	mScale{ DirectX::SimpleMath::Vector3::One };

	DirectX::SimpleMath::Matrix		mLocalTransform{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix		mWorldMatrix{ DirectX::SimpleMath::Matrix::Identity };

	DirectX::BoundingOrientedBox	mOrientedBoundingBox{};
	DirectX::BoundingOrientedBox	mWorldBoundingBox{};

	Transform*						mParent{ nullptr };

	// 나중에 애니매이션에 쓰자.. 
	std::vector<Transform*>			mChildren{};
};