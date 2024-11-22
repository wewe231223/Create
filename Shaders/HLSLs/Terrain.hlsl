#include "Params.hlsl"
#include "Common.hlsl"

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
    float4  PosH      : SV_POSITION;
    float3  PosW      : POSITION;
    nointerpolation uint MateialID : TEXCOORD0;
    float2  Tex1     : TEXCOORD1;
    float2  Tex2     : TEXCOORD2;
    float3  Normal   : NORMAL;
};

Terrain_VS_OUT TerrainVS(Terrain_VS_IN input)
{
    Terrain_VS_OUT output = (Terrain_VS_OUT) 0;


    output.PosW = mul(float4(input.Pos, 1.0f), gObjects[input.InstanceID].worldMatrix).xyz;
    output.PosW = mul(float4(output.PosW, 1.0f), viewMatrix);
    output.PosH = mul(float4(output.PosW, 1.0f), projectionMatrix);
    
    // Pass through other data.
    output.Tex1 = input.Tex1;
    output.Tex2 = input.Tex2;
    output.Normal = normalize(mul(float4(input.Normal, 1.0f), gObjects[input.InstanceID].worldMatrix).xyz);
    output.MateialID = gMaterialIndices[input.InstanceID];
    return output;
};


float4 TerrainPS(Terrain_VS_OUT input) : SV_Target
{
    
    float4 baseColor = gTextures[gMaterials[input.MateialID].Textures[0]].Sample(linearWrapSampler, input.Tex1);
    float4 detailColor = gTextures[gMaterials[input.MateialID].Textures[1]].Sample(linearWrapSampler, input.Tex2);
    
    float4 Color = saturate(baseColor * 0.5f + detailColor * 0.5f);
   
   return Fog(Color,input.PosW.z, 50.f, 1000.f);
}