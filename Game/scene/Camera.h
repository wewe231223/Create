#pragma once 

class Camera {
	struct CameraBuffer {
		ComPtr<ID3D12Resource> buffer{ nullptr };
		CameraContext* bufferptr{};
	};
public:
	struct CameraParam {
		float FOV{ 60.f };
		float aspectRatio{ 1.0f };
		float nearPlane{ 0.1f };
		float farPlane{ 1000.f };
	};
public:
	Camera(ComPtr<ID3D12Device>& device,std::shared_ptr<Window> window);
	~Camera();
public:
	void MakeProjectionMatrix();

	Transform& GetTransform();

	bool IsInFrustum(DirectX::BoundingOrientedBox& box);

	void Update();
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	Transform mTransform{};
	CameraParam mParam{};

	DirectX::SimpleMath::Matrix mProjectionMatrix{};
	DirectX::SimpleMath::Matrix mViewMatrix{};

	DirectX::BoundingFrustum mViewFrustum{};
	DirectX::BoundingFrustum mWorldFrustum{};

	std::array<CameraBuffer, static_cast<size_t>(GC_FrameCount)> mCameraBuffers{};
	size_t mMemoryIndex{ 0 };
};
