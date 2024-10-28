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


[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.f)]
Terrain_HS_OUT TerrainHS(InputPatch<Terrain_VS_OUT, 3> input, uint i : SV_OutputControlPointID)
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
    float fTessEdges[3] : SV_TessFactor;
    float fTessInsides : SV_InsideTessFactor;
};


static const float tessellationNear = 8.f; // 근접 거리에서의 최대 테셀레이션 팩터
static const float tessellationFar = 1.f; // 원거리에서의 최소 테셀레이션 팩터
static const float tessellationDistance = 200.f; // 테셀레이션을 보간할 거리 범위

float CalculateTessFactor(float3 p)
{
    float DistanceToCamera = distance(p, cameraPos);
        
    
    // 거리에 따라 테셀레이션 팩터 보간
    float factor = lerp(tessellationNear, tessellationFar, saturate((DistanceToCamera - tessellationNear) / tessellationDistance));

    return factor;
    
}

HSC_OUTPUT ConstantHS(InputPatch<Terrain_VS_OUT, 3> input, uint nPatchID : SV_PrimitiveID)
{
    HSC_OUTPUT output = (HSC_OUTPUT) 0;

    output.fTessEdges[0] = CalculateTessFactor(input[0].Pos);
    output.fTessEdges[1] = CalculateTessFactor(input[1].Pos);
    output.fTessEdges[2] = CalculateTessFactor(input[2].Pos);

    float3 c = (input[0].Pos + input[1].Pos + input[2].Pos ) / 3.0f;
    output.fTessInsides = CalculateTessFactor(c);

    
    return (output);
}

[domain("tri")]
Terrain_DS_OUT TerrainDS(HSC_OUTPUT input, float3 location : SV_DomainLocation, OutputPatch<Terrain_HS_OUT, 3> patch)
{
    Terrain_DS_OUT output = (Terrain_DS_OUT) 0;
    
    float3 pos = patch[0].Pos * location[0] + patch[1].Pos * location[1] + patch[2].Pos * location[2];
    

    output.Pos = mul(float4(pos, 1.0f), viewProjectionMatrix);
    output.Tex1 = patch[0].Tex1 * location[0] + patch[1].Tex1 * location[1] + patch[2].Tex1 * location[2];
    output.Tex2 = patch[0].Tex2 * location[0] + patch[1].Tex2 * location[1] + patch[2].Tex2 * location[2];
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