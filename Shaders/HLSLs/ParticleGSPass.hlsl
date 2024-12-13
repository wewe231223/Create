cbuffer GlobalCB : register(b0)
{
    uint globalTime;
    uint deltaTime;
}

cbuffer CameraCB : register(b1)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix viewProjectionMatrix;
    float3 cameraPosition;
};


Texture2D gTextures[1024] : register(t0, space0);

SamplerState pointWrapSampler : register(s0);
SamplerState pointClampSampler : register(s1);
SamplerState linearWrapSampler : register(s2);
SamplerState linearClampSampler : register(s3);
SamplerState anisotropicWrapSampler : register(s4);
SamplerState anisotropicClampSampler : register(s5);

struct Particle_VS_IN
{
    float3 position : POSITION;
    float halfWidth : WIDTH;
    float halfHeight : HEIGHT;
    float3 up : UP;
    uint textureIndex : TEXTUREINDEX;
    bool spritable : SPRITABLE;
    uint spriteFrameInRow : SPRITEFRAMEINROW;
    uint spriteFrameInCol : SPRITEFRAMEINCOL;
    float spriteDuration : SPRITEDURATION;
    
    float3 direction : DIRECTION;
    float velocity : VELOCITY;
    float lifetime : LIFETIME;
    
    uint type : PARTICLETYPE;
    uint emitType : EMITTYPE;
    uint parentID : PARENTID;
    float3 offset : PARENTOFFSET;
};

// 좀 줄이자. 
struct Particle_GS_IN
{
    float3 position : POSITION;
    float halfWidth : WIDTH;
    float halfHeight : HEIGHT;
    float3 up : UP;
    uint textureIndex : TEXTUREINDEX;
    bool spritable : SPRITABLE;
    uint spriteFrameInRow : SPRITEFRAMEINROW;
    uint spriteFrameInCol : SPRITEFRAMEINCOL;
    float spriteDuration : SPRITEDURATION;
    
    float3 direction : DIRECTION;
    float velocity : VELOCITY;
    float lifetime : LIFETIME;
    
    uint type : PARTICLETYPE;
    uint emitType : EMITTYPE;
    uint parentID : PARENTID;
    float3 offset : PARENTOFFSET;
};

struct Particle_PS_IN
{
    float4 positionH : SV_Position;
    float3 positionV : POSITION;
    uint textureIndex : TEXTUREINDEX;
    float2 uv : TEXCOORD;
};


Particle_GS_IN ParticleGSPassVS(Particle_VS_IN input)
{
    Particle_GS_IN output = (Particle_GS_IN) 0;
    
    output.position = input.position;
    output.halfWidth = input.halfWidth;
    output.halfHeight = input.halfHeight;
    output.up = input.up;
    output.textureIndex = input.textureIndex;
    output.spritable = input.spritable;
    output.spriteFrameInRow = input.spriteFrameInRow;
    output.spriteFrameInCol = input.spriteFrameInCol;
    output.spriteDuration = input.spriteDuration;
    output.direction = input.direction;
    output.velocity = input.velocity;
    output.lifetime = input.lifetime;
    output.type = input.type;
    output.parentID = input.parentID;
    output.offset = input.offset;
    
    return output;
}

[maxvertexcount(4)]
void ParticleGSPassGS(point Particle_GS_IN input[1], inout TriangleStream<Particle_PS_IN> output)
{
    Particle_PS_IN outPoint = (Particle_PS_IN) 0;
    
    output.Append(outPoint);
    output.Append(outPoint);
    output.Append(outPoint);
    
}

float4 ParticleGSPassPS(Particle_PS_IN input) : SV_TARGET
{
   // return gTextures[input.textureIndex].Sample(linearWrapSampler, input.uv);
    return float4(0.f, 0.f, 0.f, 0.f);
}