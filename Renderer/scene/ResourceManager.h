#pragma once 
// 씬에서 사용할 모든 리소스 저장. ( 모델, 텍스쳐 등등 )  
class ResourceManager {
	class ModelContainer {
	public:
		ModelContainer();
		~ModelContainer();

		void Insert(const std::string& name,std::shared_ptr<class Model>&& newModel);
		std::shared_ptr<IRendererEntity> GetModel(const std::string& name, const std::vector<MaterialIndex>& materials);
		std::vector<std::shared_ptr<class Model>>::iterator begin();
		std::vector<std::shared_ptr<class Model>>::iterator end();
	private:
		std::vector<std::shared_ptr<class Model>>						mModels{};
		std::unordered_map<std::string, std::shared_ptr<class Model>>	mModelMap{};
	};
public:
	ResourceManager(ComPtr<ID3D12Device>& device);
	~ResourceManager();

	template<typename T>
	void CreateShader(ComPtr<ID3D12Device>& device, const std::string& name);

	void CreateTexture(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, const std::string& name, const fs::path& path);
	void CreateMaterial(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, const std::string& name, const Material& material);
	
	template<typename T,typename... Args> 
	void CreateModel(const std::string&,ComPtr<ID3D12Device>&,ComPtr<ID3D12GraphicsCommandList>&,Args&&...);
	void UploadMaterial(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList);

	
	void SetGlobals(ComPtr<ID3D12GraphicsCommandList>& commandList);

	std::shared_ptr<IGraphicsShader>			GetShader(const std::string& name);
	MaterialIndex								GetMaterial(const std::string& name);
	TextureIndex								GetTexture(const std::string& name);
	std::shared_ptr<IRendererEntity>			GetModel(const std::string& name, const std::vector<MaterialIndex>& materials);

	void PrepareRender(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
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
inline void ResourceManager::CreateShader(ComPtr<ID3D12Device>& device,const std::string& name)
{
	auto newShader = std::make_shared<T>(device);
	mShaderMap[name] = std::move(newShader);
}

template<typename T, typename ...Args>
inline void ResourceManager::CreateModel(const std::string& name,ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, Args && ...args)
{
	auto newModel = std::make_shared<T>(device,commandList,std::forward<Args>(args)...);
	mModelContainer->Insert(name,std::move(newModel));
}
