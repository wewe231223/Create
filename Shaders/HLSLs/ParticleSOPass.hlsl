#define ParticleType_emit 0
#define ParticleType_shell 1
#define ParticleType_ember 2

cbuffer GlobalCB : register(b0)
{
    uint globalTime;        // ms 
    uint deltaTime;         // ms 
}

Buffer<float> RandomBuffer : register(t0);
Buffer<float3> ParentPosition : register(t1);

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

struct Particle_GS_IN
{
    float3 position : POSITION;
    float halfWidth : WIDTH;
    float halfHeight : HEIGHT;
    float3 up : UP;
    uint textureIndex : TEXTUREINDEX;
    bool spritable : SPRITABLE;
    uint spriteFrameInRow : SPRITEFRAMEINROW;
    uint spriteFrameINCol : SPRITEFRAMEINCOL;
    float spriteDuration : SPRITEDURATION;
    
    
    float3 direction : DIRECTION;
    float velocity : VELOCITY;
    float lifetime : LIFETIME;
    uint type : PARTICLETYPE;
    
    // 이 아래의 값은 이 파티클이 Emit 파티클일 때에만 의미를 가진다. 
    uint emitType : EMITTYPE;
    uint parentID : PARENTID;
    float3 offset : PARENTOFFSET;
    
};

struct Particle_GS_OUT
{
    float3 pos : POSITION;
};



Particle_GS_IN ParticleSOPassVS(Particle_VS_IN input)
{
    Particle_GS_IN output = (Particle_GS_IN) 0;
    
    output.position = input.position;
    output.halfWidth = input.halfWidth;
    output.halfHeight = input.halfHeight;
    output.up = input.up;
    output.textureIndex = input.textureIndex;
    output.spritable = input.spritable;
    output.spriteFrameInRow = input.spriteFrameInRow;
    output.spriteFrameINCol = input.spriteFrameInCol;
    output.spriteDuration = input.spriteDuration;
    output.direction = input.direction;
    output.velocity = input.velocity;
    output.lifetime = input.lifetime;
    output.type = input.type;
    output.parentID = input.parentID;
    output.offset = input.offset;
    
    return output;
}

[maxvertexcount(40 + 1)]
void ParticleSOPassGS(point Particle_GS_IN input[1], inout PointStream<Particle_GS_OUT> output, uint primitiveID : SV_PrimitiveID)
{
    switch (input[0].type)
    {
        case ParticleType_emit:
            break;
        case ParticleType_shell:
            break;
        case ParticleType_ember:
            break;
    }
    
    Particle_GS_OUT outPoint = (Particle_GS_OUT) 0;
    outPoint.pos = input[0].position * primitiveID;
    
    output.Append(outPoint);
}