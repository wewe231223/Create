#include "pch.h"
#include "resource/Model.h"
#include "resource/Mesh.h"
#include "buffer/UploadBuffer.h"
#include "buffer/DefaultBuffer.h"
#include "resource/Shader.h"
#include "ui/Console.h"
//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Model Basis									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


Model::Model(ComPtr<ID3D12Device>& device,std::shared_ptr<IGraphicsShader> shader)
	: mShader(shader)
{
	D3D12_RESOURCE_DESC desc{};

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeof(ModelContext) * static_cast<UINT64>(GC_MaxRefPerModel);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);

	for (auto& ContextBuffer : mModelContexts) {
		CheckHR(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(ContextBuffer.mBuffer.GetAddressOf())
			)
		);

		ContextBuffer.mBuffer->Map(0, nullptr, reinterpret_cast<void**>(std::addressof(ContextBuffer.mBufferPtr)));
	}

}

Model::~Model()
{
}

DirectX::BoundingOrientedBox Model::GetBoundingBox() const
{
	return mBoundingBox;
}

void Model::WriteContext(ModelContext* data, const std::span<MaterialIndex>& materials)
{
	memcpy(mModelContexts[mMemoryIndex].mBufferPtr + mInstanceCount, data, sizeof(ModelContext));

	for (auto i = 0; i < mMeshes.size(); ++i) {
		mMeshes[i]->WriteContext(materials[i]);
	}

	mInstanceCount++;
}

/// <summary>
/// 일단 세자. 나중에 MaxRef 가 넘어갈 때, 재할당에 사용해야한다. 
/// </summary>
void Model::AddRef() 
{
	mRefCount++;
}

bool Model::SetShader(ComPtr<ID3D12GraphicsCommandList> commandList, bool shadow)
{
	return mShader->SetShader(commandList, shadow);
}

size_t Model::GetShaderID() const noexcept
{
	return mShader->GetShaderID();
}

bool Model::CompareShaderID(const std::shared_ptr<Model>& other) const noexcept
{
	return mShader->GetShaderID() < other->mShader->GetShaderID();
}



bool Model::CompareEqualShader(const std::shared_ptr<Model>& other) const noexcept
{
	return mShader->GetShaderID() != other->mShader->GetShaderID();
}

void Model::SetModelContext(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (mInstanceCount == 0) {
		return;
	}

	commandList->SetGraphicsRootShaderResourceView(GRP_ObjectConstants, mModelContexts[mMemoryIndex].mBuffer->GetGPUVirtualAddress());
}

void Model::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (mInstanceCount == 0) {
		return;
	}

	commandList->IASetVertexBuffers(0, static_cast<UINT>(mVertexBufferViews.size()), mVertexBufferViews.data());
	commandList->IASetIndexBuffer(&mIndexBufferView);

	for (auto& mesh : mMeshes) {
		mesh->Render(commandList);
	}
	
}

UINT Model::GetInstanceCount() const
{
	return mInstanceCount;
}

void Model::EndRender()
{
	mInstanceCount = 0;
	mMemoryIndex = (mMemoryIndex + 1) % GC_FrameCount;

	for (auto& mesh : mMeshes) {
		mesh->EndRender();
	}

}

void Model::CreateBBFromMeshes(std::vector<DirectX::XMFLOAT3>& positions)
{
	DirectX::BoundingBox box{};
	DirectX::BoundingBox::CreateFromPoints(box, positions.size(), positions.data(), sizeof(DirectX::XMFLOAT3));

	mBoundingBox = DirectX::BoundingOrientedBox{ box.Center,box.Extents,{0.f,0.f,0.f,1.f} };
}


//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//								Terrain									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////
#include "resource/TerrainImage.h"

TerrainModel::TerrainModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList,std::shared_ptr<IGraphicsShader> terrainShader, std::shared_ptr<TerrainImage> terrainImage,DirectX::SimpleMath::Vector3 scale)
	: Model(device,terrainShader), mTerrainImage(terrainImage), mScale(scale)
{
	mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_texcoord1 | VertexAttrib_texcoord2;
	assert(!mShader->CheckAttribute(mAttribute));

	std::vector<DirectX::XMFLOAT3> positions{};
	std::vector<DirectX::XMFLOAT3> normals{};
	std::vector<DirectX::XMFLOAT2> texcoords1{};
	std::vector<DirectX::XMFLOAT2> texcoords2{};

	for (float z = 0.f; z < static_cast<float>(mTerrainImage->GetHeight()); z += 1.f) {
		for (float x = 0.f; x < static_cast<float>(mTerrainImage->GetWidth()); x += 1.f)
		{
			positions.emplace_back(x * mScale.x, mTerrainImage->GetHeight(x, z) * mScale.y, z * mScale.z);
			normals.emplace_back(mTerrainImage->GetNormal(static_cast<int>(x), static_cast<int>(z), mScale));
			texcoords1.emplace_back(x / static_cast<float>(mTerrainImage->GetWidth() - 1), static_cast<float>(mTerrainImage->GetHeight() - 1 - z) / static_cast<float>(mTerrainImage->GetHeight() - 1));
			texcoords2.emplace_back(x / mScale.x * 50.f, z / mScale.z * 50.f);
		}
	}


	std::vector<UINT> indices{};
	UINT width = static_cast<UINT>(mTerrainImage->GetWidth());
	UINT height = static_cast<UINT>(mTerrainImage->GetHeight());

	for (UINT z = 0; z < height - 1; ++z)
	{
		// 홀수 번째 줄 ( 왼쪽 --> 오른쪽 )
		if (z % 2 == 0)
			for (UINT x = 0; x < width; ++x)
			{
				if (x == 0 && z > 0) indices.emplace_back(x + (z * width));
				indices.emplace_back(x + (z * width));
				indices.emplace_back(x + (z * width) + width);
			}
		// 짝수 번째 줄 ( 오른쪽 --> 왼쪽 ) 
		else
			for (int x = static_cast<int>(width - 1); x >= 0; --x)
			{
				UINT ux = static_cast<UINT>(x);
				if (ux == width - 1) indices.emplace_back(ux + (z * width));
				indices.emplace_back(ux + (z * width));
				indices.emplace_back(ux + (z * width) + width);
			}
	}


	mVertexBuffers[0] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(positions.data()), sizeof(DirectX::XMFLOAT3) * positions.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[0]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * positions.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[1] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(normals.data()), sizeof(DirectX::XMFLOAT3) * normals.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[1]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * normals.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[2] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(texcoords1.data()), sizeof(DirectX::XMFLOAT2) * texcoords1.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[2]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT2) * texcoords1.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT2)));

	mVertexBuffers[3] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(texcoords2.data()), sizeof(DirectX::XMFLOAT2) * texcoords2.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[3]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT2) * texcoords2.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT2)));

	mIndexBuffer = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(indices.data()), sizeof(UINT) * indices.size());
	mIndexBufferView = { mIndexBuffer->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(UINT) * indices.size()), DXGI_FORMAT_R32_UINT };

	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 0, static_cast<UINT>(indices.size())));

	Model::CreateBBFromMeshes(positions);
}

TerrainModel::~TerrainModel()
{

}



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Textured Model								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


TexturedModel::TexturedModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<IGraphicsShader> shader, BasicShape shapeType)
	: Model(device,shader)
{
	mShader = shader;
	mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_texcoord1 ;
	assert(!mShader->CheckAttribute(mAttribute));

	std::vector<DirectX::XMFLOAT3> positions{};
	std::vector<DirectX::XMFLOAT3> normals{};
	std::vector<DirectX::XMFLOAT2> texcoords{};
	
	std::vector<UINT> indices{};

	switch (shapeType)
	{
	case TexturedModel::Cube:
		positions = {
			{ -1.0f, -1.0f, -1.0f },{ -1.0f, +1.0f, -1.0f },{ +1.0f, +1.0f, -1.0f },{ +1.0f, -1.0f, -1.0f },
			{ -1.0f, -1.0f, +1.0f },{ -1.0f, +1.0f, +1.0f },{ +1.0f, +1.0f, +1.0f },{ +1.0f, -1.0f, +1.0f }
		};

		normals = {
			{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },
			{ 0.0f, 0.0f, +1.0f },{ 0.0f, 0.0f, +1.0f },{ 0.0f, 0.0f, +1.0f },{ 0.0f, 0.0f, +1.0f }
		};

		texcoords = {
			{ 0.0f, 1.0f },{ 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f },
			{ 1.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f }
		};

		indices = {
			0, 1, 2, 
			0, 2, 3,
			4, 6, 5, 
			4, 7, 6,
			4, 5, 1, 
			4, 1, 0,
			3, 2, 6, 
			3, 6, 7,
			1, 5, 6, 
			1, 6, 2,
			4, 0, 3, 
			4, 3, 7
		};

		break;
	default:
		break;
	}


	mVertexBuffers[0] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(positions.data()), sizeof(DirectX::XMFLOAT3) * positions.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[0]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * positions.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[1] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(normals.data()), sizeof(DirectX::XMFLOAT3) * normals.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[1]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * normals.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[2] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(texcoords.data()), sizeof(DirectX::XMFLOAT2) * texcoords.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[2]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT2) * texcoords.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT2)));

	mIndexBuffer = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(indices.data()), sizeof(UINT) * indices.size());
	mIndexBufferView = { mIndexBuffer->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(UINT) * indices.size()), DXGI_FORMAT_R32_UINT };

	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, static_cast<UINT>(indices.size())));

	Model::CreateBBFromMeshes(positions);
}

TexturedModel::TexturedModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<IGraphicsShader> shader, std::vector<DirectX::XMFLOAT3>& positions, std::vector<DirectX::XMFLOAT3>& norms, std::vector<DirectX::XMFLOAT2>& uvs, std::vector<UINT>& indices)
	: Model(device, shader)
{
	mShader = shader;
	mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_texcoord1;
	assert(!mShader->CheckAttribute(mAttribute));

	mVertexBuffers[0] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(positions.data()), sizeof(DirectX::XMFLOAT3) * positions.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[0]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * positions.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[1] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(norms.data()), sizeof(DirectX::XMFLOAT3) * norms.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[1]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * norms.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[2] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(uvs.data()), sizeof(DirectX::XMFLOAT2) * uvs.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[2]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT2) * uvs.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT2)));

	mIndexBuffer = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(indices.data()), sizeof(UINT) * indices.size());
	mIndexBufferView = { mIndexBuffer->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(UINT) * indices.size()), DXGI_FORMAT_R32_UINT };

	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, static_cast<UINT>(indices.size())));

	Model::CreateBBFromMeshes(positions);
}

TexturedModel::TexturedModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<IGraphicsShader> shader, std::vector<DirectX::XMFLOAT3>& positions, std::vector<DirectX::XMFLOAT3>& norms, std::vector<DirectX::XMFLOAT3>& tangents, std::vector<DirectX::XMFLOAT3>& biTangents, std::vector<DirectX::XMFLOAT2>& uvs, std::vector<UINT>& indices)
	:Model(device, shader)
{
	mShader = shader;
	mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_tangent | VertexAttrib_bitangent | VertexAttrib_texcoord1;
	assert(!mShader->CheckAttribute(mAttribute));

	mVertexBuffers[0] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(positions.data()), sizeof(DirectX::XMFLOAT3) * positions.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[0]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * positions.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[1] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(norms.data()), sizeof(DirectX::XMFLOAT3) * norms.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[1]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * norms.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[2] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(tangents.data()), sizeof(DirectX::XMFLOAT3) * tangents.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[2]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * tangents.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[3] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(biTangents.data()), sizeof(DirectX::XMFLOAT3) * biTangents.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[3]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * biTangents.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[4] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(uvs.data()), sizeof(DirectX::XMFLOAT2) * uvs.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[4]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT2) * uvs.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT2)));

	mIndexBuffer = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(indices.data()), sizeof(UINT) * indices.size());
	mIndexBufferView = { mIndexBuffer->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(UINT) * indices.size()), DXGI_FORMAT_R32_UINT };

	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, static_cast<UINT>(indices.size())));

	Model::CreateBBFromMeshes(positions);
}

TexturedModel::~TexturedModel()
{
}



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							SkyBox Model								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

constexpr float SKYBOX_SIZE = 200.f;

SkyBoxModel::SkyBoxModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<IGraphicsShader> shader)
	: Model(device, shader)
{
	mShader = shader;
	mAttribute = VertexAttrib_position | VertexAttrib_texcoord1;
	assert(!mShader->CheckAttribute(mAttribute));
	
	float size = SKYBOX_SIZE / 2.f ;

	std::vector<DirectX::XMFLOAT3> positions = {
		// Front face
		{-size, -size, -size},
		{-size,  size, -size},
		{ size,  size, -size},
		{ size, -size, -size},

		// Back face
		{ size, -size,  size},
		{ size,  size,  size},
		{-size,  size,  size},
		{-size, -size,  size},

		// Top face
		{-size,  size, -size},
		{-size,  size,  size},
		{ size,  size,  size},
		{ size,  size, -size},

		// Bottom face
		{-size, -size,  size},
		{-size, -size, -size},
		{ size, -size, -size},
		{ size, -size,  size},

		// Left face
		{-size, -size,  size},
		{-size,  size,  size},
		{-size,  size, -size},
		{-size, -size, -size},

		// Right face
		{ size, -size, -size},
		{ size,  size, -size},
		{ size,  size,  size},
		{ size, -size,  size},
	};
	 
	std::vector<DirectX::XMFLOAT2> uvs = {
		// Front face
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		// Back face
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		// Top face
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		// Bottom face
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		// Left face
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		// Right face
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
	};

	std::vector<UINT> indices = {
		// Front face
		0, 2, 1, 0, 3, 2,
		// Back face
		4, 6, 5, 4, 7, 6,
		// Top face
		8, 10, 9, 8, 11, 10,
		// Bottom face
		12, 14, 13, 12, 15, 14,
		// Left face
		16, 18, 17, 16, 19, 18,
		// Right face
		20, 22, 21, 20, 23, 22,
	};


	mVertexBuffers[0] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(positions.data()), sizeof(DirectX::XMFLOAT3) * positions.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[0]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)* positions.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[1] = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(uvs.data()), sizeof(DirectX::XMFLOAT2) * uvs.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[1]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT2)* uvs.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT2)));

	mIndexBuffer = std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(indices.data()), sizeof(UINT) * indices.size());
	mIndexBufferView = { mIndexBuffer->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(UINT) * indices.size()), DXGI_FORMAT_R32_UINT };

	// Front Face
	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 6));
	// Back Face 
	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6, 6));
	// Top Face 
	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 12, 6));
	// Bottom Face 
	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 18, 6));
	// Left Face
	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 24, 6));
	// Right Face 
	mMeshes.emplace_back(std::make_unique<Mesh>(device, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 30, 6));
}


SkyBoxModel::~SkyBoxModel()
{
}
