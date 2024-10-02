#pragma once 

//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Shader Basis								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


// 셰이더의 기본 틀은 여기서 지원하고, 디테일한 셰이더 구현은 상속을 통해 처리한다. 
class GraphicsShaderBase : public IGraphicsShader {

protected:
	// CS = 0, END = 6 은 별다른 의미가 있는건 아니고 그냥 보기 편하라고 적었음. 
	enum class EShaderType : UINT{
		VS = 0U,
		PS = 1U,
		GS = 2U,
		HS = 3U,
		DS = 4U,
		END = 5U
	};
public:
	GraphicsShaderBase(ComPtr<ID3D12Device>& device);
	~GraphicsShaderBase();

	virtual size_t GetShaderID() const noexcept override;
	virtual void SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList) override;
	bool CheckAttribute(VertexAttribute attribute);
protected:
	void MakeShader(EShaderType shaderType,const std::string& path, const std::string& entryPoint, const std::string& shaderModel,const D3D_SHADER_MACRO* defines = nullptr);
	D3D12_SHADER_BYTECODE GetShaderByteCode(EShaderType shaderType) const noexcept;
private:
	bool GetFileModified(const fs::path& hlslPath,const fs::path& binPath);
	void SaveBlobBinary(const fs::path& path,ComPtr<ID3D10Blob>& blob);
	void StoreShader(EShaderType shaderType,ComPtr<ID3D10Blob>&& blob);
	void DeleteAllBinarys();
	bool CheckParams();
protected:
	std::array<ComPtr<ID3DBlob>, static_cast<size_t>(EShaderType::END)> mShaderBlobs;
	size_t mShaderID{};

	ComPtr<ID3D12PipelineState> mPipelineState;
	ComPtr<ID3D12RootSignature> mRootSignature;

	std::array<CD3DX12_STATIC_SAMPLER_DESC, 6> mStaticSamplers{};

	VertexAttribute mAttribute{ 0b0000'0000'0000'0000 };
};

class ComputeShaderBase {
public:
	ComputeShaderBase();
	~ComputeShaderBase();

	void SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList);
protected:
	void GetShader(const std::string& path, const std::string& entryPoint, const std::string& shaderModel, const D3D_SHADER_MACRO* defines = nullptr);
private:
	bool GetFileModified(const fs::path& hlslPath, const fs::path& binPath);
	void SaveBlobBinary(const fs::path& path, ComPtr<ID3D10Blob>& blob);
protected:
	ComPtr<ID3DBlob> mShaderBlob;

	ComPtr<ID3D12PipelineState> mPipelineState;
	ComPtr<ID3D12RootSignature> mRootSignature;
};




//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Standard Shader								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


class StandardShader : public GraphicsShaderBase {
public:
	StandardShader(ComPtr<ID3D12Device>& device);
	~StandardShader();
};




//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Terrain Shader								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


class TerrainShader : public GraphicsShaderBase {
public:
	TerrainShader(ComPtr<ID3D12Device>& device);
	~TerrainShader();
};



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//						TexturedObject Shader							//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class TexturedObjectShader : public GraphicsShaderBase {
public:
	TexturedObjectShader(ComPtr<ID3D12Device>& device);
	~TexturedObjectShader();
};