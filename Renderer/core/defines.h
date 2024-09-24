#pragma once

// Model 에서 지원하는 정점 속성과, Shader 에서 원하는 정점 속성을 다루는 비트 플래그 변수 자료형 
using VertexAttribute = WORD; 

using TextureIndex	= UINT;
using MaterialIndex = UINT;

enum : WORD {
	VertexAttrib_position			= 0b0000'0000'0000'0001,	// 1
	VertexAttrib_normal				= 0b0000'0000'0000'0010,	// 2
	VertexAttrib_texcoord1			= 0b0000'0000'0000'0100,	// 3
	VertexAttrib_texcoord2			= 0b0000'0000'0000'1000,	// 4
	VertexAttrib_texcoord3			= 0b0000'0000'0001'0000,	// 5
	VertexAttrib_texcoord4			= 0b0000'0000'0010'0000,	// 6
	VertexAttrib_tangent			= 0b0000'0000'0100'0000,	// 7
	VertexAttrib_bitangent			= 0b0000'0000'1000'0000,	// 8
	VertexAttrib_color				= 0b0000'0001'0000'0000,	// 9
	VertexAttrib_bone_id			= 0b0000'0010'0000'0000,	// 10
	VertexAttrib_boneweight			= 0b0000'0100'0000'0000,	// 11
	VertexAttrib_end				= 11						// 12
};

// Material 에서 지원하는 상수들을 다루는 구조체.
// 텍스쳐는 인덱스로 대체함. 
// 
// DiffuseColor				: 확산 색상.
// SpecularColor 			: 반사 색상.
// AmbientColor				: 주변광 색상.
// EmissiveColor			: 자체 발광 색상.
// DiffuseTexIndex_1		: 확산 반사 텍스쳐 1.
// DiffuseTexIndex_2		: 확산 반사 텍스쳐 2.
// SpecularTexIndex_1		: 반사 텍스쳐 1.
// SpecularTexIndex_2		: 반사 텍스쳐 2.
// NormalTexIndex			: 노말맵 텍스쳐.
// HeightTexIndex			: 높이맵 텍스쳐.
// RoughnessTexIndex		: 거칠기 텍스쳐.
// MetalicTexIndex			: 금속성 텍스쳐.
// AmbientOcclusionTexIndex	: 주변광 폐색 텍스쳐.
// SpecularPower			: 반사광 세기.
// Shininess				: 광택.
// Opacity					: 불투명도.
// AlphaCutoff				: 알파 컷오프.
// UVScale					: UV 스케일.
// UVOffset					: UV 오프셋.
// RefractionIndex			: 굴절률.
// FresnelEffect			: 프레넬 효과.
// SubsurfaceScattering		: 피하 산란 강도.

/// <summary>
/// 각 값의 의미는 정의부 주석 참고.
/// </summary>
struct Material {
	DirectX::XMFLOAT4 DiffuseColor				{ 0.f,0.f,0.f,0.f };
	DirectX::XMFLOAT4 SpecularColor				{ 0.f,0.f,0.f,0.f };
	DirectX::XMFLOAT4 AmbientColor				{ 0.f,0.f,0.f,0.f };
	DirectX::XMFLOAT4 EmissiveColor				{ 0.f,0.f,0.f,0.f };
	TextureIndex DiffuseTexIndex_1				{ std::numeric_limits<MaterialIndex>::max() };
	TextureIndex DiffuseTexIndex_2				{ std::numeric_limits<MaterialIndex>::max() };
	TextureIndex SpecularTexIndex_1				{ std::numeric_limits<MaterialIndex>::max() };
	TextureIndex SpecularTexIndex_2				{ std::numeric_limits<MaterialIndex>::max() };
	TextureIndex NormalTexIndex					{ std::numeric_limits<MaterialIndex>::max() };
	TextureIndex HeightTexIndex					{ std::numeric_limits<MaterialIndex>::max() };
	TextureIndex RoughnessTexIndex				{ std::numeric_limits<MaterialIndex>::max() };
	TextureIndex MetalicTexIndex				{ std::numeric_limits<MaterialIndex>::max() };
	TextureIndex AmbientOcclusionTexIndex		{ std::numeric_limits<MaterialIndex>::max() };
	float SpecularPower							{ 32.f };
	float Shininess								{ 32.f };
	float Opacity								{ 1.f };
	float AlphaCutoff							{ 0.5f };
	DirectX::XMFLOAT2 UVScale					{ 1.f,1.f };
	DirectX::XMFLOAT2 UVOffset					{ 0.f,0.f };
	float RefractionIndex						{ 1.f };
	float FresnelEffect							{ 0.f };
	float SubsurfaceScattering					{ 0.f };
};

struct CameraContext {
	DirectX::SimpleMath::Matrix View{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix Projection{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix ViewProjection{ DirectX::SimpleMath::Matrix::Identity };
};

struct ModelContext {
	DirectX::SimpleMath::Matrix World{ DirectX::SimpleMath::Matrix::Identity };
	bool Visible{ true };
};


enum EGlobalConstants : UINT {
	GC_FrameCount			= 3,
	GC_BackBufferCount		= 3,
	GC_RenderTargetFormat	= static_cast<UINT>(DXGI_FORMAT_R8G8B8A8_UNORM), // DXGI_FORMAT_R8G8B8A8_UNORM
	GC_MaxTextureNumber		= 2048
};


enum EGraphicRootParamIndex : UINT {
	GRP_MeshConstants = 0,
	GRP_CameraConstants = 1,
	GRP_ObjectConstants = 2,
	GRP_MaterialSRV = 3,
	GRP_Texture = 4,
	GRP_END = 5
};
