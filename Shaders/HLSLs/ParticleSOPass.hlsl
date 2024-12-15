#define ParticleType_emit 0
#define ParticleType_shell 1
#define ParticleType_ember 2

#define ember_LifeTime 2.f

#define RANDOM_BUFFER_SIZE 4096
#define NULL_PARENT_ID 0xFFFFFFFF
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
    uint deltaSeed = uint(deltaTime * 1000.f);

    // 시드 결합 후 해시 생성
    uint combinedSeed = FastHash(seed) ^ FastHash(timeSeed) ^ FastHash(deltaSeed);
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
    float theta = rand2 * 3.14159265; // [0, π] 범위

    float3 direction = float3(
        xyMagnitude * cos(theta), // X 값
        xyMagnitude * sin(theta), // Y 값
        z // Z 값
    );

    return normalize(direction); // 단위 벡터로 정규화
}
//----------------------------------------------------------[ Random ]----------------------------------------------------------

ParticleVertex ParticleSOPassVS(ParticleVertex input)
{    
    return input;
}

[maxvertexcount(3)]
void ParticleSOPassGS(point ParticleVertex input[1], inout PointStream<ParticleVertex> output, uint primitiveID : SV_PrimitiveID)
{    
    ParticleVertex outPoint = input[0];
    // outPoint.lifetime -= deltaTime;
        
    //if (outPoint.parentID & 0xFFFFFFFF == 0xFFFFFFFF)
    //{
    //    outPoint.position += outPoint.direction * outPoint.velocity * deltaTime;
    //}
    //else
    //{
    //    outPoint.position = ParentPosition[outPoint.parentID] + outPoint.offset;
    //}

    output.Append(outPoint);

    
    //if (outPoint.type == ParticleType_emit)
    //{
    //    if (outPoint.lifetime <= 0.f)
    //    {
    //        ParticleVertex newParticle = (ParticleVertex) 0;
    
    //        newParticle.position = outPoint.position;
    //        newParticle.halfWidth = outPoint.halfWidth;
    //        newParticle.halfHeight = outPoint.halfHeight;
    //        float Lifetime = GenerateRandomInRange(0.5f, 1.5f, primitiveID);
    //        newParticle.totalLifetime = Lifetime;
    //        newParticle.lifetime = Lifetime;
    //        newParticle.velocity = GenerateRandomInRange(0.3f, 1.f, primitiveID);
    //        newParticle.textureIndex = outPoint.textureIndex;
    
    //        newParticle.direction = GenerateRandomDirection(primitiveID);
    //        newParticle.type = ParticleType_ember;
    //        newParticle.parentID = 0xFFFFFFFF;
    
    //        output.Append(newParticle);
            
    //        outPoint.remainEmit--;
    //        outPoint.lifetime = outPoint.totalLifetime;
    //    }
    //}
    //else if (outPoint.type == ParticleType_shell)
    //{

    //}
    //else if (outPoint.type == ParticleType_ember)
    //{

    //}


    
 
}