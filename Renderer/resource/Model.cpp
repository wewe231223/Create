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


Model::Model(std::shared_ptr<GraphicsShaderBase> shader)
	: mShader(shader)
{
}

Model::~Model()
{
}



void Model::SetShader(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	mShader->SetShader(commandList);
}

bool Model::CompareShader(const std::shared_ptr<Model>& other) const noexcept
{
	return mShader->GetShaderID() < other->mShader->GetShaderID();
}

void Model::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	commandList->IASetVertexBuffers(0, static_cast<UINT>(mVertexBufferViews.size()), mVertexBufferViews.data());
	commandList->IASetIndexBuffer(&mIndexBufferView);

	// refcount 세야함. 
	for (auto& mesh : mMeshes) {
		mesh->Render(commandList,1);
	}

}


//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Terrain										//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////
#include "resource/TerrainImage.h"

TerrainModel::TerrainModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList,std::shared_ptr<GraphicsShaderBase> terrainShader, std::shared_ptr<TerrainImage> terrainImage,DirectX::SimpleMath::Vector3 scale, MaterialIndex matidx)
	: Model(terrainShader), mTerrainImage(terrainImage), mScale(scale)
{
	mMaterialIndex = matidx;
	mAttribute = VertexAttrib_position | VertexAttrib_normal | VertexAttrib_texcoord1 | VertexAttrib_texcoord2;

	// 서로 맞지 않는 경우 객체 생성이 중단됨. ( 유효하지 않은 객체가 생성됨 ) 
	if (mShader->CheckAttribute(mAttribute)) {
		Console.WarningLog("서로 맞지 않은 셰이더가 연결되었습니다.");
	}
	else {
		TerrainModel::Create(device, commandList);
	}

}

TerrainModel::~TerrainModel()
{

}

// 드디어! 
void TerrainModel::Create(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	std::vector<DirectX::XMFLOAT3> positions{};
	std::vector<DirectX::XMFLOAT3> normals{};
	std::vector<DirectX::XMFLOAT2> texcoords1{};
	std::vector<DirectX::XMFLOAT2> texcoords2{};

	for (float z = 0.f;  z < static_cast<float>(mTerrainImage->GetHeight()); z += 1.f) {
		for (float x = 0.f; x < static_cast<float>(mTerrainImage->GetWidth()); x += 1.f)
		{
			positions.emplace_back(x * mScale.x, mTerrainImage->GetHeight(x, z) * mScale.y, z * mScale.z);
			normals.emplace_back(mTerrainImage->GetNormal(static_cast<int>(x),static_cast<int>(z), mScale));
			texcoords1.emplace_back(x / static_cast<float>(mTerrainImage->GetWidth()),1.f -  z / static_cast<float>(mTerrainImage->GetHeight()));
			texcoords2.emplace_back(x / mScale.x * 1.5f, z / mScale.z * 1.5f);
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


	mVertexBuffers[0]	= std::make_unique<DefaultBuffer>(device, commandList,reinterpret_cast<void*>(positions.data()), sizeof(DirectX::XMFLOAT3) * positions.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[0]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * positions.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[1]	= std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(normals.data()), sizeof(DirectX::XMFLOAT3) * normals.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[1]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * normals.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT3)));

	mVertexBuffers[2]	= std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(texcoords1.data()), sizeof(DirectX::XMFLOAT2) * texcoords1.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[2]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT2) * texcoords1.size()),static_cast<UINT>(sizeof(DirectX::XMFLOAT2)));

	mVertexBuffers[3]	= std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(texcoords2.data()), sizeof(DirectX::XMFLOAT2) * texcoords2.size());
	mVertexBufferViews.emplace_back(mVertexBuffers[3]->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(DirectX::XMFLOAT2) * texcoords2.size()), static_cast<UINT>(sizeof(DirectX::XMFLOAT2)));

	mIndexBuffer		= std::make_unique<DefaultBuffer>(device, commandList, reinterpret_cast<void*>(indices.data()), sizeof(UINT) * indices.size());
	mIndexBufferView = { mIndexBuffer->GetBuffer()->GetGPUVirtualAddress(), static_cast<UINT>(sizeof(UINT) * indices.size()), DXGI_FORMAT_R32_UINT };

	mMeshes.emplace_back(std::make_unique<Mesh>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 0, static_cast<UINT>(indices.size()), mMaterialIndex));
}
