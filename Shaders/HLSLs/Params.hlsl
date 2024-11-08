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

struct ObjectCB
{
    matrix worldMatrix;
};

// b1 : Root Descriptor CBV 
cbuffer CameraCB : register(b1)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix viewProjectionMatrix;
    float3 cameraPosition;
};

// t0 : Root Descriptor Static SRV ( CB )
StructuredBuffer<uint> gMaterialIndices : register(t0);
// t0 : Root Descriptor Dynamic SRV ( CB ) 
StructuredBuffer<ObjectCB>  gObjects        : register(t1);
// t1 : Root Descriptor Static SRV ( CB )
StructuredBuffer<Material>  gMaterials      : register(t2);
// t2 : Textures
Texture2D                   gTextures[1024]            : register(t3,space0);
Texture2DArray              gTextureArray[512]         : register(t3,space1);   
TextureCube                 gTextureCube[512]          : register(t3,space2);


SamplerState pointWrapSampler : register(s0);
SamplerState pointClampSampler : register(s1);
SamplerState linearWrapSampler : register(s2);
SamplerState linearClampSampler : register(s3);
SamplerState anisotropicWrapSampler : register(s4);
SamplerState anisotropicClampSampler : register(s5);
