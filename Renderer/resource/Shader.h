#pragma once 

// TODO : PSO, RootSignature 은 어떻게 할래? 
class Shader {
	// 셰이더 아이디 때문에 Material 클래스를 friend 로 선언함. 
	// 이것을 getter 로 하지 않은 이유는 Material 이외 그 어느 부분에서도 셰이더의 ID 를 알아야 할 일이 없기 때문임. 즉 셰이더 아이디는 Batch Rendering 을 위해서 만든 것임. 
	friend class Material;
public:
	// CS = 0, END = 6 은 별다른 의미가 있는건 아니고 그냥 보기 편하라고 적었음. 
	enum class EShaderType : UINT{
		CS = 0U,
		VS,
		PS,
		GS,
		HS,
		DS,
		END = 6U
	};
public:
	Shader();
	Shader(EShaderType shaderType, const std::string& path, const std::string& entryPoint, const std::string& shaderModel, const D3D_SHADER_MACRO* defines = nullptr);
	~Shader();
protected:
	void GetShader(EShaderType shaderType,const std::string& path, const std::string& entryPoint, const std::string& shaderModel,const D3D_SHADER_MACRO* defines = nullptr);
private:
	bool GetFileModified(const fs::path& hlslPath,const fs::path& binPath);
	void SaveBlobBinary(const fs::path& path,ComPtr<ID3D10Blob>& blob);
	void StoreShader(EShaderType shaderType,ComPtr<ID3D10Blob>&& blob);
protected:
	std::array<ComPtr<ID3DBlob>, static_cast<size_t>(EShaderType::END)> mShaderBlobs;
	size_t mShaderID{};


	ComPtr<ID3DBlob> mRootSignatureBlob{ nullptr };
	ComPtr<ID3D12PipelineState> mPso{ nullptr };
};