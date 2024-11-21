#include "params.hlsl"
#include "common.hlsl"
#include "Lights.hlsl"

struct NormalTexturedObject_VS_IN
{
    float3 position     : POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 bitangent    : BINORMAL;
    float2 tex1         : TEXCOORD0;
    uint instanceID     : SV_InstanceID;
};

struct NormalTexturedObject_PS_IN
{
    float4 position     : SV_POSITION;
    float3 positionV    : POSITION1;
    float3 positionW    : POSITION2;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 bitangent    : BINORMAL;
    float2 tex1         : TEXCOORD0;
    nointerpolation uint materialID : TEXCOORD1;
};

NormalTexturedObject_PS_IN NormaledTexturedObjectVS(NormalTexturedObject_VS_IN input)
{
    NormalTexturedObject_PS_IN output = (NormalTexturedObject_PS_IN)0;

    
    float4 pos = float4(input.position, 1.f);
    
    pos                 = mul(pos, gObjects[input.instanceID].worldMatrix);
    output.positionW    = pos.xyz;
    pos                 = mul(pos,viewMatrix);
    output.positionV    = pos.xyz;
    output.position     = mul(pos, projectionMatrix);
    
    output.normal       = normalize(mul(float4(input.normal, 1.f), gObjects[input.instanceID].worldMatrix).xyz);
    output.tangent      = normalize(mul(float4(input.tangent, 1.f), gObjects[input.instanceID].worldMatrix).xyz);
    output.bitangent    = normalize(mul(float4(input.bitangent, 1.f), gObjects[input.instanceID].worldMatrix).xyz);

    // Pass through other data.
    output.tex1 = input.tex1;
    output.materialID = gMaterialIndices[input.instanceID];
    
    return output;
}

float4 NormaledTexturedObjectPS(NormalTexturedObject_PS_IN input) : SV_TARGET
{
    float3 normal = input.normal;
    float3x3 TBN = float3x3(input.tangent, input.bitangent, normal);
    
   
    float3 normalColor = gTextures[gMaterials[input.materialID].Textures[1]].Sample(pointWrapSampler, input.tex1).rgb;
    normalColor = normalize(normalColor.rgb * 2.f - 1.f); // [0, 1] -> [-1, 1]
    
    normal = normalize(mul(normal, TBN));
    
    float4 illumination = float4(1.f, 1.f, 1.f, 1.f);
    illumination = Lighting(input.positionW, normal);
    
    float4 baseColor = gTextures[gMaterials[input.materialID].Textures[0]].Sample(linearClampSampler, input.tex1);
    
    return float4(input.tangent, 1.f);
   // return Fog(lerp(baseColor,illumination,0.3f), input.positionV.z, 50.f, 1000.f);
}
