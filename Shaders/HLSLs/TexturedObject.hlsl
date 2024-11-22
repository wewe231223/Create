#include "Params.hlsl"
#include "Common.hlsl"
#include "Lights.hlsl"

struct TexturedObject_VS_IN
{
    float3  Pos      : POSITION;
    float3  Normal   : NORMAL;
    float2  Tex1     : TEXCOORD0;
    uint    InstanceID : SV_InstanceID;
};

struct TexturedObject_VS_OUT
{
    float4  Pos      : SV_POSITION;
    float3  PosW     : POSITION1;
    float3  PosV     : POSITION2;
    nointerpolation uint MaterialID : TEXCOORD0;
    float2  Tex1     : TEXCOORD1;
    float3  Normal   : NORMAL;
};


TexturedObject_VS_OUT TexturedObjectVS(TexturedObject_VS_IN input)
{
    TexturedObject_VS_OUT output = (TexturedObject_VS_OUT) 0;

    
    
    float4 pos = float4(input.Pos, 1.f);
    
    pos = mul(pos, gObjects[input.InstanceID].worldMatrix);
    output.PosW = pos.xyz;
    pos = mul(pos, viewMatrix);
    output.PosV = pos.xyz;
    output.Pos= mul(pos, projectionMatrix);
    
    
    // Pass through other data.
    output.Tex1 = input.Tex1;
    
    float3x3 normalMatrix = (float3x3) gObjects[input.InstanceID].worldMatrix;
    
    output.Normal = normalize(mul(input.Normal, normalMatrix));
    
    output.MaterialID = gMaterialIndices[input.InstanceID];
    
    
    
    return output;
};


float4 TexturedObjectPS(TexturedObject_VS_OUT input) : SV_Target
{
    float4 baseColor = gTextures[gMaterials[input.MaterialID].Textures[0]].Sample(linearClampSampler, input.Tex1);
    
    return Fog(lerp(baseColor,Lighting(input.PosW,input.Normal),0.5f), input.PosV.z, 50.f, 1000.f);
}
