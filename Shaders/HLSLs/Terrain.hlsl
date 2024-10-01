#include "Params.hlsl"

struct Terrain_VS_IN
{
    float3  Pos      : POSITION;
    float2  Tex1     : TEXCOORD0;
    float2  Tex2     : TEXCOORD1;
    float3  Normal   : NORMAL;
    uint    InstanceID : SV_InstanceID;
};



struct Terrain_VS_OUT
{
    float4 Pos      : SV_POSITION;
    float2 Tex1     : TEXCOORD0;
    float2 Tex2     : TEXCOORD1;
    float3 Normal   : NORMAL;
};

Terrain_VS_OUT TerrainVS(Terrain_VS_IN input)
{
    Terrain_VS_OUT output = (Terrain_VS_OUT) 0;

    float4x4 world = gObjects[input.InstanceID].worldMatrix;
    float4x4 worldviewproj = mul(world, viewProjectionMatrix);
    output.Pos = mul(float4(input.Pos, 1.0f), worldviewproj);
    
    // Pass through other data.
    output.Tex1 = input.Tex1;
    output.Tex2 = input.Tex2;
    output.Normal = input.Normal;
    return output;
};


float4 TerrainPS(Terrain_VS_OUT input) : SV_Target
{
    float4 baseColor = gTextures[gMaterials[materialIndex].DiffuseTexIndex_1].Sample(linearWrapSampler, input.Tex1);
    float4 detailColor = gTextures[gMaterials[materialIndex].DiffuseTexIndex_2].Sample(linearWrapSampler, input.Tex2);
    
    float4 Color = saturate(baseColor * 0.5f + detailColor * 0.5f) ;
    
    return Color;
}