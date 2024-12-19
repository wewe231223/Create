#define PARAM

struct Material {
    float4 DiffuseColor;            // 확산 색상
    float4 SpecularColor;           // 반사 색상
    float4 AmbientColor;            // 주변광 색상
    float4 EmissiveColor;           // 자체 발광 색상
    uint    Textures[32];
    float SpecularPower;            // 반사광 세기
    float Shininess;                // 광택
    float Opacity;                  // 불투명도
    float AlphaCutoff;              // 알파 컷오프
    float2 UVScale;                 // UV 스케일
    float2 UVOffset;                // UV 오프셋
    float RefractionIndex;          // 굴절률
    float FresnelEffect;            // 프레넬 효과
    float SubsurfaceScattering;     // 피하 산란 강도
};

#define LIGHT_TYPE_POINT             1
#define LIGHT_TYPE_SPOT              2
#define LIGHT_TYPE_DIRECTIONAL       3
#define MAX_LIGHT                    16

struct LightInfo
{
    float4  diffuse;
    float4  specular;
    float4  ambient;
    float3  position;
    float3  direction;
    float3  attenuation;
    float   falloff;
    float   internalTheta; //cos(m_fTheta)
    float   externalThetha; //cos(m_fPhi)
    bool    enable;
    int     type;
    float   range;
    float   padding;
};

struct ObjectCB
{
    matrix worldMatrix;
    float3 BBExtents;
};

// b1 : Root Descriptor CBV 
cbuffer CameraCB : register(b1)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix viewProjectionMatrix;
    float3 cameraPosition;
};

cbuffer ShadowMapIndex : register(b2)
{
    uint shadowmapIndex;
}

cbuffer ShadowTransform : register(b3)
{
    matrix view;
    matrix proj;
    matrix viewproj;
    float3 campos;
};

// t0 : Root Descriptor Static SRV ( CB )
StructuredBuffer<uint>              gMaterialIndices    : register(t0);
// t1 : Root Descriptor Dynamic SRV ( CB )
StructuredBuffer<LightInfo>         gLights             : register(t1);
// t2 : Root Descriptor Dynamic SRV ( CB ) 
StructuredBuffer<ObjectCB>          gObjects            : register(t2);
// t3 : Root Descriptor Static SRV ( CB )
StructuredBuffer<Material>          gMaterials          : register(t3);
// t4 : Textures
Texture2D                   gTextures[1024]            : register(t4,space0);
Texture2DArray              gTextureArray[512]         : register(t4,space1);   
TextureCube                 gTextureCube[512]          : register(t4,space2);

SamplerState pointWrapSampler : register(s0);
SamplerState pointClampSampler : register(s1);
SamplerState linearWrapSampler : register(s2);
SamplerState linearClampSampler : register(s3);
SamplerState anisotropicWrapSampler : register(s4);
SamplerState anisotropicClampSampler : register(s5);
