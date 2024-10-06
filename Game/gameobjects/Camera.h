#pragma once 

class Camera {
	struct CameraBuffer {
		ComPtr<ID3D12Resource> mBuffer{ nullptr };
		CameraContext* mBufferPtr{ nullptr };
	};
public:
	struct CameraParam {
		float FOV;
		float NearZ;
		float FarZ;
		float Aspect;
	};
public:
	Camera(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList,std::shared_ptr<Window> window);
	~Camera();

	CameraParam& GetCameraParam();

	bool Intersect(DirectX::BoundingOrientedBox& box);
	void UpdateDynamicVariables();
	void UpdateStaticVariables();
	void SetVariables(ComPtr<ID3D12GraphicsCommandList>& commandList);

	Transform& GetTransform();
private:
	CameraParam														mCameraParam{};
	std::shared_ptr<Window>											mWindow{ nullptr };
	std::array<CameraBuffer, static_cast<size_t>(GC_FrameCount)>	mCameraContexts{};
	UINT															mMemoryIndex{ 0 };

	DirectX::SimpleMath::Matrix										mProjectionMatrix{};
	DirectX::SimpleMath::Matrix										mViewMatrix{};

	DirectX::SimpleMath::Vector3									mPosition{ 0.f,0.f,0.f };
	DirectX::SimpleMath::Quaternion									mRotation{ 0.f,0.f,0.f,1.f };
	
	DirectX::BoundingFrustum										mViewFrustum{};
	DirectX::BoundingFrustum										mWorldFrustum{};

	Transform mTransform{};
};


namespace TEST {
	class Camera : public RenderableComponentBase {
		struct CameraBuffer {
			ComPtr<ID3D12Resource> mBuffer{ nullptr };
			CameraContext* mBufferPtr{ nullptr };
		};
	public:
		struct CameraParam {
			float FOV;
			float NearZ;
			float FarZ;
			float Aspect;
		};
	public:
		Camera(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commnadList,std::shared_ptr<Window> window);
		virtual ~Camera() override;

	public:
		CameraParam& GetCameraParam();
		bool Intersect(DirectX::BoundingOrientedBox& box);
		// 렌더 할 때 카메라 움직임 <- 이러면 Frustum culling 이 제대로 안됨 
		// 
		virtual void Render(GameObject* object, ComPtr<ID3D12GraphicsCommandList>& commandList) override;
	private:
		std::shared_ptr<Window> mWindow{ nullptr };
		CameraParam mCameraParam{};

		std::array<CameraBuffer, static_cast<size_t>(GC_FrameCount)> mCameraContexts{};
		UINT mMemoryIndex{ 0 };

		DirectX::SimpleMath::Matrix mProjectionMatrix{};
		DirectX::SimpleMath::Matrix mViewMatrix{};

		DirectX::BoundingFrustum mViewFrustum{};
		DirectX::BoundingFrustum mWorldFrustum{};
	};
}