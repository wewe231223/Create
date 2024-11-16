cbuffer CameraCB : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix viewProjectionMatrix;
    float3 cameraPosition;
};

cbuffer GlobalCB : register(b1)
{
    uint globalTime;
}

Texture2D gTextures[1024] : register(t0, space0);

SamplerState pointWrapSampler : register(s0);
SamplerState pointClampSampler : register(s1);
SamplerState linearWrapSampler : register(s2);
SamplerState linearClampSampler : register(s3);
SamplerState anisotropicWrapSampler : register(s4);
SamplerState anisotropicClampSampler : register(s5);

struct BillBoard_VS_IN
{
    float3 position         : POSITION;
    uint halfWidth          : WIDTH;
    uint height             : HEIGHT;
    float3 up               : UP; 
    uint textureIndex       : TEXTUREINDEX;
    bool spritable          : SPRITABLE;
    uint spriteFrameInRow   : SPRITEFRAMEINROW;
    uint spriteFrameInCol   : SPRITEFRAMEINCOLUMN;
    uint spriteDuration    : SPRITEDURATION;
};

struct BillBoard_GS_IN
{
    float3 position : POSITION;
    uint halfWidth : WIDTH;
    uint height : HEIGHT;
    float3 up : UP;
    uint textureIndex : TEXTUREINDEX;
    bool spritable : SPRITABLE;
    uint spriteFrameInRow : SPRITEFRAMEINROW;
    uint spriteFrameInCol : SPRITEFRAMEINCOLUMN;
    uint spriteDuration : SPRITEDURATION;
};

struct BillBoard_PS_IN
{
    float4 positionH : SV_Position;
    float4 positionW : POSITION;
    uint textureIndex : TEXTUREINDEX;
    float2 uv : TEXCOORD;
};

BillBoard_GS_IN BillBoardVS(BillBoard_VS_IN input)
{
    BillBoard_GS_IN output = (BillBoard_GS_IN)0;
    
    output.position = input.position;
    output.halfWidth = input.halfWidth;
    output.height = input.height;
    output.up = input.up;
    output.textureIndex = input.textureIndex;
    output.spritable = input.spritable;
    output.spriteFrameInRow = input.spriteFrameInRow;
    output.spriteFrameInCol = input.spriteFrameInCol;
    output.spriteDuration = input.spriteDuration;
    
    return output; 
}

[maxvertexcount(4)]
void BillBoardGS(point BillBoard_GS_IN input[1], inout TriangleStream<BillBoard_PS_IN> output)
{
    float3 forward,right,up;
    
    if (input[0].up == float3(0.f, 0.f, 0.f))
    {
        forward  = normalize(cameraPosition - input[0].position);
        right    = cross(forward, float3(0.f, 1.f, 0.f));
        up       = cross(forward, right);
    }
    else
    {
        forward = normalize(cameraPosition - input[0].position);
        right = cross(forward, input[0].up);
        up = input[0].up;
    }
    
    float3x3 uvTransform = float3x3(
                                    1.f, 0.f, 0.f, 
                                    0.f, 1.f, 0.f, 
                                    0.f, 0.f, 1.f
                                    );
    
    if (input[0].spritable)
    {
        uint spriteIndex = globalTime % input[0].spriteDuration;
        
        float spriteWidthRatio = 1.f / input[0].spriteFrameInRow;
        float spriteHeightRatio = 1.f / input[0].spriteFrameInCol;
        
        uvTransform = float3x3(
                            spriteWidthRatio,                   0.f,                    (spriteIndex % input[0].spriteFrameInRow) * spriteWidthRatio,
                            0.f,                                spriteHeightRatio,      (spriteIndex / input[0].spriteFrameInCol) * spriteHeightRatio,
                            0.f,                                0.f,                    1.f
                            );
    }
    
    
    float2 uvs[4];
    uvs[0] = float2(0.f, 1.f);
    uvs[1] = float2(0.f, 0.f);
    uvs[2] = float2(1.f, 1.f);
    uvs[3] = float2(1.f, 0.f);
    
    
    float3 positions[4];
    positions[0] = float4(input[0].position + right * input[0].halfWidth - up * input[0].height, 0.f);
    positions[1] = float4(input[0].position + right * input[0].halfWidth + up * input[0].height, 0.f);
    positions[2] = float4(input[0].position - right * input[0].halfWidth - up * input[0].height, 0.f);
    positions[3] = float4(input[0].position - right * input[0].halfWidth + up * input[0].height, 0.f);

    
    BillBoard_PS_IN outpoint;
    
    for(uint i = 0; i < 4; ++i)
    {
        outpoint.positionH = mul(float4(positions[i], 1.f), viewProjectionMatrix);
        outpoint.positionW = float4(positions[i], 1.f);
        outpoint.textureIndex = input[0].textureIndex;
        outpoint.uv = mul(uvTransform, float3(uvs[i], 1.f)).xy;
        output.Append(outpoint);
    }
    
    
}


float4 BillBoardPS(BillBoard_PS_IN input)
{
    return gTextures[input.textureIndex].Sample(pointClampSampler, input.uv);
}