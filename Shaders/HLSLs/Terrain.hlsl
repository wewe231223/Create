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
    float3  Pos      : POSITION;
    nointerpolation uint MateialID : TEXCOORD0;
    float2  Tex1     : TEXCOORD1;
    float2  Tex2     : TEXCOORD2;
    float3  Normal   : NORMAL;
};

struct Terrain_HS_OUT
{
    float3 Pos : POSITION;
    nointerpolation uint MaterialID : TEXCOORD0;
    float2 Tex1 : TEXCOORD1;
    float2 Tex2 : TEXCOORD2;
    float3 Normal : NORMAL;
};


Terrain_VS_OUT TerrainVS(Terrain_VS_IN input)
{
    Terrain_VS_OUT output = (Terrain_VS_OUT) 0;

    output.Pos = input.Pos;
    // Pass through other data.
    output.Tex1 = input.Tex1;
    output.Tex2 = input.Tex2;
    output.Normal = input.Normal;
    output.MateialID = gMaterialIndices[input.InstanceID];
    return output;
};


[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(32.f)]
Terrain_HS_OUT TerrainHS(InputPatch<Terrain_VS_OUT, 4> input, uint i : SV_OutputControlPointID)
{
    Terrain_HS_OUT output = (Terrain_HS_OUT) 0;
    output.Pos = input[i].Pos;
    output.Tex1 = input[i].Tex1;
    output.Tex2 = input[i].Tex2;
    output.Normal = input[i].Normal;
    output.MaterialID = input[i].MateialID;
    
    return output;
}


struct Terrain_DS_OUT {
    float4 Pos : SV_POSITION;
    nointerpolation uint MaterialID : TEXCOORD0;
    float2 Tex1 : TEXCOORD1;
    float2 Tex2 : TEXCOORD2;
    float3 Normal : NORMAL;
};


struct HSC_OUTPUT
{
    float fTessEdges[4] : SV_TessFactor;
    float fTessInsides[2] : SV_InsideTessFactor;
};


static const float tessellationNear = 16.f; // 근접 거리에서의 최대 테셀레이션 팩터
static const float tessellationFar = 1.f; // 원거리에서의 최소 테셀레이션 팩터
static const float tessellationDistance = 200.f; // 테셀레이션을 보간할 거리 범위

float CalculateTessFactor(float3 p)
{
    float DistanceToCamera = distance(p, cameraPos);
    // 거리에 따라 테셀레이션 팩터 보간
    // float factor = lerp(tessellationNear, tessellationFar, saturate((DistanceToCamera - tessellationNear) / tessellationDistance));
    float factor = saturate((DistanceToCamera - tessellationNear) / (tessellationDistance - tessellationNear));
    return pow(2, lerp(6.f, 0.f, factor));
   
}

HSC_OUTPUT ConstantHS(InputPatch<Terrain_VS_OUT, 4> input, uint nPatchID : SV_PrimitiveID)
{
    HSC_OUTPUT output = (HSC_OUTPUT) 0;
    
    float3 e0 = 0.5f * (input[0].Pos + input[2].Pos);
    float3 e1 = 0.5f * (input[0].Pos + input[1].Pos);
    float3 e2 = 0.5f * (input[1].Pos + input[3].Pos);
    float3 e3 = 0.5f * (input[2].Pos + input[3].Pos);
    

    output.fTessEdges[0] = CalculateTessFactor(e0);
    output.fTessEdges[1] = CalculateTessFactor(e1);
    output.fTessEdges[2] = CalculateTessFactor(e2);
    output.fTessEdges[3] = CalculateTessFactor(e3);

    float3 c = (input[0].Pos + input[1].Pos + input[2].Pos * input[3].Pos) * 0.25f;
    output.fTessInsides[0] = CalculateTessFactor(c);
    output.fTessInsides[1] = output.fTessInsides[0];

    
    return (output);
}

[domain("quad")]
Terrain_DS_OUT TerrainDS(HSC_OUTPUT input, float2 location : SV_DomainLocation, OutputPatch<Terrain_HS_OUT, 4> patch)
{
    Terrain_DS_OUT output = (Terrain_DS_OUT) 0;
    
    float3 posw = lerp(lerp(patch[0].Pos, patch[1].Pos, location.x), lerp(patch[2].Pos, patch[3].Pos, location.x), location.y);
   
    output.Tex1 = lerp(lerp(patch[0].Tex1, patch[1].Tex1, location.x), lerp(patch[2].Tex1, patch[3].Tex1, location.x), location.y);
    output.Tex2 = lerp(lerp(patch[0].Tex2, patch[1].Tex2, location.x), lerp(patch[2].Tex2, patch[3].Tex2, location.x), location.y);
    
    float height = gTextures[gMaterials[patch[0].MaterialID].Textures[2]].SampleLevel(anisotropicClampSampler, output.Tex1, 0).r * 255.f;
    
    output.Pos = mul(mul(float4(posw.x, height, posw.z, 1.0f), gObjects[0].worldMatrix), viewProjectionMatrix);
    output.MaterialID = patch[0].MaterialID;
    
    return output;
}



float4 TerrainPS(Terrain_DS_OUT input) : SV_Target
{
    
    float4 baseColor = gTextures[gMaterials[input.MaterialID].Textures[0]].Sample(linearWrapSampler, input.Tex1);
    float4 detailColor = gTextures[gMaterials[input.MaterialID].Textures[1]].Sample(linearWrapSampler, input.Tex2);
    
    float4 Color = saturate(baseColor * 0.5f + detailColor * 0.5f) ;
    
    return Color;
}