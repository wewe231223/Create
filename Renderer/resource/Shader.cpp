#include "pch.h"
#include "resource/Shader.h"
#include "ui/Console.h"

//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Shader Basis								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////
size_t id = 0;

GraphicsShaderBase::GraphicsShaderBase(ComPtr<ID3D12Device>& device)
{

    mShaderID = id++;


	mStaticSamplers[0] = { 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP };
	mStaticSamplers[1] = { 1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP };
	mStaticSamplers[2] = { 2, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP };
	mStaticSamplers[3] = { 3, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP };
	mStaticSamplers[4] = { 4, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 8 };
	mStaticSamplers[5] = { 5, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 8 };


}

GraphicsShaderBase::~GraphicsShaderBase()
{
}

size_t GraphicsShaderBase::GetShaderID() const noexcept
{
    return mShaderID;
}

void GraphicsShaderBase::SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	assert( mPipelineState.Get() && mRootSignature.Get() );
	commandList->SetGraphicsRootSignature(mRootSignature.Get());
	commandList->SetPipelineState(mPipelineState.Get());

}

bool GraphicsShaderBase::CheckAttribute(VertexAttribute attribute)
{
    return mAttribute ^ attribute;
}


void GraphicsShaderBase::MakeShader(EShaderType shaderType,const std::string& path, const std::string& entryPoint, const std::string& shaderModel, const D3D_SHADER_MACRO* defines)
{
    fs::path hlslPath{ "./Shaders/HLSLs/" + path };
    fs::path binaryPath{ "./Shaders/Binarys/" + hlslPath.stem().string() };



    switch (shaderType)
    {
    case GraphicsShaderBase::EShaderType::VS:
		binaryPath += "_VS";
        break;
    case GraphicsShaderBase::EShaderType::PS:
		binaryPath += "_PS";
        break;
    case GraphicsShaderBase::EShaderType::GS:
		binaryPath += "_GS";
        break;
    case GraphicsShaderBase::EShaderType::HS:
		binaryPath += "_HS";
        break;
    case GraphicsShaderBase::EShaderType::DS:
		binaryPath += "_DS";
        break;
    case GraphicsShaderBase::EShaderType::END:
        break;
    default:
        break;
    }


	binaryPath += ".cso";
    


	if (!fs::exists(hlslPath) and !fs::exists(binaryPath)) {
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

    if (fs::exists(binaryPath) and not GetFileModified(hlslPath, binaryPath) and not CheckParams()) {
		Console.InfoLog("{} 파일을 로드합니다.", binaryPath.string());
        D3DReadFileToBlob(binaryPath.c_str(), &byteCode);
        return StoreShader(shaderType, std::move(byteCode));
    }
    Console.InfoLog("{} 파일을 컴파일합니다.", hlslPath.string());
    HRESULT hr = D3DCompileFromFile(hlslPath.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), shaderModel.c_str(), compileFlags, 0, &byteCode, &errors);

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

D3D12_SHADER_BYTECODE GraphicsShaderBase::GetShaderByteCode(EShaderType shaderType) const noexcept
{
	auto& shader = mShaderBlobs[static_cast<size_t>(shaderType)];
	return shader ? D3D12_SHADER_BYTECODE{ shader->GetBufferPointer(), shader->GetBufferSize() } : D3D12_SHADER_BYTECODE(nullptr);
}

bool GraphicsShaderBase::GetFileModified(const fs::path& hlslPath, const fs::path& binPath)
{
    if (!fs::exists(binPath)) {
        return true;
    }
    return fs::last_write_time(hlslPath) > fs::last_write_time(binPath);
 
}

void GraphicsShaderBase::SaveBlobBinary(const fs::path& path,ComPtr<ID3D10Blob>& blob)
{
    std::ofstream ofs(path, std::ios::binary);
    ofs.write(reinterpret_cast<char*>(blob->GetBufferPointer()), blob->GetBufferSize());
	Console.InfoLog("{} 파일에 셰이더를 저장하였습니다.", path.string());
}



void GraphicsShaderBase::StoreShader(EShaderType shaderType,ComPtr<ID3D10Blob>&& blob)
{
	mShaderBlobs[static_cast<size_t>(shaderType)] = std::move(blob);
}

void GraphicsShaderBase::DeleteAllBinarys()
{
	fs::remove_all("Shaders/Binarys");
	fs::create_directories("Shaders/Binarys");
}

bool GraphicsShaderBase::CheckParams()
{
    if (!fs::exists("Shaders/Binarys/Params.cso")) {
		std::ofstream out{ "Shaders/Binarys/Params.cso", std::ios::trunc };
        out << 123;
		out.close();
        return true;
    }

	auto result = fs::last_write_time("Shaders/HLSLs/Params.hlsli") > fs::last_write_time("Shaders/Binarys/Params.cso");
	if (result) {
        Console.InfoLog("셰이더 Param 이 수정되었습니다. 모든 셰이더를 다시 컴파일합니다.");
        GraphicsShaderBase::DeleteAllBinarys();
        std::ofstream out{ "Shaders/Binarys/Params.cso" };
        out.close();
	}

    return result;
}



ComputeShaderBase::ComputeShaderBase()
{
}

ComputeShaderBase::~ComputeShaderBase()
{
}

void ComputeShaderBase::SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    assert(not mPipelineState | not mRootSignature);
	commandList->SetPipelineState(mPipelineState.Get());
	commandList->SetComputeRootSignature(mRootSignature.Get());
}

void ComputeShaderBase::GetShader(const std::string& path, const std::string& entryPoint, const std::string& shaderModel, const D3D_SHADER_MACRO* defines)
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
    if (fs::exists(binaryPath) and !GetFileModified(hlslPath, binaryPath)) {
        Console.InfoLog("{} 파일을 로드합니다.", binaryPath.string());
        D3DReadFileToBlob(binaryPath.c_str(), &byteCode);
        mShaderBlob = std::move(byteCode);
    }
	Console.InfoLog("{} 파일을 컴파일합니다.", hlslPath.string());
    HRESULT hr = D3DCompileFromFile(hlslPath.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), shaderModel.c_str(), compileFlags, 0, &byteCode, &errors);

    if (FAILED(hr)) {
        if (errors) {
            //::SetLastError(ERROR_INVALID_ACCESS); TBD 
            ::OutputDebugStringA((char*)errors->GetBufferPointer());
            ::CheckHR(hr);
        }
        return;
    }
    SaveBlobBinary(binaryPath, byteCode);
    mShaderBlob = std::move(byteCode);
}

bool ComputeShaderBase::GetFileModified(const fs::path& hlslPath, const fs::path& binPath)
{
    if (!fs::exists(binPath)) {
        return true;
    }
    return fs::last_write_time(hlslPath) > fs::last_write_time(binPath);
}

void ComputeShaderBase::SaveBlobBinary(const fs::path& path, ComPtr<ID3D10Blob>& blob)
{
    std::ofstream ofs(path, std::ios::binary);
    ofs.write(reinterpret_cast<char*>(blob->GetBufferPointer()), blob->GetBufferSize());
    Console.InfoLog("{} 파일에 셰이더를 저장하였습니다.", path.string());
}




//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Standard Shader								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////



StandardShader::StandardShader(ComPtr<ID3D12Device>& device) : GraphicsShaderBase(device)
{

}

StandardShader::~StandardShader()
{

}






//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Terrain Shader								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

TerrainShader::TerrainShader(ComPtr<ID3D12Device>& device)
    : GraphicsShaderBase(device)
{
    mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_texcoord1 | VertexAttrib_texcoord2;
    MakeShader(EShaderType::VS, "Terrain.hlsl", "TerrainVS", "vs_5_1", nullptr);
	MakeShader(EShaderType::PS, "Terrain.hlsl", "TerrainPS", "ps_5_1", nullptr);


    D3D12_INPUT_ELEMENT_DESC inputDescs[4]{};

    inputDescs[0] = { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    inputDescs[1] = { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,1,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    inputDescs[2] = { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,2,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    inputDescs[3] = { "TEXCOORD",1,DXGI_FORMAT_R32G32_FLOAT,3,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

    // 애는 하나만씀. 
	D3D12_DESCRIPTOR_RANGE texRange[3];
    // Tex2D 
    texRange[0].BaseShaderRegister = 2;
	texRange[0].NumDescriptors = 1024;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	texRange[0].RegisterSpace = 0;
    // Tex2DArray
    texRange[1].BaseShaderRegister = 2;
    texRange[1].NumDescriptors = 512;
    texRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[1].RegisterSpace = 1;
    // TexCube 
    texRange[2].BaseShaderRegister = 2;
    texRange[2].NumDescriptors = 512;
    texRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[2].RegisterSpace = 2;

	D3D12_ROOT_PARAMETER rootParams[GRP_END]{};

    rootParams[GRP_MeshConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParams[GRP_MeshConstants].Descriptor.ShaderRegister = 0;
	rootParams[GRP_MeshConstants].Descriptor.RegisterSpace = 0;
	rootParams[GRP_MeshConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_CameraConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[GRP_CameraConstants].Descriptor.ShaderRegister = 1;
	rootParams[GRP_CameraConstants].Descriptor.RegisterSpace = 0;
	rootParams[GRP_CameraConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParams[GRP_ObjectConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParams[GRP_ObjectConstants].Descriptor.ShaderRegister = 0;
	rootParams[GRP_ObjectConstants].Descriptor.RegisterSpace = 0;
	rootParams[GRP_ObjectConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParams[GRP_MaterialSRV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParams[GRP_MaterialSRV].Descriptor.ShaderRegister = 1;
	rootParams[GRP_MaterialSRV].Descriptor.RegisterSpace = 0;
	rootParams[GRP_MaterialSRV].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_Texture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[GRP_Texture].DescriptorTable.NumDescriptorRanges = _countof(texRange);
	rootParams[GRP_Texture].DescriptorTable.pDescriptorRanges = texRange;
	rootParams[GRP_Texture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC rootDesc{};
    rootDesc.NumParameters = _countof(rootParams);
    rootDesc.pParameters = rootParams;
    rootDesc.NumStaticSamplers = static_cast<UINT>(mStaticSamplers.size());
    rootDesc.pStaticSamplers = mStaticSamplers.data();
    rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;

    auto hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf());
    if (FAILED(hr)) {
        OutputDebugStringA(static_cast<const char*>(error->GetBufferPointer()));
    }

    CheckHR(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));




    // 파이프라인 상태 객체(PSO) 생성
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputDescs, _countof(inputDescs) };
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = GetShaderByteCode(EShaderType::VS);
    psoDesc.PS = GetShaderByteCode(EShaderType::PS);
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
    psoDesc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = static_cast<DXGI_FORMAT>(EGlobalConstants::GC_RenderTargetFormat);
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;



    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mPipelineState.GetAddressOf())));

    Console.InfoLog("Terrain Shader 가 성공적으로 로딩되었습니다.");

}

TerrainShader::~TerrainShader()
{
}
