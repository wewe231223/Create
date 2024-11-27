#include "params.hlsl"

// 이게 가능하다고? 
struct BoundingBox_VS_IN
{
    uint InstanceID : SV_InstanceID;
};

struct BoundingBox_GS_IN
{
    float3 center   : CENTER;
    float3 extents  : EXTENTS;
    float3 forward  : FORWARD;
    float3 right    : RIGHT;
    float3 up       : UP;
};

struct BoundingBox_PS_IN
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

BoundingBox_GS_IN BoundingBoxVS(BoundingBox_VS_IN input)
{
    BoundingBox_GS_IN output = (BoundingBox_GS_IN) 0;
    
    ObjectCB object = gObjects[input.InstanceID];
    
    output.extents = object.BBExtents;
    output.right = normalize(object.worldMatrix[0].xyz);
    output.up = normalize(object.worldMatrix[1].xyz);
    output.forward = normalize(object.worldMatrix[2].xyz);
    output.center = object.worldMatrix[3].xyz;
 
    return output;
}

#define BoundingBoxColor    float4(1.f, 0.f, 0.f, 1.f)
#define ForwardColor        float4(0.f, 0.f, 1.f, 1.f)
#define RightColor          float4(1.f, 1.f, 0.f, 1.f)
#define UpColor             float4(0.f, 1.f, 0.f, 1.f)
#define AxisRenderRatio     1.5f

[maxvertexcount(16)]
void BoundingBoxGS(point BoundingBox_GS_IN input[1],inout LineStream<BoundingBox_PS_IN> output)
{
    float3 center = input[0].center;
    float3 extents = input[0].extents;
    
    float3 TopPoints[4];
    float3 BottomPoints[4];
    // Top face (위쪽 면)
    TopPoints[0]    = center + input[0].forward * extents.z - input[0].right * extents.x + input[0].up * extents.y; // 위 앞 왼쪽 
    TopPoints[1]    = center + input[0].forward * extents.z + input[0].right * extents.x + input[0].up * extents.y; // 위 앞 오른쪽
    TopPoints[2]    = center - input[0].forward * extents.z + input[0].right * extents.x + input[0].up * extents.y; // 위 뒤 오른쪽
    TopPoints[3]    = center - input[0].forward * extents.z - input[0].right * extents.x + input[0].up * extents.y; // 위 뒤 왼쪽
    // Bottom face (아래쪽 면)
    BottomPoints[0] = center + input[0].forward * extents.z - input[0].right * extents.x - input[0].up * extents.y; // 아래 앞 왼쪽
    BottomPoints[1] = center + input[0].forward * extents.z + input[0].right * extents.x - input[0].up * extents.y; // 아래 앞 오른쪽
    BottomPoints[2] = center - input[0].forward * extents.z + input[0].right * extents.x - input[0].up * extents.y; // 아래 뒤 오른쪽
    BottomPoints[3] = center - input[0].forward * extents.z - input[0].right * extents.x - input[0].up * extents.y; // 아래 뒤 왼쪽
    
    
    BoundingBox_PS_IN outpoint;
    outpoint.color = BoundingBoxColor;
    
    // 윗면 엣지들 ( 점 4개 ) 
    [unroll(4)] 
    for (int i = 0; i < 4; ++i)
    {
        outpoint.position = mul(float4(TopPoints[i], 1.f), viewProjectionMatrix);
        output.Append(outpoint);
    }
    //// 위 앞 왼쪽 - 아래 앞 왼쪽 엣지 ( 점 2개 )   
    outpoint.position = mul(float4(TopPoints[0], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    outpoint.position = mul(float4(BottomPoints[0], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    
    // 아랫면 엣지들 ( 점 4개 ) 
    [unroll(4)]
    for (int k = 1; k < 4; ++k)
    {
        outpoint.position = mul(float4(BottomPoints[k], 1.f), viewProjectionMatrix);
        output.Append(outpoint);
    }
    // 아랫면 닫기 ( 점 1개 ) 
    outpoint.position = mul(float4(BottomPoints[0], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    
    //// 별도 엣지들 ( 점 6개 ) 
    
    //// 앞 오른쪽 엣지 
    output.RestartStrip();
    outpoint.position = mul(float4(TopPoints[1], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    outpoint.position = mul(float4(BottomPoints[1], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    
    //// 뒤 왼쪽 엣지
    output.RestartStrip();
    outpoint.position = mul(float4(TopPoints[3], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    outpoint.position = mul(float4(BottomPoints[3], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    
    //// 뒤 오른쪽 엣지 
    output.RestartStrip();
    outpoint.position = mul(float4(TopPoints[2], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    outpoint.position = mul(float4(BottomPoints[2], 1.f), viewProjectionMatrix);
    output.Append(outpoint);
    
    //
    // 총 점의 개수 5 + 5 + 6 = 16개 
    //    
}


float4 BoundingBoxPS (BoundingBox_PS_IN input) : SV_Target
{
    return input.color;
}
