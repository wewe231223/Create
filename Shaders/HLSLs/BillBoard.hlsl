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
    uint spriteFrameInCol   : SPRITEFRAMEINCOL;
    uint spriteDuration : SPRITEDURATION;
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
    uint spriteFrameInCol : SPRITEFRAMEINCOL;
    uint spriteDuration : SPRITEDURATION;
};

struct BillBoard_PS_IN
{
    float4 positionH : SV_Position;
    float3 positionW : POSITION;
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

bool CheckZeroVector(float3 v)
{
    return length(v) < 0.0001f;
}

[maxvertexcount(4)]
void BillBoardGS(point BillBoard_GS_IN input[1], inout TriangleStream<BillBoard_PS_IN> output)
{
    float3 forward,right,up;
    
    if (CheckZeroVector(input[0].up))
    {
        forward  = normalize(cameraPosition - input[0].position);
        right = normalize(cross(float3(0.f, 1.f, 0.f), forward));
        up = normalize(cross(forward, right));
    }
    else
    {
        forward = normalize(cameraPosition - input[0].position);
        right = normalize(cross(input[0].up, forward));
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
    uvs[1] = float2(0.f, 0.f);
    uvs[0] = float2(0.f, 1.f);
    uvs[3] = float2(1.f, 0.f);
    uvs[2] = float2(1.f, 1.f);
    
    
    float4 positions[4];
    positions[0] = float4(input[0].position + right * input[0].halfWidth - up * input[0].height, 1.f);
    positions[1] = float4(input[0].position + right * input[0].halfWidth + up * input[0].height, 1.f);
    positions[2] = float4(input[0].position - right * input[0].halfWidth - up * input[0].height, 1.f);
    positions[3] = float4(input[0].position - right * input[0].halfWidth + up * input[0].height, 1.f);

    
    BillBoard_PS_IN outpoint;
    
    for(uint i = 0; i < 4; i++)
    {
        outpoint.positionH = mul(positions[i], viewProjectionMatrix);
        outpoint.positionW = positions[i].xyz;
        outpoint.textureIndex = input[0].textureIndex;
        //outpoint.uv = mul(uvTransform, float3(uvs[i], 1.f)).xy;
        outpoint.uv = uvs[i];
        output.Append(outpoint);
    }
    
    
}

float4 BillBoardPS(BillBoard_PS_IN input) : SV_TARGET
{
    float4 Color = gTextures[input.textureIndex].Sample(linearWrapSampler, input.uv);
 
    if (Color.a < 0.1f)
    {
        discard;
    }
    
    return Color;
}