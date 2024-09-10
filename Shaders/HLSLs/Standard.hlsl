struct VS_IN
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};


struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};


VS_OUT ST_VS_main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    output.position = float4(input.position, 1.0f);
    output.normal = input.normal;
    output.uv = input.uv;
    return output; 
}
