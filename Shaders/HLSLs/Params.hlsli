struct Material {
    float4 DiffuseColor;            // 확산 색상
    float4 SpecularColor;           // 반사 색상
    float4 AmbientColor;            // 주변광 색상
    float4 EmissiveColor;           // 자체 발광 색상
    uint DiffuseTexIndex_1;         // 확산 반사 텍스쳐 1
    uint DiffuseTexIndex_2;         // 확산 반사 텍스쳐 2
    uint SpecularTexIndex_1;        // 반사 텍스쳐 1
    uint SpecularTexIndex_2;        // 반사 텍스쳐 2
    uint NormalTexIndex;            // 노말맵 텍스쳐
    uint HeightTexIndex;            // 높이맵 텍스쳐
    uint RoughnessTexIndex;         // 거칠기 텍스쳐
    uint MetalicTexIndex;           // 금속성 텍스쳐
    uint AmbientOcclusionTexIndex;  // 주변광 폐색 텍스쳐
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


cbuffer CameraCB : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix viewProjectionMatrix;
};

cbuffer ModelCB : register(b1)
{
    uint materialIndex;
};

StructuredBuffer<ObjectCB>  gObjects        : register(t0);
StructuredBuffer<Material>  gMaterials      : register(t1);
Texture2D                   gTextures[]     : register(t2);


SamplerState pointWrapSampler : register(s0);
SamplerState pointClampSampler : register(s1);
SamplerState linearWrapSampler : register(s2);
SamplerState linearClampSampler : register(s3);
SamplerState anisotropicWrapSampler : register(s4);
SamplerState anisotropicClampSampler : register(s5);
