#include "Params.hlsl"
#include "Common.hlsl"
#include "Lights.hlsl"
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
    float3  PosV      : POSITION1;
    float3  PosW       : POSITION2;
    nointerpolation uint MateialID : TEXCOORD0;
    float2  Tex1     : TEXCOORD1;
    float2  Tex2     : TEXCOORD2;
    float3  Normal   : NORMAL;
};

Terrain_VS_OUT TerrainVS(Terrain_VS_IN input)
{
    Terrain_VS_OUT output = (Terrain_VS_OUT) 0;

    float4 pos = float4(input.Pos, 1.f);
    
    pos = mul(pos, gObjects[input.InstanceID].worldMatrix);
    output.PosW = pos.xyz;
    pos = mul(pos, viewMatrix);
    output.PosV = pos.xyz;
    output.PosH = mul(pos, projectionMatrix);
    
    
    // Pass through other data.
    output.Tex1 = input.Tex1;
    output.Tex2 = input.Tex2;
    
    float3x3 normalMatrix = (float3x3) gObjects[input.InstanceID].worldMatrix;
    output.Normal = normalize(mul(input.Normal, normalMatrix));
    
    output.MateialID = gMaterialIndices[input.InstanceID];
    return output;
};


float4 TerrainPS(Terrain_VS_OUT input) : SV_Target
{
    
    float4 baseColor = gTextures[gMaterials[input.MateialID].Textures[0]].Sample(linearWrapSampler, input.Tex1);
    float4 detailColor = gTextures[gMaterials[input.MateialID].Textures[1]].Sample(linearWrapSampler, input.Tex2);
    
    float4 Color = saturate(baseColor * 0.5f + detailColor * 0.5f);
   
    float depth =  input.PosH.z / input.PosH.w ;
    return float4(float3(depth,depth,depth) , 1.f);
    //return Fog(lerp(Color, Lighting(input.PosW,input.Normal),0.3f), input.PosV.z, 50.f, 1000.f);
}