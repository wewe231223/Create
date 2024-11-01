#include "Params.hlsl"
#include "Common.hlsl"

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
    float3  PosV     : POSITION;
    nointerpolation uint MaterialID : TEXCOORD0;
    float2  Tex1     : TEXCOORD1;
    float3  Normal   : NORMAL;
};


TexturedObject_VS_OUT TexturedObjectVS(TexturedObject_VS_IN input)
{
    TexturedObject_VS_OUT output = (TexturedObject_VS_OUT) 0;

    output.Pos = mul(float4(input.Pos, 1.0f), mul(gObjects[input.InstanceID].worldMatrix, viewProjectionMatrix));
    
    output.PosV = mul(float4(input.Pos, 1.0f), gObjects[input.InstanceID].worldMatrix).xyz;
    output.PosV = mul(float4(output.PosV, 1.0f), viewMatrix).xyz;
    
    // Pass through other data.
    output.Tex1 = input.Tex1;
    output.Normal = input.Normal;
    output.MaterialID = gMaterialIndices[input.InstanceID];
    
    
    
    return output;
};


float4 TexturedObjectPS(TexturedObject_VS_OUT input) : SV_Target
{
    float4 baseColor = gTextures[gMaterials[input.MaterialID].Textures[0]].Sample(linearClampSampler, input.Tex1);
    
    return Fog(baseColor, input.PosV.z, 50.f, 1000.f);
}
