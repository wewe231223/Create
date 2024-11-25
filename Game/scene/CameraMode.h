#pragma once 


//
// 카메라의 모드는 크게 두가지가 있다.
// 1. 입력 콜백 함수 관리 ( 추가 / 삭제 ) 
// 2. 카메라 업데이트 관여,카메라가 따라다닐 객체 등.. ( 카메라의 위치, 회전, 줌 등을 관리 )
// 
// 이 클래스를 설계한 이유는 카메라 모드의 전환을 보다 용이하게 하기 위함임. 
// 

//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							CameraMode Base								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class CameraMode {
public:
	CameraMode(std::shared_ptr<class Camera> camera);
	virtual ~CameraMode();
public:
	std::shared_ptr<CameraMode> ChangeCameraMode(std::shared_ptr<CameraMode>& mode);

	virtual void Enter() PURE;
	virtual void Exit() PURE;
	virtual void Update() PURE;

	virtual ECameraType GetType() const PURE;

protected:
	std::shared_ptr<class Camera> mCamera{ nullptr };

	int mInputCallBackSign{};
};



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Free Camera Mode							//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


class FreeCameraMode : public CameraMode {
public:
	FreeCameraMode(std::shared_ptr<class Camera> camera);
	virtual ~FreeCameraMode();
public:
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual ECameraType GetType() const;
};





//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							TPP Camera Mode								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


class TPPCameraMode : public CameraMode {
public:
	TPPCameraMode(std::shared_ptr<class Camera> camera, Transform& transform, const DirectX::SimpleMath::Vector3& offset);
	virtual ~TPPCameraMode();
public:
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
	virtual ECameraType GetType() const;
private:
	Transform& mTargetTransform;
	DirectX::SimpleMath::Vector3 mOffset{ DirectX::SimpleMath::Vector3::Zero };
};


