#pragma once 


class SkyBox {
public:
	SkyBox(std::shared_ptr<I3DRenderable> model,const std::vector<MaterialIndex> images);
	~SkyBox();
public:
	void Render(const DirectX::SimpleMath::Matrix& translation);
private:
	ModelContext mContext{};

	std::shared_ptr<I3DRenderable> mModel{ nullptr };
	std::vector<MaterialIndex> mImages{};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 그림자 매핑에 대하여.. 
// 
// 1. 조명의 위치로 카메라를 옮겨서 그림자를 캐스팅 할 오브젝트들을 그린다. 
// 2. 해당 렌더링의 깊이 값을 텍스쳐로 가져와 저장한다. 
// 3. 원래 카메라의 위치로 돌아와, 오브젝트를 렌더링 한다. 
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	void SetCameraSkyBox(std::shared_ptr<SkyBox>&& skybox);

	Transform& GetTransform();

	bool IsInFrustum(DirectX::BoundingOrientedBox& box);

	void Update();
	void RenderSkyBox();
	D3D12_GPU_VIRTUAL_ADDRESS GetCameraBufferAddress();
private:
	Transform mTransform{};
	CameraParam mParam{};

	DirectX::SimpleMath::Matrix mProjectionMatrix{};
	DirectX::SimpleMath::Matrix mViewMatrix{};

	DirectX::BoundingFrustum mViewFrustum{};
	DirectX::BoundingFrustum mWorldFrustum{};

	std::shared_ptr<SkyBox> mSkyBox{ nullptr };

	std::array<CameraBuffer, static_cast<size_t>(GC_FrameCount)> mCameraBuffers{};
	size_t mMemoryIndex{ 0 };
};
