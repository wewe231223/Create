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
// 셰이더 사이의 서열이 필요하다. 반드시 먼저 그려져야 하는 셰이더들은 Reserved 보다 작은 ID 를 통해 먼저 그려질 수 있도록 하자.
// ReservedID 사용 현황 
// 0 : SkyBoxShader
// 1 : 
// 2 : 
// 3 :
// 4 :

size_t shaderIDReserved = 4;
size_t id = 0;

GraphicsShaderBase::GraphicsShaderBase(ComPtr<ID3D12Device>& device)
{

	mShaderID = shaderIDReserved + (++id);


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

bool GraphicsShaderBase::SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList, bool shadow)
{
    // 그림자 매핑을 위한 Set 함수. 여기서는 픽셀 셰이더를 제외한 파이프라인을 생성자에서 정의한 경우, 그것을 set 한다. 그렇지 않다면 아무행위도 하지 않는다. 
   
    if (shadow) {
        if (not mShadowPipelineState) {
            // 이 경로에서만 유일하게 false 를 리턴한다. 이는 shadow casting 을 하지 않을 객체들은 불필요하게 pipelinestate 를 정의하지 않고, 그리지 않기 위함이다. 
            return false;
        }
        assert(mRootSignature.Get());
        commandList->SetGraphicsRootSignature(mRootSignature.Get());
		commandList->SetPipelineState(mShadowPipelineState.Get());
        return true;
    }
    else {
	    assert( mRenderPipelineState.Get() && mRootSignature.Get() );
	    commandList->SetGraphicsRootSignature(mRootSignature.Get());
	    commandList->SetPipelineState(mRenderPipelineState.Get());
    }
    return true;
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
        ::CheckHR(D2DERR_SHADER_COMPILE_FAILED);
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

    if (!fs::exists("Shaders/Binarys/Lights.cso")) {
        std::ofstream out{ "Shaders/Binarys/Lights.cso", std::ios::trunc };
        out << 123;
        out.close();
        return true;
    }

	auto paramResult = fs::last_write_time("Shaders/HLSLs/Params.hlsl") > fs::last_write_time("Shaders/Binarys/Params.cso");
	if (paramResult) {
        Console.InfoLog("셰이더 Param 이 수정되었습니다. 모든 셰이더를 다시 컴파일합니다.");
        GraphicsShaderBase::DeleteAllBinarys();
        std::ofstream out{ "Shaders/Binarys/Params.cso" };
        out.close();
	}

    auto lightResult = fs::last_write_time("Shaders/HLSLs/Lights.hlsl") > fs::last_write_time("Shaders/Binarys/Lights.cso");
    if (lightResult) {
        Console.InfoLog("셰이더 Light 이 수정되었습니다. 모든 셰이더를 다시 컴파일합니다.");
        GraphicsShaderBase::DeleteAllBinarys();
        std::ofstream out{ "Shaders/Binarys/Lights.cso" };
        out.close();
    }



    return ( paramResult and lightResult );
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
    texRange[0].BaseShaderRegister = 4;
    texRange[0].NumDescriptors = 1024;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[0].RegisterSpace = 0;
    // Tex2DArray
    texRange[1].BaseShaderRegister = 4;
    texRange[1].NumDescriptors = 512;
    texRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[1].RegisterSpace = 1;
    // TexCube 
    texRange[2].BaseShaderRegister = 4;
    texRange[2].NumDescriptors = 512;
    texRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[2].RegisterSpace = 2;


    D3D12_ROOT_PARAMETER rootParams[GRP_END]{};

    rootParams[GRP_CameraConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_CameraConstants].Descriptor.ShaderRegister = 1;
    rootParams[GRP_CameraConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_CameraConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MeshConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MeshConstants].Descriptor.ShaderRegister = 0;
    rootParams[GRP_MeshConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_MeshConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_LightInfo].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_LightInfo].Descriptor.ShaderRegister = 1;
    rootParams[GRP_LightInfo].Descriptor.RegisterSpace = 0;
    rootParams[GRP_LightInfo].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowMap].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[GRP_ShadowMap].Constants.Num32BitValues = 1;
    rootParams[GRP_ShadowMap].Constants.ShaderRegister = 2;
    rootParams[GRP_ShadowMap].Constants.RegisterSpace = 0;
    rootParams[GRP_ShadowMap].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_ShadowTransform].Descriptor.ShaderRegister = 3;
    rootParams[GRP_ShadowTransform].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ShadowTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ObjectConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_ObjectConstants].Descriptor.ShaderRegister = 2;
    rootParams[GRP_ObjectConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ObjectConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MaterialSRV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MaterialSRV].Descriptor.ShaderRegister = 3;
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

	CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mShadowPipelineState.GetAddressOf())));

    psoDesc.PS = GetShaderByteCode(EShaderType::PS);
    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

    Console.InfoLog("Terrain Shader 가 성공적으로 로딩되었습니다.");

}

TerrainShader::~TerrainShader()
{
}





//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//						TexturedObject Shader							//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////



TexturedObjectShader::TexturedObjectShader(ComPtr<ID3D12Device>& device) : GraphicsShaderBase(device)
{
    mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_texcoord1;
    MakeShader(EShaderType::VS, "TexturedObject.hlsl", "TexturedObjectVS", "vs_5_1", nullptr);
    MakeShader(EShaderType::PS, "TexturedObject.hlsl", "TexturedObjectPS", "ps_5_1", nullptr);


    D3D12_INPUT_ELEMENT_DESC inputDescs[3]{};

    inputDescs[0] = { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    inputDescs[1] = { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,1,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    inputDescs[2] = { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,2,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

    // 애는 하나만씀. 
    D3D12_DESCRIPTOR_RANGE texRange[3];
    // Tex2D 
    texRange[0].BaseShaderRegister = 4;
    texRange[0].NumDescriptors = 1024;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[0].RegisterSpace = 0;
    // Tex2DArray
    texRange[1].BaseShaderRegister = 4;
    texRange[1].NumDescriptors = 512;
    texRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[1].RegisterSpace = 1;
    // TexCube 
    texRange[2].BaseShaderRegister = 4;
    texRange[2].NumDescriptors = 512;
    texRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[2].RegisterSpace = 2;


    D3D12_ROOT_PARAMETER rootParams[GRP_END]{};

    rootParams[GRP_CameraConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_CameraConstants].Descriptor.ShaderRegister = 1;
    rootParams[GRP_CameraConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_CameraConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MeshConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MeshConstants].Descriptor.ShaderRegister = 0;
    rootParams[GRP_MeshConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_MeshConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_LightInfo].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_LightInfo].Descriptor.ShaderRegister = 1;
    rootParams[GRP_LightInfo].Descriptor.RegisterSpace = 0;
    rootParams[GRP_LightInfo].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowMap].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[GRP_ShadowMap].Constants.Num32BitValues = 1;
    rootParams[GRP_ShadowMap].Constants.ShaderRegister = 2;
    rootParams[GRP_ShadowMap].Constants.RegisterSpace = 0;
    rootParams[GRP_ShadowMap].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[GRP_ShadowTransform].Descriptor.ShaderRegister = 3;
	rootParams[GRP_ShadowTransform].Descriptor.RegisterSpace = 0;
	rootParams[GRP_ShadowTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ObjectConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_ObjectConstants].Descriptor.ShaderRegister = 2;
    rootParams[GRP_ObjectConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ObjectConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MaterialSRV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MaterialSRV].Descriptor.ShaderRegister = 3;
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


    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mShadowPipelineState.GetAddressOf())));

    psoDesc.PS = GetShaderByteCode(EShaderType::PS);
    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

    Console.InfoLog("TexturedObject Shader 가 성공적으로 로딩되었습니다.");

}

TexturedObjectShader::~TexturedObjectShader()
{
}



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							SkyBox Shader								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

// 현재 스카이 박스의 경우 먼저 그려야 하는 방식으로 PSO 를 정의하였다. 따라서 스카이 박스 셰이더가 모든 렌더 오브젝트 중 가장 앞에 위치해야함.
SkyBoxShader::SkyBoxShader(ComPtr<ID3D12Device>& device)
	: GraphicsShaderBase(device)
{
    mShaderID = 0;

	mAttribute = VertexAttrib_position | VertexAttrib_texcoord1 ;
	MakeShader(EShaderType::VS, "SkyBox.hlsl", "SkyBoxVS", "vs_5_1", nullptr);
	MakeShader(EShaderType::PS, "SkyBox.hlsl", "SkyBoxPS", "ps_5_1", nullptr);


	D3D12_INPUT_ELEMENT_DESC inputDescs[2]{};

	inputDescs[0] = { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	inputDescs[1] = { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,1,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };


    // 애는 하나만씀. 
    D3D12_DESCRIPTOR_RANGE texRange[3];
    // Tex2D 
    texRange[0].BaseShaderRegister = 4;
    texRange[0].NumDescriptors = 1024;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[0].RegisterSpace = 0;
    // Tex2DArray
    texRange[1].BaseShaderRegister = 4;
    texRange[1].NumDescriptors = 512;
    texRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[1].RegisterSpace = 1;
    // TexCube 
    texRange[2].BaseShaderRegister = 4;
    texRange[2].NumDescriptors = 512;
    texRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[2].RegisterSpace = 2;


    D3D12_ROOT_PARAMETER rootParams[GRP_END]{};

    rootParams[GRP_CameraConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_CameraConstants].Descriptor.ShaderRegister = 1;
    rootParams[GRP_CameraConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_CameraConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MeshConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MeshConstants].Descriptor.ShaderRegister = 0;
    rootParams[GRP_MeshConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_MeshConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParams[GRP_LightInfo].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParams[GRP_LightInfo].Descriptor.ShaderRegister = 1;
	rootParams[GRP_LightInfo].Descriptor.RegisterSpace = 0;
	rootParams[GRP_LightInfo].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowMap].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[GRP_ShadowMap].Constants.Num32BitValues = 1;
    rootParams[GRP_ShadowMap].Constants.ShaderRegister = 2;
    rootParams[GRP_ShadowMap].Constants.RegisterSpace = 0;
    rootParams[GRP_ShadowMap].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_ShadowTransform].Descriptor.ShaderRegister = 3;
    rootParams[GRP_ShadowTransform].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ShadowTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ObjectConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_ObjectConstants].Descriptor.ShaderRegister = 2;
    rootParams[GRP_ObjectConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ObjectConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MaterialSRV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MaterialSRV].Descriptor.ShaderRegister = 3;
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
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = static_cast<DXGI_FORMAT>(EGlobalConstants::GC_RenderTargetFormat);
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;



    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

    Console.InfoLog("SkyBox Shader 가 성공적으로 로딩되었습니다.");
}

SkyBoxShader::~SkyBoxShader()
{
}



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							UI Shader									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

UIShader::UIShader(ComPtr<ID3D12Device>& device)
	: GraphicsShaderBase(device)
{
    mAttribute = VertexAttrib_position;
    MakeShader(EShaderType::VS, "UI.hlsl", "UiVS", "vs_5_1", nullptr);
    MakeShader(EShaderType::PS, "UI.hlsl", "UiPS", "ps_5_1", nullptr);


    D3D12_INPUT_ELEMENT_DESC inputDescs[1]{};

    inputDescs[0] = { "POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

    // 애는 하나만씀. 
    D3D12_DESCRIPTOR_RANGE texRange[1];
    // Tex2D 
    texRange[0].BaseShaderRegister = 1;
    texRange[0].NumDescriptors = 64;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[0].RegisterSpace = 0;


    D3D12_ROOT_PARAMETER rootParams[URP_END]{};

	rootParams[URP_ObjectConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParams[URP_ObjectConstants].Descriptor.ShaderRegister = 0;
	rootParams[URP_ObjectConstants].Descriptor.RegisterSpace = 0;
	rootParams[URP_ObjectConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParams[URP_Texture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[URP_Texture].DescriptorTable.NumDescriptorRanges = _countof(texRange);
	rootParams[URP_Texture].DescriptorTable.pDescriptorRanges = texRange;
	rootParams[URP_Texture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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
    
    psoDesc.BlendState.AlphaToCoverageEnable = false;
    psoDesc.BlendState.IndependentBlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


    psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.DepthStencilState.StencilEnable = false;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = static_cast<DXGI_FORMAT>(EGlobalConstants::GC_RenderTargetFormat);
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;



    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

    Console.InfoLog("UI Shader 가 성공적으로 로딩되었습니다.");
}

UIShader::~UIShader()
{
}







//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//						BillBoard Shader								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

//
//  11.16 
//  1. 빌보드 셰이더 작성 
//  2. 빌보드 파이프라인 구성 
//  3. 빌보드 렌더링 
//

BillBoardShader::BillBoardShader(ComPtr<ID3D12Device>& device)
	: GraphicsShaderBase(device)
{
    mAttribute = VertexAttrib_BillBoard;
    MakeShader(EShaderType::VS, "BillBoard.hlsl", "BillBoardVS", "vs_5_1", nullptr);
    MakeShader(EShaderType::GS, "BillBoard.hlsl", "BillBoardGS", "gs_5_1", nullptr);
    MakeShader(EShaderType::PS, "BillBoard.hlsl", "BillBoardPS", "ps_5_1", nullptr);


    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // position
        { "WIDTH",              0, DXGI_FORMAT_R32_FLOAT,       0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // halfWidth
        { "HEIGHT",             0, DXGI_FORMAT_R32_FLOAT,       0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // height
        { "UP",                 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // up
        { "TEXTUREINDEX",       0, DXGI_FORMAT_R32_UINT,        0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // texture
        { "SPRITABLE",          0, DXGI_FORMAT_R8_UINT,         0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spritable
        { "SPRITEFRAMEINROW",   0, DXGI_FORMAT_R32_UINT,        0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteFrameInRow
        { "SPRITEFRAMEINCOL",   0, DXGI_FORMAT_R32_UINT,        0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteFrameInCol
        { "SPRITEDURATION",     0, DXGI_FORMAT_R32_FLOAT,       0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteDuration
    };


    // 애는 하나만씀. 
    D3D12_DESCRIPTOR_RANGE texRange[1];
    // Tex2D 
    texRange[0].BaseShaderRegister = 0;
    texRange[0].NumDescriptors = 1024;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[0].RegisterSpace = 0;


    D3D12_ROOT_PARAMETER rootParams[BRP_END]{};

	rootParams[BRP_CameraConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[BRP_CameraConstants].Descriptor.ShaderRegister = 0;
	rootParams[BRP_CameraConstants].Descriptor.RegisterSpace = 0;
	rootParams[BRP_CameraConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParams[BRP_Time].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParams[BRP_Time].Constants.Num32BitValues = 1;
	rootParams[BRP_Time].Constants.ShaderRegister = 1;
	rootParams[BRP_Time].Constants.RegisterSpace = 0;
	rootParams[BRP_Time].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParams[BRP_Texture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[BRP_Texture].DescriptorTable.NumDescriptorRanges = _countof(texRange);
	rootParams[BRP_Texture].DescriptorTable.pDescriptorRanges = texRange;
	rootParams[BRP_Texture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


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
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = GetShaderByteCode(EShaderType::VS);
	psoDesc.GS = GetShaderByteCode(EShaderType::GS);
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
    psoDesc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
    
    psoDesc.BlendState.AlphaToCoverageEnable = false;
    psoDesc.BlendState.IndependentBlendEnable = false;
    psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = static_cast<DXGI_FORMAT>(EGlobalConstants::GC_RenderTargetFormat);
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;


	CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mShadowPipelineState.GetAddressOf())));

    psoDesc.PS = GetShaderByteCode(EShaderType::PS);
    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

    Console.InfoLog("BillBoard Shader 가 성공적으로 로딩되었습니다.");

}

BillBoardShader::~BillBoardShader()
{
}





//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//					Normal TexturedObject Shader						//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

NormalTexturedObjectShader::NormalTexturedObjectShader(ComPtr<ID3D12Device>& device)
	: GraphicsShaderBase(device)
{
	mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_tangent | VertexAttrib_bitangent | VertexAttrib_texcoord1;

    MakeShader(EShaderType::VS, "NormalTexturedObject.hlsl", "NormaledTexturedObjectVS", "vs_5_1", nullptr);
    MakeShader(EShaderType::PS, "NormalTexturedObject.hlsl", "NormaledTexturedObjectPS", "ps_5_1", nullptr);

    D3D12_INPUT_ELEMENT_DESC inputDescs[5]{};

    inputDescs[0] = { "POSITION"    ,0,DXGI_FORMAT_R32G32B32_FLOAT  ,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    inputDescs[1] = { "NORMAL"      ,0,DXGI_FORMAT_R32G32B32_FLOAT  ,1,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	inputDescs[2] = { "TANGENT"     ,0,DXGI_FORMAT_R32G32B32_FLOAT  ,2,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	inputDescs[3] = { "BINORMAL"    ,0,DXGI_FORMAT_R32G32B32_FLOAT  ,3,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    inputDescs[4] = { "TEXCOORD"    ,0,DXGI_FORMAT_R32G32_FLOAT     ,4,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

    // 애는 하나만씀. 
    D3D12_DESCRIPTOR_RANGE texRange[3];
    // Tex2D 
    texRange[0].BaseShaderRegister = 4;
    texRange[0].NumDescriptors = 1024;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[0].RegisterSpace = 0;
    // Tex2DArray
    texRange[1].BaseShaderRegister = 4;
    texRange[1].NumDescriptors = 512;
    texRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[1].RegisterSpace = 1;
    // TexCube 
    texRange[2].BaseShaderRegister = 4;
    texRange[2].NumDescriptors = 512;
    texRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[2].RegisterSpace = 2;


    D3D12_ROOT_PARAMETER rootParams[GRP_END]{};

    rootParams[GRP_CameraConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_CameraConstants].Descriptor.ShaderRegister = 1;
    rootParams[GRP_CameraConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_CameraConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MeshConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MeshConstants].Descriptor.ShaderRegister = 0;
    rootParams[GRP_MeshConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_MeshConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_LightInfo].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_LightInfo].Descriptor.ShaderRegister = 1;
    rootParams[GRP_LightInfo].Descriptor.RegisterSpace = 0;
    rootParams[GRP_LightInfo].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowMap].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[GRP_ShadowMap].Constants.Num32BitValues = 1;
    rootParams[GRP_ShadowMap].Constants.ShaderRegister = 2;
    rootParams[GRP_ShadowMap].Constants.RegisterSpace = 0;
    rootParams[GRP_ShadowMap].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_ShadowTransform].Descriptor.ShaderRegister = 3;
    rootParams[GRP_ShadowTransform].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ShadowTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ObjectConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_ObjectConstants].Descriptor.ShaderRegister = 2;
    rootParams[GRP_ObjectConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ObjectConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MaterialSRV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MaterialSRV].Descriptor.ShaderRegister = 3;
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

    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mShadowPipelineState.GetAddressOf())));

    psoDesc.PS = GetShaderByteCode(EShaderType::PS);
    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

    Console.InfoLog("Normal TexturedObject Shader 가 성공적으로 로딩되었습니다.");
}

NormalTexturedObjectShader::~NormalTexturedObjectShader()
{
}







//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Bounding Box Shader							//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

BoundingBoxShader::BoundingBoxShader(ComPtr<ID3D12Device>& device)
    : GraphicsShaderBase(device)
{
    mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_tangent | VertexAttrib_bitangent | VertexAttrib_texcoord1;

    MakeShader(EShaderType::VS, "BoundingBox.hlsl", "BoundingBoxVS", "vs_5_1", nullptr);
	MakeShader(EShaderType::GS, "BoundingBox.hlsl", "BoundingBoxGS", "gs_5_1", nullptr);
    MakeShader(EShaderType::PS, "BoundingBox.hlsl", "BoundingBoxPS", "ps_5_1", nullptr);

    // 애는 하나만씀. 
    D3D12_DESCRIPTOR_RANGE texRange[3];
    // Tex2D 
    texRange[0].BaseShaderRegister = 4;
    texRange[0].NumDescriptors = 1024;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[0].RegisterSpace = 0;
    // Tex2DArray
    texRange[1].BaseShaderRegister = 4;
    texRange[1].NumDescriptors = 512;
    texRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[1].RegisterSpace = 1;
    // TexCube 
    texRange[2].BaseShaderRegister = 4;
    texRange[2].NumDescriptors = 512;
    texRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[2].RegisterSpace = 2;


    D3D12_ROOT_PARAMETER rootParams[GRP_END]{};

    rootParams[GRP_CameraConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_CameraConstants].Descriptor.ShaderRegister = 1;
    rootParams[GRP_CameraConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_CameraConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MeshConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MeshConstants].Descriptor.ShaderRegister = 0;
    rootParams[GRP_MeshConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_MeshConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_LightInfo].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_LightInfo].Descriptor.ShaderRegister = 1;
    rootParams[GRP_LightInfo].Descriptor.RegisterSpace = 0;
    rootParams[GRP_LightInfo].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowMap].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[GRP_ShadowMap].Constants.Num32BitValues = 1;
    rootParams[GRP_ShadowMap].Constants.ShaderRegister = 2;
	rootParams[GRP_ShadowMap].Constants.RegisterSpace = 0;
    rootParams[GRP_ShadowMap].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ShadowTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[GRP_ShadowTransform].Descriptor.ShaderRegister = 3;
    rootParams[GRP_ShadowTransform].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ShadowTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_ObjectConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_ObjectConstants].Descriptor.ShaderRegister = 2;
    rootParams[GRP_ObjectConstants].Descriptor.RegisterSpace = 0;
    rootParams[GRP_ObjectConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[GRP_MaterialSRV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[GRP_MaterialSRV].Descriptor.ShaderRegister = 3;
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
    psoDesc.InputLayout = { nullptr,0 };
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = GetShaderByteCode(EShaderType::VS);
	psoDesc.GS = GetShaderByteCode(EShaderType::GS);
    psoDesc.PS = GetShaderByteCode(EShaderType::PS);
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
    psoDesc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = static_cast<DXGI_FORMAT>(EGlobalConstants::GC_RenderTargetFormat);
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    CheckHR(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

    Console.InfoLog("BoundingBox Shader 가 성공적으로 로딩되었습니다.");
}

BoundingBoxShader::~BoundingBoxShader()
{
}








//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//						Particle SO Shader								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

ParticleSOShader::ParticleSOShader(ComPtr<ID3D12Device>& device)
    : GraphicsShaderBase(device)
{
    // Attribute 생략. 

    MakeShader(EShaderType::VS, "ParticleSOPass.hlsl", "ParticleSOPassVS", "vs_5_1", nullptr);
    MakeShader(EShaderType::GS, "ParticleSOPass.hlsl", "ParticleSOPassGS", "gs_5_1", nullptr);

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // position
        { "WIDTH",              0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // halfWidth
        { "HEIGHT",             0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // height
        { "TEXTUREINDEX",       0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // texture
        { "SPRITABLE",          0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spritable
        { "SPRITEFRAMEINROW",   0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteFrameInRow
        { "SPRITEFRAMEINCOL",   0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteFrameInCol
        { "SPRITEDURATION",     0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteDuration
        { "DIRECTION",          0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "VELOCITY",           0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TOTALLIFETIME",      0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, 
        { "LIFETIME",           0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "PARTICLETYPE",       0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "EMITTYPE",           0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "REMAINEMIT",         0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "PARENTID",           0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "PARENTOFFSET",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
    
    // 텍스쳐 없음. 

    D3D12_ROOT_PARAMETER rootParams[ParticleSORP_END]{};

    rootParams[ParticleSORP_TimeConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[ParticleSORP_TimeConstants].Constants.Num32BitValues = 2;
	rootParams[ParticleSORP_TimeConstants].Constants.ShaderRegister = 0;
    rootParams[ParticleSORP_TimeConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[ParticleSORP_RandomBuffer].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[ParticleSORP_RandomBuffer].Descriptor.ShaderRegister = 0;
    rootParams[ParticleSORP_RandomBuffer].Descriptor.RegisterSpace = 0;
    rootParams[ParticleSORP_RandomBuffer].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[ParticleSORP_ParentPosition].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParams[ParticleSORP_ParentPosition].Descriptor.ShaderRegister = 1;
	rootParams[ParticleSORP_ParentPosition].Descriptor.RegisterSpace = 0;
	rootParams[ParticleSORP_ParentPosition].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC rootDesc{};
    rootDesc.NumParameters = _countof(rootParams);
    rootDesc.pParameters = rootParams;
    rootDesc.NumStaticSamplers = static_cast<UINT>(mStaticSamplers.size());
    rootDesc.pStaticSamplers = mStaticSamplers.data();
    rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;

    ComPtr<ID3DBlob> signature{ nullptr };
    ComPtr<ID3DBlob> error{ nullptr };

    auto hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf());
    if (FAILED(hr)) {
        OutputDebugStringA(static_cast<const char*>(error->GetBufferPointer()));
    }

    CheckHR(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));

    // 파이프라인 상태 객체(PSO) 생성
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = GetShaderByteCode(EShaderType::VS);
    psoDesc.GS = GetShaderByteCode(EShaderType::GS);

    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
    psoDesc.RasterizerState.DepthBias = 0;
    psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
    psoDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
    psoDesc.RasterizerState.DepthClipEnable = TRUE;
    psoDesc.RasterizerState.MultisampleEnable = FALSE;
    psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
    psoDesc.RasterizerState.ForcedSampleCount = 0;
    psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    
    psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
    psoDesc.BlendState.IndependentBlendEnable = FALSE;
    psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
    psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.DepthStencilState.StencilReadMask = 0x00;
    psoDesc.DepthStencilState.StencilWriteMask = 0x00;
    psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
    psoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    psoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    psoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
    
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    psoDesc.NumRenderTargets = 0;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    
    // SO Decl 
    D3D12_SO_DECLARATION_ENTRY soEntries[] = {
        { 0, "POSITION",         0, 0, 3, 0 }, // POSITION,             3 component (float3)
        { 0, "WIDTH",            0, 0, 1, 0 }, // WIDTH,                1 component (float)
        { 0, "HEIGHT",           0, 0, 1, 0 }, // HEIGHT,               1 component (float)
        { 0, "TEXTUREINDEX",     0, 0, 1, 0 }, // TEXTUREINDEX,         1 component (uint)
        { 0, "SPRITABLE",        0, 0, 1, 0 }, // SPRITABLE,            1 component (uint8)
        { 0, "SPRITEFRAMEINROW", 0, 0, 1, 0 }, // SPRITEFRAMEINROW,     1 component (uint)
        { 0, "SPRITEFRAMEINCOL", 0, 0, 1, 0 }, // SPRITEFRAMEINCOL,     1 component (uint)
        { 0, "SPRITEDURATION",   0, 0, 1, 0 }, // SPRITEDURATION,       1 component (float)
        { 0, "DIRECTION",        0, 0, 3, 0 }, // DIRECTION,            3 component (float3)
        { 0, "VELOCITY",         0, 0, 1, 0 }, // VELOCITY,             1 component (float)
        { 0, "TOTALLIFETIME",    0, 0, 1, 0 }, // TOTALLIFETIME         1 component (float) 
        { 0, "LIFETIME",         0, 0, 1, 0 }, // LIFETIME,             1 component (float)
        { 0, "PARTICLETYPE",     0, 0, 1, 0 }, // PARTICLETYPE,         1 component (uint)
        { 0, "EMITTYPE",         0, 0, 1, 0 }, // EMITTYPE,             1 component (uint)
        { 0, "REMAINEMIT",       0, 0, 1, 0 }, // REMAINEMIT,           1 component (uint)     
        { 0, "PARENTID",         0, 0, 1, 0 }, // PARENTID,             1 component (uint)
        { 0, "PARENTOFFSET",     0, 0, 3, 0 }  // PARENTOFFSET,         3 component (float3)
    };

    UINT strides[] = { 
        sizeof(ParticleVertex)
    };
    
    psoDesc.StreamOutput.NumEntries = _countof(soEntries);
	psoDesc.StreamOutput.pSODeclaration = soEntries;
    psoDesc.StreamOutput.NumStrides = 1;
    psoDesc.StreamOutput.pBufferStrides = strides;
	psoDesc.StreamOutput.RasterizedStream = D3D12_SO_NO_RASTERIZED_STREAM;

    CheckHR(device->CreateGraphicsPipelineState(std::addressof(psoDesc), IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

}

ParticleSOShader::~ParticleSOShader()
{
}








//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//						Particle GS Shader								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

ParticleGSShader::ParticleGSShader(ComPtr<ID3D12Device>& device)
    :GraphicsShaderBase(device)
{

    // Attribute 생략. 

    MakeShader(EShaderType::VS, "ParticleGSPass.hlsl", "ParticleGSPassVS", "vs_5_1", nullptr);
    MakeShader(EShaderType::GS, "ParticleGSPass.hlsl", "ParticleGSPassGS", "gs_5_1", nullptr);
    MakeShader(EShaderType::PS, "ParticleGSPass.hlsl", "ParticleGSPassPS", "ps_5_1", nullptr);


    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // position
        { "WIDTH",              0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // halfWidth
        { "HEIGHT",             0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // height
        { "TEXTUREINDEX",       0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // texture
        { "SPRITABLE",          0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spritable
        { "SPRITEFRAMEINROW",   0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteFrameInRow
        { "SPRITEFRAMEINCOL",   0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteFrameInCol
        { "SPRITEDURATION",     0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // spriteDuration
        { "DIRECTION",          0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "VELOCITY",           0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TOTALLIFETIME",      0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "LIFETIME",           0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "PARTICLETYPE",       0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "EMITTYPE",           0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "REMAINEMIT",         0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "PARENTID",           0, DXGI_FORMAT_R32_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "PARENTOFFSET",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
    
    D3D12_DESCRIPTOR_RANGE texRange[1];
    // Tex2D 
    texRange[0].BaseShaderRegister = 0;
    texRange[0].NumDescriptors = 1024;
    texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    texRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange[0].RegisterSpace = 0;


    D3D12_ROOT_PARAMETER rootParams[ParticleGSRP_END]{};

	rootParams[ParticleGSRP_CameraConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[ParticleGSRP_CameraConstants].Descriptor.ShaderRegister = 0;
	rootParams[ParticleGSRP_CameraConstants].Descriptor.RegisterSpace = 0;
	rootParams[ParticleGSRP_CameraConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[ParticleGSRP_TimeConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParams[ParticleGSRP_TimeConstants].Constants.Num32BitValues = 1;
	rootParams[ParticleGSRP_TimeConstants].Constants.ShaderRegister = 1;
	rootParams[ParticleGSRP_TimeConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[ParticleGSRP_Texture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[ParticleGSRP_Texture].DescriptorTable.NumDescriptorRanges = _countof(texRange);
	rootParams[ParticleGSRP_Texture].DescriptorTable.pDescriptorRanges = texRange;
	rootParams[ParticleGSRP_Texture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC rootDesc{};
    rootDesc.NumParameters = _countof(rootParams);
    rootDesc.pParameters = rootParams;
    rootDesc.NumStaticSamplers = static_cast<UINT>(mStaticSamplers.size());
    rootDesc.pStaticSamplers = mStaticSamplers.data();
    rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature{ nullptr };
    ComPtr<ID3DBlob> error{ nullptr };

    auto hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf());
    if (FAILED(hr)) {
        OutputDebugStringA(static_cast<const char*>(error->GetBufferPointer()));
    }

    CheckHR(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));

    // 파이프라인 상태 객체(PSO) 생성
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = GetShaderByteCode(EShaderType::VS);
    psoDesc.GS = GetShaderByteCode(EShaderType::GS);
	psoDesc.PS = GetShaderByteCode(EShaderType::PS);
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
    psoDesc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };

    psoDesc.BlendState.AlphaToCoverageEnable = false;
    psoDesc.BlendState.IndependentBlendEnable = false;
    psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = static_cast<DXGI_FORMAT>(EGlobalConstants::GC_RenderTargetFormat);
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;


    CheckHR(device->CreateGraphicsPipelineState(std::addressof(psoDesc), IID_PPV_ARGS(mRenderPipelineState.GetAddressOf())));

}

ParticleGSShader::~ParticleGSShader()
{
}
