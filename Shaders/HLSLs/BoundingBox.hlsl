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

[maxvertexcount(5 + 5 + 2 + 2 + 2 + 2)]
void BoundingBoxGS(point BoundingBox_GS_IN input[1],inout LineStream<BoundingBox_PS_IN> output)
{
    
}



