#pragma once 

class Shader {
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

	void GetShader(EShaderType shaderType,const std::string& path, const std::string& entryPoint, const std::string& shaderModel,const D3D_SHADER_MACRO* defines = nullptr);
private:
	bool GetFileModified(const fs::path& hlslPath,const fs::path& binPath);
	void SaveBlobBinary(const fs::path& path,ComPtr<ID3D10Blob>& blob);
	void StoreShader(EShaderType shaderType,ComPtr<ID3D10Blob>&& blob);
private:
	std::array<ComPtr<ID3DBlob>, static_cast<size_t>(EShaderType::END)> mShaderBlobs;
	size_t mShaderID{};
};