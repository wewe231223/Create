#pragma once 
// 모델 컨테이너가 현재 존재하는 모델이 없는지 검사합니다.
#define MODEL_CONT_CHECK_EMPTY 

// 씬에서 사용할 모든 리소스 저장. ( 모델, 텍스쳐 등등 )  
class ResourceManager {
	class ModelContainer {
	public:
		ModelContainer();
		~ModelContainer();

		bool Empty() const noexcept;
		void Insert(const std::string& name,std::shared_ptr<class Model>&& newModel);
		std::shared_ptr<I3DRenderable> GetModel(const std::string& name);
		std::vector<std::shared_ptr<class Model>>::iterator begin();
		std::vector<std::shared_ptr<class Model>>::iterator end();
	private:
		std::vector<std::shared_ptr<class Model>>						mModels{};
		std::unordered_map<std::string, std::shared_ptr<class Model>>	mModelMap{};
	};
public:
	ResourceManager(ComPtr<ID3D12Device>& device);
	~ResourceManager();

	ComPtr<ID3D12Device> GetDevice() const noexcept;
	ComPtr<ID3D12GraphicsCommandList>& GetLoadCommandList();
	// 직렬화가 필요한 부분이다...
	template<typename T>
	void CreateShader(const std::string& name);
	void CreateTexture(const std::string& name, const fs::path& path);
	void CreateMaterial(const std::string& name, const Material& material);
	
	template<typename T,typename... Args> 
	void CreateModel(const std::string&, Args&&...);

	// 이 함수를 호출한 뒤에 반드시 FlushCommandQueue 를 호출할 것 ( 이 객체와 Fence 는 무관하기에 동작을 구분하였음 ) 
	void ExecuteUpload(ComPtr<ID3D12CommandQueue>& queue);

	D3D12_GPU_DESCRIPTOR_HANDLE					GetTexHandle();

	// NOT IMPL
	D3D12_GPU_DESCRIPTOR_HANDLE					GetTexArrHandle();
	// NOT IMPL
	D3D12_GPU_DESCRIPTOR_HANDLE					GetCubeTexHandle();
	
	std::shared_ptr<IGraphicsShader>			GetShader(const std::string& name);
	MaterialIndex								GetMaterial(const std::string& name);
	TextureIndex								GetTexture(const std::string& name);
	std::shared_ptr<I3DRenderable>				GetModel(const std::string& name);

	// 이 부분들은 렌더링할 때 호출된다. 로드 커맨드 리스트와 다르므로, 이 세 함수는 렌더 커맨드 리스트를 받아야 한다. 
	void SetGlobals(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void PrepareRender(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	void Reset();
	void UploadMaterial();
private:
	ComPtr<ID3D12Device>				mDevice{ nullptr };
	ComPtr<ID3D12GraphicsCommandList>	mLoadCommandList{ nullptr };
	ComPtr<ID3D12CommandAllocator>		mCommandAllocator{ nullptr };
			
	ComPtr<ID3D12DescriptorHeap> mTexHeap{ nullptr };

	std::unique_ptr<class DefaultBuffer>				mMaterialBuffer{ nullptr };

	std::vector<std::unique_ptr<class DefaultBuffer>>	mTextures{};
	std::vector<Material>								mMaterials{};
	
	std::unordered_map<std::string, std::shared_ptr<IGraphicsShader>>			mShaderMap{};
	std::unordered_map<std::string, TextureIndex>								mTextureMap{};
	std::unordered_map<std::string, MaterialIndex>								mMaterialMap{};

	std::unique_ptr<ModelContainer> 					mModelContainer{ nullptr };
};

template<typename T>
inline void ResourceManager::CreateShader(const std::string& name)
{
	auto newShader = std::make_shared<T>(mDevice);
	mShaderMap[name] = std::move(newShader);
}

template<typename T, typename ...Args>
inline void ResourceManager::CreateModel(const std::string& name, Args && ...args)
{
	auto newModel = std::make_shared<T>(mDevice,mLoadCommandList,std::forward<Args>(args)...);
	mModelContainer->Insert(name,std::move(newModel));
}
