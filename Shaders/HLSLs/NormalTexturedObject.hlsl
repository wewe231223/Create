#include "params.hlsl"
#include "common.hlsl"

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
    float3 positionV    : POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 bitangent    : BINORMAL;
    float2 tex1         : TEXCOORD0;
    nointerpolation uint materialID : TEXCOORD1;
};

NormalTexturedObject_PS_IN NormaledTexturedObjectVS(NormalTexturedObject_VS_IN input)
{
    NormalTexturedObject_PS_IN output = (NormalTexturedObject_PS_IN)0;

    output.position     = mul(float4(input.position, 1.f), gObjects[input.instanceID].worldMatrix);
    output.positionV    = mul(output.position,viewMatrix).xyz;
    output.position     = mul(output.position, viewProjectionMatrix);
    
    output.normal       = normalize(mul(float4(input.normal, 1.f), gObjects[input.instanceID].worldMatrix).xyz);
    output.tangent      = normalize(mul(float4(input.tangent, 1.f), gObjects[input.instanceID].worldMatrix).xyz);
    output.bitangent    = normalize(mul(float4(input.bitangent, 1.f), gObjects[input.instanceID].worldMatrix).xyz);

    // Pass through other data.
    output.tex1 = input.tex1;
    output.materialID = gMaterialIndices[input.instanceID];
    
    return output;
}

float4 NormalTexturedObjectPS(NormalTexturedObject_PS_IN input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 bitangent = normalize(input.bitangent);
    float3 tangent = cross(normal, bitangent);
    
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    float4 baseColor = gTextures[gMaterials[input.materialID].Textures[0]].Sample(linearClampSampler, input.tex1);
    
    return Fog(baseColor, input.positionV.z, 50.f, 1000.f);
}
