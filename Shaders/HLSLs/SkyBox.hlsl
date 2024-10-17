#include "Params.hlsl"

struct SkyBox_VS_IN
{
    float3  Pos      : POSITION;
    float2  Tex      : TEXCOORD0;
};

struct SkyBox_VS_OUT
{
    float4  Pos      : SV_POSITION;
    float2  Tex      : TEXCOORD0;
};


SkyBox_VS_OUT SkyBoxVS(SkyBox_VS_IN input)
{
    SkyBox_VS_OUT output = (SkyBox_VS_OUT) 0;

    // Skybox is Unique
    output.Pos = mul(float4(input.Pos, 1.0f), mul(gObjects[0].worldMatrix, viewProjectionMatrix));
    output.Tex = input.Tex; 
    return output;
};


float4 SkyBoxPS(SkyBox_VS_OUT input) : SV_TARGET
{
    return gTextures[gMaterials[gMaterialIndices[0]].Textures[0]].Sample(linearClampSampler, input.Tex);
}