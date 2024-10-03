#pragma once 

/*
1. Position, Rotation, Scale 관련 함수
Translate(Vector3 translation): 오브젝트를 지정한 방향으로 이동시킵니다.
Rotate(Vector3 eulerAngles): 오일러 각도를 기준으로 오브젝트를 회전시킵니다.
RotateAround(Vector3 point, Vector3 axis, float angle): 특정 점을 기준으로 지정된 축을 따라 회전시킵니다.
LookAt(Transform target): 다른 Transform을 바라보도록 회전시킵니다.
LookAt(Vector3 worldPosition): 특정 좌표를 바라보도록 회전시킵니다.


2. Child 관련 함수
SetParent(Transform parent): 부모를 설정합니다.
DetachChildren(): 자식 오브젝트를 모두 부모로부터 분리합니다.
Find(string name): 자식 오브젝트 중 이름으로 특정 오브젝트를 찾습니다.
GetChild(int index): 자식 오브젝트를 인덱스로 가져옵니다.
GetSiblingIndex(): 형제 사이에서의 인덱스를 반환합니다.
SetSiblingIndex(int index): 형제 사이에서의 인덱스를 설정합니다.
GetEnumerator(): 자식 오브젝트를 반복할 수 있는 열거자를 반환합니다.

*/


class Transform {
public:
	Transform();
	~Transform();

	void Translate(const DirectX::SimpleMath::Vector3& translation);
	void Rotate(float yaw = 0.f, float pitch = 0.f, float roll = 0.f);
	void Scale(const DirectX::SimpleMath::Vector3& scale);
	void Scale(float x = 1.f, float y = 1.f, float z = 1.f);
	void LookAt(const Transform& target);
	void LookAt(const DirectX::SimpleMath::Vector3& worldPosition);

	DirectX::SimpleMath::Vector3 GetPosition() const;
	DirectX::SimpleMath::Quaternion GetRotation() const;
	DirectX::SimpleMath::Vector3 GetScale() const;

	DirectX::SimpleMath::Vector3 GetFoward() const;
	DirectX::SimpleMath::Vector3 GetRight() const;
	DirectX::SimpleMath::Vector3 GetUp() const;

	DirectX::SimpleMath::Matrix GetWorldMatrix() const ;
private:
	DirectX::SimpleMath::Vector3 mPosition{ DirectX::SimpleMath::Vector3::Zero };
	DirectX::SimpleMath::Quaternion mRotation{ DirectX::SimpleMath::Quaternion::Identity };
	DirectX::SimpleMath::Vector3 mScale{ DirectX::SimpleMath::Vector3::Zero };
};

