#define ParticleType_emit 1
#define ParticleType_shell 2
#define ParticleType_ember 3

#define ember_LifeTime 2.f

#define RANDOM_BUFFER_SIZE 4096
#define NULL_PARENT_ID 0xFFFFFFFE
///////////////////////////////////////////////////////////////////////////////
// Particle Type ( TBD ) 
//
// Particle Attribute 
// 0b0000'0000'0000'0000'0000'0000'0000'0001 = Emit 
// 0b0000'0000'0000'0000'0000'0000'0000'0010 = Shell 
// 0b0000'0000'0000'0000'0000'0000'0000'0100 = Ember
//
// Particle 
//
//
///////////////////////////////////////////////////////////////////////////////
cbuffer GlobalCB : register(b0)
{
    float globalTime;        // 초 단위 
    float deltaTime;         // 초 단위 
}


StructuredBuffer<float> RandomBuffer : register(t0);
StructuredBuffer<float3> ParentPosition : register(t1);

struct ParticleVertex
{
    float3 position : POSITION;
    float halfWidth : WIDTH;
    float halfHeight : HEIGHT;
    uint textureIndex : TEXTUREINDEX;
    uint spritable : SPRITABLE;
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

struct ParticleSO_GS_IN
{
    float3 position : POSITION;
    float halfWidth : WIDTH;
    float halfHeight : HEIGHT;
    uint textureIndex : TEXTUREINDEX;
    uint spritable : SPRITABLE;
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
    uint VertexID : VERTEXID;
};


//----------------------------------------------------------[ Random ]----------------------------------------------------------
// 해시 함수
uint FastHash(uint seed)
{
    uint hash = seed * 1665u + 101423u; // 선형 합동 생성기
    return hash ^ (hash >> 16); // 비트 셔플
}

// 랜덤 값 추출 함수
float FetchRandomValue(uint index)
{
    return RandomBuffer[index % 4096]; // 4096으로 나눠 순환
}

// 난수 생성
float GenerateRandom(uint seed)
{
    // 시간을 밀리초 단위로 변환하고 정수화
    uint timeSeed = uint(globalTime * 1000.f);
 //   uint deltaSeed = uint(deltaTime * 1000.f);

    // 시드 결합 후 해시 생성
    uint combinedSeed = FastHash(seed) ^ FastHash(timeSeed);
    return FetchRandomValue(combinedSeed); // [0, 1] 범위의 난수 반환
}

// 특정 범위 내 난수 생성
float GenerateRandomInRange(float min, float max, uint seed)
{
    float randomValue = GenerateRandom(seed);
    return lerp(min, max, randomValue); // [min, max] 범위로 매핑
}

// 랜덤 방향 생성
float3 GenerateRandomDirection(uint seed)
{
    // 두 개의 난수 값 생성
    float rand1 = GenerateRandom(seed); // 첫 번째 난수
    float rand2 = GenerateRandom(seed + 1); // 두 번째 난수

    // [-1, 1] 범위로 변환
    rand1 = rand1 * 2.0 - 1.0;
    rand2 = rand2 * 2.0 - 1.0;

    // 랜덤 방향 계산
    float z = rand1; // -1에서 1까지의 Z 축 값
    float xyMagnitude = sqrt(max(1.0 - z * z, 0.0)); // XY 평면상의 반지름

    // 랜덤 각도 (theta)
    float theta = rand2 * 3.14159; // [0, π] 범위

    float3 direction = float3(
        xyMagnitude * cos(theta), // X 값
        xyMagnitude * sin(theta), // Y 값
        z // Z 값
    );

    return normalize(direction); // 단위 벡터로 정규화
}
//----------------------------------------------------------[ Random ]----------------------------------------------------------

ParticleSO_GS_IN ParticleSOPassVS(ParticleVertex input, uint vertedID : SV_VertexID)
{    
    ParticleSO_GS_IN output = (ParticleSO_GS_IN) 0;
    
    output.position = input.position;
    output.halfWidth = input.halfWidth;
    output.halfHeight = input.halfHeight;
    output.textureIndex = input.textureIndex;
    output.spritable = input.spritable;
    output.spriteFrameInRow = input.spriteFrameInRow;
    output.spriteFrameInCol = input.spriteFrameInCol;
    output.spriteDuration = input.totalLifetime;
    output.direction = input.direction;
    output.velocity = input.velocity;
    output.totalLifetime = input.totalLifetime;
    output.lifetime = input.lifetime;
    output.type = input.type;
    output.emitType = input.emitType;
    output.remainEmit = input.remainEmit;
    output.parentID = input.parentID;
    output.offset = input.offset;
    output.VertexID = vertedID;
    
    return output;
}

void AppendVertex(inout ParticleVertex vertex, inout PointStream<ParticleVertex> stream)
{
    stream.Append(vertex);
}

void EmitParticleUpdate(inout ParticleVertex vertex, uint vertexID, inout PointStream<ParticleVertex> stream)
{
    if (vertex.remainEmit == 0)
    {
        return;
    }
    
    ParticleVertex newParticle = (ParticleVertex) 0;
    if (vertex.lifetime <= 0.f)
    {
        newParticle.position = vertex.position;
        newParticle.halfWidth = vertex.halfWidth;
        newParticle.halfHeight = vertex.halfHeight;
        newParticle.textureIndex = vertex.textureIndex;
        newParticle.spritable = vertex.spritable;
        newParticle.spriteFrameInRow = vertex.spriteFrameInRow;
        newParticle.spriteFrameInCol = vertex.spriteFrameInCol;
        newParticle.spriteDuration = vertex.spriteDuration;
        newParticle.direction = GenerateRandomDirection(vertexID);
        newParticle.velocity = GenerateRandomInRange(5.f, 3.f, vertexID);
        newParticle.totalLifetime = GenerateRandomInRange(3.f, 6.f, vertexID);
        newParticle.lifetime = newParticle.totalLifetime;
        newParticle.type = ParticleType_ember;
        newParticle.emitType = ParticleType_ember;
        newParticle.remainEmit = 0;
        newParticle.parentID = NULL_PARENT_ID;
        newParticle.offset = float3(0.f, 0.f, 0.f);
        
        vertex.lifetime = vertex.totalLifetime;
        vertex.remainEmit--;

        
        AppendVertex(newParticle,stream);
    }
   
    AppendVertex(vertex, stream);
    
}

void EmberParticleUpdate(inout ParticleVertex vertex, inout PointStream<ParticleVertex> stream)
{
    if (vertex.lifetime >= 0.f)
    {
        stream.Append(vertex);
    }
}

[maxvertexcount(16)]
void ParticleSOPassGS(point ParticleSO_GS_IN input[1], inout PointStream<ParticleVertex> output)
{    
    ParticleVertex outPoint = (ParticleVertex)0;
    
    outPoint.position = input[0].position + input[0].direction * input[0].velocity * deltaTime;
    outPoint.halfWidth = input[0].halfWidth;
    outPoint.halfHeight = input[0].halfHeight;
    outPoint.textureIndex = input[0].textureIndex;
    outPoint.spritable = input[0].spritable;
    outPoint.spriteFrameInRow = input[0].spriteFrameInRow;
    outPoint.spriteFrameInCol = input[0].spriteFrameInCol;
    outPoint.spriteDuration = input[0].spriteDuration;
    outPoint.direction = input[0].direction;
    outPoint.velocity = input[0].velocity;
    outPoint.totalLifetime = input[0].totalLifetime;
    outPoint.lifetime = input[0].lifetime - deltaTime;
    outPoint.type = input[0].type;
    outPoint.emitType = input[0].emitType;
    outPoint.remainEmit = input[0].remainEmit;
    outPoint.parentID = input[0].parentID;
    outPoint.offset = input[0].offset;
   
    if (outPoint.type == ParticleType_emit) EmitParticleUpdate(outPoint, input[0].VertexID, output);
    else if (outPoint.type == ParticleType_ember) EmberParticleUpdate(outPoint, output);
}
