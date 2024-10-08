#pragma once 

class Camera : public RenderableComponentBase, public UpdatableComponentBase {
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

	static constexpr size_t TypeIndex = 3;
public:
	Camera(ComPtr<ID3D12Device>& device, std::shared_ptr<Window> window);
	virtual ~Camera() override;

public:
	CameraParam& GetCameraParam();
	bool Intersect(DirectX::BoundingOrientedBox& box);
	// ::LateUpdate 
	virtual void Update(GameObject* object) override;
	virtual void Render(GameObject* object, ComPtr<ID3D12GraphicsCommandList>& commandList) override;

	void UpdateProjectionMatrix();
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
