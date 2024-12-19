struct UIObjectCB
{
    float3x3 Transform;
    float3x3 UVTransform;
    uint ImageIndex;
    uint GreyScale;
};


StructuredBuffer<UIObjectCB> gUIObjects : register(t0);

Texture2D gUITexture[64] : register(t1);

SamplerState pointWrapSampler : register(s0);
SamplerState pointClampSampler : register(s1);
SamplerState linearWrapSampler : register(s2);
SamplerState linearClampSampler : register(s3);
SamplerState anisotropicWrapSampler : register(s4);
SamplerState anisotropicClampSampler : register(s5);


struct UI_VS_IN
{
    float2  Pos         : POSITION;
    uint  InstanceID : SV_InstanceID;
};

struct UI_VS_OUT
{
    float4  Pos                     : SV_POSITION;
    nointerpolation uint ImageIndex : TEXCOORD0;
    float2  Tex                     : TEXCOORD1;
};


UI_VS_OUT UiVS(UI_VS_IN In)
{
    UI_VS_OUT Out;

    float3 pos = mul(float3(In.Pos, 1.f), gUIObjects[In.InstanceID].Transform);
    Out.Pos = float4(pos.xy,1.f, 1.f);
    Out.ImageIndex = gUIObjects[In.InstanceID].ImageIndex;
    Out.Tex = mul(float3(In.Pos, 1.f), gUIObjects[In.InstanceID].UVTransform).xy;
    return Out;
}

float4 UiPS(UI_VS_OUT In) : SV_TARGET
{
    return gUITexture[In.ImageIndex].Sample( linearWrapSampler, In.Tex);
}