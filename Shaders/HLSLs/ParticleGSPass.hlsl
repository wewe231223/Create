#define ParticleType_emit 1
#define ParticleType_shell 2
#define ParticleType_ember 3

cbuffer CameraCB : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix viewProjectionMatrix;
    float3 cameraPosition;
};

cbuffer GlobalCB : register(b1)
{
    uint globalTime; // 밀리초 단위 
}

Texture2D gTextures[1024] : register(t0, space0);

SamplerState pointWrapSampler : register(s0);
SamplerState pointClampSampler : register(s1);
SamplerState linearWrapSampler : register(s2);
SamplerState linearClampSampler : register(s3);
SamplerState anisotropicWrapSampler : register(s4);
SamplerState anisotropicClampSampler : register(s5);

struct ParticleVertex
{
    float3 position : POSITION;
    float halfWidth : WIDTH;
    float halfHeight : HEIGHT;
    uint textureIndex : TEXTUREINDEX;
    bool spritable : SPRITABLE;
    uint spriteFrameInRow : SPRITEFRAMEINROW;
    uint spriteFrameInCol : SPRITEFRAMEINCOL;
    float spriteDuration : SPRITEDURATION;
    
    float3 direction : DIRECTION;
    float velocity : VELOCITY;
    float totalLifetime : TOTALLIFETIME;
    float lifetime : LIFETIME;
    
    uint type : PARTICLETYPE;
    uint emitType : EMITTYPE;
    uint remainEmit : REMAINEMIT;
    uint parentID : PARENTID;
    float3 offset : PARENTOFFSET;
};

struct Particle_PS_IN
{
    float4 positionH : SV_Position;
    float3 positionV : POSITION;
    uint textureIndex : TEXTUREINDEX;
    float2 uv : TEXCOORD;
    float4 color : Color;
};


ParticleVertex ParticleGSPassVS(ParticleVertex input)
{   
    return input;
}

uint GetSpriteIndex(float TimeSinceStarted, float SpriteDuration, uint TotalSpriteCount)
{
    float frame_duration_ms = (1000.0 * SpriteDuration) / TotalSpriteCount; // 각 그림이 표시되는 시간 (밀리초 단위)
    return ((uint) (TimeSinceStarted / frame_duration_ms) % TotalSpriteCount); // 현재 그림 번호
}


void CreateBillBoard(ParticleVertex vertex,inout TriangleStream<Particle_PS_IN> stream)
{
    float3 forward, right, up;

    
    forward = normalize(cameraPosition - vertex.position);
    right = normalize(cross(float3(0.f, 1.f, 0.f), forward));
    up = normalize(cross(forward, right));
    
    float3x3 uvTransform = float3x3(
                                    1.f, 0.f, 0.f,
                                    0.f, 1.f, 0.f,
                                    0.f, 0.f, 1.f
                                    );
    
    if (vertex.spritable == 1)
    {
        uint spriteIndex = GetSpriteIndex(globalTime, vertex.spriteDuration, vertex.spriteFrameInRow * vertex.spriteFrameInCol);
        
        float spriteWidthRatio = 1.f / vertex.spriteFrameInRow;
        float spriteHeightRatio = 1.f / vertex.spriteFrameInCol;
        
        uvTransform = float3x3(
                            spriteWidthRatio, 0.f, (spriteIndex % vertex.spriteFrameInRow) * spriteWidthRatio,
                            0.f, spriteHeightRatio, (spriteIndex / vertex.spriteFrameInCol) * spriteHeightRatio,
                            0.f, 0.f, 1.f
                            );
    }
    
    
    float2 uvs[4];
    uvs[1] = float2(0.f, 0.f);
    uvs[0] = float2(0.f, 1.f);
    uvs[3] = float2(1.f, 0.f);
    uvs[2] = float2(1.f, 1.f);
    
    
    float4 positions[4];
    positions[0] = float4(vertex.position + right * vertex.halfWidth - up * vertex.halfHeight, 1.f);
    positions[1] = float4(vertex.position + right * vertex.halfWidth + up * vertex.halfHeight, 1.f);
    positions[2] = float4(vertex.position - right * vertex.halfWidth - up * vertex.halfHeight, 1.f);
    positions[3] = float4(vertex.position - right * vertex.halfWidth + up * vertex.halfHeight, 1.f);
   
    Particle_PS_IN outpoint;
    [unroll(4)]
    for (uint i = 0; i < 4; i++)
    {
        outpoint.positionV = mul(positions[i], viewMatrix).xyz;
        outpoint.positionH = mul(positions[i], viewProjectionMatrix);
        outpoint.textureIndex = vertex.textureIndex;
        outpoint.uv = mul(uvTransform, float3(uvs[i], 1.f)).xy;
        
        if (vertex.type == ParticleType_ember)
        {
            outpoint.color = float4(1.f,1.f,1.f,1.f) * (vertex.lifetime / vertex.totalLifetime);
        }
        else
        {
            outpoint.color = float4(1.f, 1.f, 1.f, 1.f);
        }
        
        stream.Append(outpoint);
    }
    
    
}


[maxvertexcount(4)]
void ParticleGSPassGS(point ParticleVertex input[1], inout TriangleStream<Particle_PS_IN> output)
{
    CreateBillBoard(input[0], output);    
}

float4 ParticleGSPassPS(Particle_PS_IN input) : SV_TARGET
{
    float4 Color = gTextures[input.textureIndex].Sample(linearWrapSampler, input.uv);
    
    if (Color.a < 0.25f)
    {
        discard;
    }
    
    
    Color.a *= input.color.a;
    
    
    return Color;
}