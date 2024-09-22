#include "Params.hlsli"

struct Terrain_VS_IN
{
    float3  Pos      : POSITION;
    float2  Tex1     : TEXCOORD0;
    float2  Tex2     : TEXCOORD1;
    float3  Normal   : NORMAL;
    uint    InstanceID : SV_INSTANCEID;
};



struct Terrain_VS_OUT
{
    float4 PosH     : SV_POSITION;
    float2 Tex1     : TEXCOORD0;
    float2 Tex2     : TEXCOORD1;
    float3 Normal   : NORMAL;
    float3 PosW     : POSITION;
};

Terrain_VS_OUT TerrainVS(Terrain_VS_IN input)
{
    Terrain_VS_OUT output = (Terrain_VS_OUT) 0;

    
    
    // Transform to world space.
    float4 posW = float4(input.Pos, 1.0f);
   // output.PosW = mul(posW, gObjects[input.InstanceID].worldMatrix).xyz;
    // Transform to homogeneous clip space.
   // output.PosH = mul(posW, viewProjectionMatrix);
    
    // Pass through other data.
    output.Tex1 = input.Tex1;
    output.Tex2 = input.Tex2;
    output.Normal = input.Normal;

    return output;
};


float4 TerrainPS(Terrain_VS_OUT input) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}