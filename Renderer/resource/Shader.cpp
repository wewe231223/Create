#include "pch.h"
#include "resource/Shader.h"
#include "ui/Console.h"

size_t id = 0;

Shader::Shader()
{
	mShaderID = id++;
}

Shader::Shader(EShaderType shaderType, const std::string& path, const std::string& entryPoint, const std::string& shaderModel, const D3D_SHADER_MACRO* defines)
{
	mShaderID = id++;
	Shader::GetShader(shaderType, path, entryPoint, shaderModel, defines);
}

Shader::~Shader()
{
}

void Shader::GetShader(EShaderType shaderType,const std::string& path, const std::string& entryPoint, const std::string& shaderModel, const D3D_SHADER_MACRO* defines)
{
    fs::path hlslPath{ "./Shaders/HLSLs/" + path };
    fs::path binaryPath{ "./Shaders/Binarys/" + hlslPath.stem().string() + ".cso" };

	if (!fs::exists(hlslPath) && !fs::exists(binaryPath)) {
		::CheckHR(D3D11_ERROR_FILE_NOT_FOUND);
	}

    ComPtr<ID3DBlob> byteCode = nullptr;
    ComPtr<ID3DBlob> errors;
    
	UINT compileFlags = 0;

#ifdef _DEBUG 
     compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else 
     compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif 
    if (fs::exists(binaryPath) && !GetFileModified(hlslPath, binaryPath)) {
		Console.InfoLog("{} 파일을 로드합니다.", binaryPath.string());
        D3DReadFileToBlob(binaryPath.c_str(), &byteCode);
        return StoreShader(shaderType, std::move(byteCode));
    }

    HRESULT hr = D3DCompileFromFile(hlslPath.c_str(), defines, nullptr, entryPoint.c_str(), shaderModel.c_str(), compileFlags, 0, &byteCode, &errors);

    if (FAILED(hr)) {
        if (errors) {
            //::SetLastError(ERROR_INVALID_ACCESS); TBD 
			::OutputDebugStringA((char*)errors->GetBufferPointer());
            ::CheckHR(hr);
        }
        return;
    }
    SaveBlobBinary(binaryPath,byteCode);
	StoreShader(shaderType, std::move(byteCode));
}

bool Shader::GetFileModified(const fs::path& hlslPath, const fs::path& binPath)
{
    if (!fs::exists(binPath)) {
        return true;
    }
    bool result = fs::last_write_time(hlslPath) > fs::last_write_time(binPath);
    return result;
}

void Shader::SaveBlobBinary(const fs::path& path,ComPtr<ID3D10Blob>& blob)
{
    std::ofstream ofs(path, std::ios::binary);
    ofs.write(reinterpret_cast<char*>(blob->GetBufferPointer()), blob->GetBufferSize());
	Console.InfoLog("{} 파일에 셰이더를 저장하였습니다.", path.string());
}

void Shader::StoreShader(EShaderType shaderType,ComPtr<ID3D10Blob>&& blob)
{
	mShaderBlobs[static_cast<size_t>(shaderType)] = std::move(blob);
}
