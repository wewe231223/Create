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
	VertexAttrib_end				= 11,						// 12
	// 이 아래로는 특별한 의미를 갖는 Attribute 들만 위치하게 할 것.
	// 이 아래 위치하는 정점 속성들은 일반적인 정점 구조를 가지지 않으며, 하나의 Stride 에 정점에 필요한 모든 정보를 가진다.
	VertexAttrib_BillBoard			= 0b0000'1000'0000'0000,	// Special Attribute 
	VertexAttrib_BB					= 0b0001'0000'0000'0000,
};

enum EGlobalConstants : UINT {
	GC_FrameCount			= 3,
	GC_BackBufferCount		= 3,
	GC_RenderTargetFormat	= static_cast<UINT>(DXGI_FORMAT_R8G8B8A8_UNORM), // DXGI_FORMAT_R8G8B8A8_UNORM
	GC_MaxTextureNumber		= 2048,
	GC_MaxUiImageNumber		= 64,
	GC_MaxRefPerModel		= 1024,
	GC_MaxLight				= 16
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
	TextureIndex Textures[32]					{};
	float SpecularPower							{ 32.f };
	float Shininess								{ 32.f };
	float Opacity								{ 1.f };
	float AlphaCutoff							{ 0.5f };
	DirectX::XMFLOAT2 UVScale					{ 1.f,1.f };
	DirectX::XMFLOAT2 UVOffset					{ 0.f,0.f };
	float RefractionIndex						{ 1.f };
	float FresnelEffect							{ 0.f };
	float SubsurfaceScattering					{ 0.f };

public:
	Material() = default;
	Material(TextureIndex tex) { Textures[0] = tex; }
	Material(TextureIndex tex1, TextureIndex tex2) { Textures[0] = tex1; Textures[1] = tex2; }
	Material(TextureIndex tex1, TextureIndex tex2, TextureIndex tex3) { Textures[0] = tex1; Textures[1] = tex2; Textures[2] = tex3; }
	Material(TextureIndex tex1, TextureIndex tex2, TextureIndex tex3, TextureIndex tex4) { Textures[0] = tex1; Textures[1] = tex2; Textures[2] = tex3; Textures[3] = tex4; }
	Material(TextureIndex tex1, TextureIndex tex2, TextureIndex tex3, TextureIndex tex4, TextureIndex tex5) { Textures[0] = tex1; Textures[1] = tex2; Textures[2] = tex3; Textures[3] = tex4; Textures[4] = tex5; }
};

struct CameraContext {
	DirectX::SimpleMath::Matrix View{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix Projection{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix ViewProjection{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Vector3 CameraPosition{ 0.f,0.f,0.f };
};

struct ModelContext {
	DirectX::SimpleMath::Matrix World{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Vector3 Extents{ 0.f,0.f,0.f };
};

struct ModelContext2D {
	DirectX::XMFLOAT3X3 Transform{ 
		1.f,0.f,0.f,
		0.f,1.f,0.f,
		0.f,0.f,1.f 
	};

	DirectX::XMFLOAT3X3 UVTransform{
		1.f,0.f,0.f,
		0.f,1.f,0.f,
		0.f,0.f,1.f
	};


	UINT ImageIndex{ 0 };
};


// ===============================================================================================================================================
//							BillBoard Vertex ( Geometry Shader / Stream Output ) 
// ===============================================================================================================================================
// 
// 빌보드 정점 4개가 생성되는 기준은 Right 와 UP 이다. 
// 빌보드는 카메라 좌표를 항상 바라보는 개체를 칭하고, 이를 위해 Forward 는 항상 카메라의 위치벡터에서 자신의 위치벡터를 뺀 벡터를 사용한다. 
// 
// 이 때, 빌보드가 바라보는 방법은 UP 에 따라 달라진다. 
//																					계산 순서는 다음과 같다 
// UP 을 설정하는 경우 UP 축을 고정하고, 카메라를 바라본다.		( FORWARD = CAM - POS, UP = UP							, RIGHT = FORWARD x UP )   
// UP 이 영벡터일 경우 항상 완전히 카메라를 바라본다			( FORWARD = CAM - POS, RIGHT = FORWARD x { 0, 1, 0 }	, UP = FORWARD x RIGHT )  
// 
// 또한 다음과 같은 기능을 지원해야 한다. 
// 게임 시작부터 지금까지의 시간을 32bit 상수로 업로드하고, 셰이더에서 이를 활용하여 현재 스프라이트 위치를 결정하는 것.
// 이것은 SO 를 활용한 GPU 파티클 시스템에서도 호환 가능하도록 만들어야 한다. 
// 
// ** 이하의 모든 시간 단위는 초 ( second ) 이며, 해당 자료형은 32 비트 부호 없는 정수로 한다. **
// 
// 필요한 정보 
// bool 스프라이트를 활성화 했는지 
// uint 가로 이미지 개수  
// uint 세로 이미지 개수 
// uint 0번째부터 n 번째 까지 걸리는 시간 ( 초 단위로 한다 )
// 
// GPU 에서는 이를 다음과 같은 방법으로 계산한다. 
// 
// 받아온 시간 값을 확인한다 ( 게임 시작 시 부터 현재까지의 시간 ) 
// 기하 셰이더 입력에, 받아온 시간 값을 현재 빌보드 point 의 0번째부터 N 번째 이미지까지 걸리는 시간으로 나눈 나머지 값을 입력한다. 
// 
// 기하 셰이더에서는 받아온 나머지 값 중, 정수부만을 취하여, 스프라이트 인덱스 ( 현재 몇 번째 이미지여야 하는지 ) 로 삼고, 이를 사용하여 스프라이트 좌표를 계산한다.   
// 이와, 이미지 단위 길이, 이미지 개수 를 사용하여 UV 변환 행렬을 계산한다. 
// 
// UV 변환 행렬은 다음과 같다 
// 
// 
// float 단위 가로 비율		= 1.0f / 이미지 가로 개수
// float 단위 세로 비율		= 1.0f / 이미지 세로 개수
// uint 스프라이트 가로 좌표	= 스프라이트 인덱스 % 이미지 가로 개수
// uint 스프라이트 세로 좌표	= 스프라이트 인덱스 / 이미지 가로 개수
// 
// 
// ** Sprite Index ** 
//		
// 각 그림이 표시되는 시간  = ( 1000.0 * 스프라이트가 표시될 시간 ( ms ) ) / 총 스프라이트 이미지 개수 ( 가로 이미지 개수 x 세로 이미지 개수 ); // 각 그림이 표시되는 시간 (밀리초 단위)
// return ((uint)( 프로그램 시작 시부터 지금까지의 시간 / 각 그림이 표시되는 시간 ) % 총 스프라이트 개수 ); // 현재 그림 번호
// 
// float3x3 UVTransform = 
// -----------------------------------------------------------------------------------------------------------------------
// |		 단위 가로 비율 			|				0.0					|		단위 가로 비율 * 스프라이트 가로 좌표			|	
// -----------------------------------------------------------------------------------------------------------------------
// |			0.0					|			단위 세로 비율				|		단위 세로 비율 * 스프라이트 세로 좌표			|
// -----------------------------------------------------------------------------------------------------------------------
// |			0.0					|				0.0					|					1.0							|
// -----------------------------------------------------------------------------------------------------------------------
// 
// 기하 셰이더의 출력전, ( 0, 0 ), ( 1, 0 ), ( 0, 1 ), ( 1, 1 ) 의 float2 점 4개를 가정한다. 
// 이 4개의 점들과, UVTransform 을 사용하여 텍스쳐 좌표를 변환하고, 이를 출력한다. 
// 
// 파티클이나 빌보드의 종류에 따라 다양한 정보가 필요할 수 있으나, 최대한 GPU 에서 이를 계산하고 생성하는 방법으로 구현한다. 
// 따라서 GS/SO 를 사용하는 모든 파이프라인의 입력 타입은 BillBoardVertex 로 고정한다. 
// 
// 
// ===============================================================================================================================================

// 일단은 빌보드 하나당 하나의 텍스쳐만 쓸 수 있게 하자 
struct BillBoardVertex {
	DirectX::XMFLOAT3 position{ 0.f,0.f,0.f };

	float halfWidth{ 0.f };
	float height{ 0.f };

	DirectX::XMFLOAT3 up{ 0.f,0.f,0.f };

	TextureIndex texture{};
#pragma region Sprite 
	bool spritable{ false };
	UINT spriteFrameInRow{ 0 };
	UINT spriteFrameInCol{ 0 };
	float spriteDuration{ 0 };
#pragma endregion Sprite 
};


enum : UINT {
	ParticleType_emit,
	ParticleType_shell,
	ParticleType_ember
};


// 계층 구조를 따르는 Emit 파티클의 경우, 부모의 정보가 필요하다. 
// 이미 만들어져 있는 월드 행렬에는 위치, 로컬 축 3개가 포함되어 있다. 이정도면 충분. 
// 만약 계층 구조를 이루는 파티클을 만들고 싶은 경우, Transform* 를 받아와서, 부모의 위치를 업데이트 하여 사용한다.
// 부모의 계층만 사용하여 계산하며, offset 을 통해 계층 구조 파티클을 구현한다. 
// 이 때 계층 구조를 가지는 파티클의 개수는 512개로 제한한다. 
// 만약 parentID 가 활성화 되어 있는 경우, 방향&속도 에 따른 위치 계산을 하지 않고, 부모의 위치 + offset 을 위치로 삼는다.
// 이 외의 계산 과정은 동일. 

struct ParticleVertex : public BillBoardVertex {
	DirectX::XMFLOAT3 direction{ 0.f,0.f,0.f };
	float velocity{ 0.f };
	float lifeTime{ 0.f };
	UINT type{};
	
	UINT parentID{ 0xFF'FF'FF'FF };
	DirectX::XMFLOAT3 offset{ 0.f,0.f,0.f };
};

enum : int {
	LightType_Point = 1,
	LightType_Spot = 2,
	LightType_Directional = 3,
};

struct LightInfo
{
	DirectX::XMFLOAT4				diffuse;				// 빛의 난반사(Diffuse Light) 색상 및 강도.
	DirectX::XMFLOAT4				specular;				// 빛의 반사광(Specular Light) 색상 및 강도.	
	DirectX::XMFLOAT4				ambient;				// 빛의 주변광(Ambient Light) 색상 및 강도.
	DirectX::XMFLOAT3				position;				// 빛의 위치.
	DirectX::XMFLOAT3				direction;				// 빛의 방향.
	DirectX::XMFLOAT3				attenuation;			// 빛의 감쇠 계수.
	float 							falloff;				// 빛의 감쇠 정도.
	float 							internalTheta;			// 스포트라이트 내부 각도의 코사인 값. cos 값을 사용하여 계산의 효율성을 높임 
	float							externalPhi;			// 스포트라이트 외부 각도의 코사인 값. cos 값을 사용하여 계산의 효율성을 높임 
	bool							enable;					// 빛의 사용/미사용.	
	int								type;					// 빛의 타입.	
	float							range;					// 빛의 범위.	
	float							padding;				// 명시적 패딩 
};

using Light = std::array<LightInfo, static_cast<size_t>(EGlobalConstants::GC_MaxLight)>::iterator;

enum EGraphicRootParamIndex : UINT {
	GRP_CameraConstants = 0,
	GRP_MeshConstants = 1,
	GRP_LightInfo = 2,
	GRP_ObjectConstants = 3,
	GRP_MaterialSRV = 4,
	GRP_Texture = 5,
	GRP_END = 6,
};

enum UIRootParamIndex : UINT {
	URP_ObjectConstants = 0,
	URP_Texture = 1,
	URP_END = 2
};

enum BillBoardRootParamIndex : UINT {
	BRP_CameraConstants = 0,
	BRP_Time = 1,
	BRP_Texture = 2,
	BRP_END
};


