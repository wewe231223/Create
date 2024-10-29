#include "Params.hlsl"

struct Terrain_VS_IN
{
    float3  Pos      : POSITION;
    float2  Tex1     : TEXCOORD0;
    float2  Tex2     : TEXCOORD1;

    uint    InstanceID : SV_InstanceID;
};

struct Terrain_VS_OUT
{
    float4  Pos      : POSITION;
    nointerpolation uint MateialID : TEXCOORD0;
    float2  Tex1     : TEXCOORD1;
    float2  Tex2     : TEXCOORD2;

};

struct Terrain_HS_OUT
{
    float4 Pos : POSITION;
    nointerpolation uint MaterialID : TEXCOORD0;
    float2 Tex1 : TEXCOORD1;
    float2 Tex2 : TEXCOORD2;

};

struct Terrain_DS_OUT {
    float4 Pos : SV_POSITION;
    nointerpolation uint MaterialID : TEXCOORD0;
    float2 Tex1 : TEXCOORD1;
    float2 Tex2 : TEXCOORD2;

};


struct HSC_OUTPUT
{
    float fTessEdges[4] : SV_TessFactor;
    float fTessInsides[2] : SV_InsideTessFactor;
};


Terrain_VS_OUT TerrainVS(Terrain_VS_IN input)
{
    Terrain_VS_OUT output = (Terrain_VS_OUT) 0;

    output.Pos = float4(input.Pos, 1.f);
    output.Tex1 = input.Tex1;
    output.Tex2 = input.Tex2;
    output.MateialID = gMaterialIndices[input.InstanceID];
    return output;
};

float CalculateTessFactor(float4 p)
{
    float DistanceToCamera = distance(p.xyz, cameraPos);
    const float d0 = 20.f;
    const float d1 = 100.f;
    
    return clamp(20.f * saturate((d1 - DistanceToCamera) / (d1 - d0)), 3.f, 64.f);
   
}

HSC_OUTPUT ConstantHS(InputPatch<Terrain_VS_OUT, 25> input, uint nPatchID : SV_PrimitiveID)
{
    HSC_OUTPUT output = (HSC_OUTPUT) 0;
    
    float4 e0 = 0.5f * (input[0].Pos + input[4].Pos);
    float4 e1 = 0.5f * (input[0].Pos + input[20].Pos);
    float4 e2 = 0.5f * (input[4].Pos + input[24].Pos);
    float4 e3 = 0.5f * (input[20].Pos + input[24].Pos);
    
    matrix world = gObjects[0].worldMatrix;
    
    
    output.fTessEdges[0] = CalculateTessFactor(mul(e0, world));
    output.fTessEdges[1] = CalculateTessFactor(mul(e1, world));
    output.fTessEdges[2] = CalculateTessFactor(mul(e2, world));
    output.fTessEdges[3] = CalculateTessFactor(mul(e3, world));

    float4 c = (input[0].Pos + input[4].Pos + input[20].Pos * input[24].Pos) * 0.25f;
    output.fTessInsides[0] = CalculateTessFactor(mul(c, world));
    output.fTessInsides[1] = output.fTessInsides[0];

    
    return (output);
}


[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.f)]
Terrain_HS_OUT TerrainHS(InputPatch<Terrain_VS_OUT, 25> input, uint i : SV_OutputControlPointID)
{
    Terrain_HS_OUT output = (Terrain_HS_OUT) 0;
    output.Pos = input[i].Pos;
    output.Tex1 = input[i].Tex1;
    output.Tex2 = input[i].Tex2;
    output.MaterialID = input[i].MateialID;
    
    return output;
}



void BernsteinBasis(float t, out float basis[5])
{
    float invT = 1.0f - t;
    basis[0] = invT * invT  * invT  * invT;
    basis[1] = 4.0f * t     * invT  * invT  * invT;
    basis[2] = 6.0f * t     * t     * invT  * invT;
    basis[3] = 4.0f * t     * t     * t     * invT;
    basis[4] = t    * t     * t     * t;
}

float3 LineBazierSum(OutputPatch<Terrain_HS_OUT, 25> patch, uint index[5], float basis[5])
{
    float3 sum = float3(0.f, 0.f, 0.f);
    for (uint i = 0; i < 5; ++i)
    {
        sum += basis[i] * patch[index[i]].Pos;
        
    }
    return sum;
}

float3 CubicBezierSum(OutputPatch<Terrain_HS_OUT, 25> patch, float basisU[5], float basisV[5])
{
    float sum = float3(0.f, 0.f, 0.f);
    for (uint i = 0; i < 5; ++i)
    {
        uint index[5] = { i * 5, i * 5 + 1, i * 5 + 2, i * 5 + 3, i * 5 + 4 };
        sum += basisV[i] * LineBazierSum(patch, index, basisU);
    }
    return sum;
}



[domain("quad")]
Terrain_DS_OUT TerrainDS(HSC_OUTPUT input, float2 location : SV_DomainLocation,const OutputPatch<Terrain_HS_OUT, 25> patch)
{
    Terrain_DS_OUT output = (Terrain_DS_OUT) 0;
    
    float basisU[5], basisV[5];
    
    BernsteinBasis(location.x, basisU);
    BernsteinBasis(location.y, basisV);
    
    output.Pos = float4(CubicBezierSum(patch, basisU, basisV), 1.f);
  //  output.Pos = mul(mul(output.Pos, gObjects[0].worldMatrix), viewProjectionMatrix);
    output.Pos = mul(output.Pos, viewProjectionMatrix);
    output.Tex1 = lerp(
        lerp(patch[0].Tex1,patch[4].Tex1, location.x),
        lerp(patch[20].Tex1, patch[24].Tex1, location.x),
        location.y
    );
    
    output.Tex2 = lerp(
        lerp(patch[0].Tex2, patch[4].Tex2, location.x),
        lerp(patch[20].Tex2, patch[24].Tex2, location.x),
        location.y
    );
    output.MaterialID = patch[0].MaterialID;
    
    
    return output;
}



float4 TerrainPS(Terrain_DS_OUT input) : SV_Target
{
    
    //float4 baseColor = gTextures[gMaterials[input.MaterialID].Textures[0]].Sample(linearWrapSampler, input.Tex1);
    //float4 detailColor = gTextures[gMaterials[input.MaterialID].Textures[1]].Sample(linearWrapSampler, input.Tex2);
    
    // float4 Color = saturate(baseColor * 0.5f + detailColor * 0.5f) ;
    
    float4 Color = float4(1.f, 1.f, 1.f, 0.f);
    
    return Color;
}