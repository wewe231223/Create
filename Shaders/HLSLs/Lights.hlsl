
#ifndef PARAM
#include "params.hlsl"
#endif

float4 DirectionalLight(int index, float3 normal, float3 toCamera)
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}


float4 PointLight(int index, float3 position, float3 normal, float3 toCamera)
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}


float4 SpotLight(int index, float3 position, float3 normal, float3 toCamera)
{
    float3 toLight = position - gLights[index].position;
    float distance = length(toLight);
    
    
    if (distance <= gLights[index].range)
    {
        float fSpecularFactor = 0.0f;
        toLight /= distance;
        
        float alpha = max(dot(toLight, gLights[index].direction), 0.0f);
        float spotFactor = pow(max(((alpha - gLights[index].externalThetha) / (gLights[index].internalTheta - gLights[index].externalThetha)), 0.0f), gLights[index].falloff);

        float attenuantionFactor = 1.0f / dot(gLights[index].attenuation, float3(1.0f, distance, distance * distance));
        
        return ((gLights[index].ambient) + (gLights[index].diffuse * alpha) + (gLights[index].specular * spotFactor)) * attenuantionFactor * spotFactor;
    } 
    return float4(1.0f, 0.0f, 0.0f, 0.0f);
}


float4 Lighting(float3 position, float3 normal)
{
    float3 toCamera = normalize(cameraPosition - position);
    
    float4 color  = float4(0.0f, 0.0f, 0.0f, 1.0f);
    [unroll(MAX_LIGHT)]
    for (int i = 0; i < MAX_LIGHT; ++i)
    {
        if (gLights[i].enable)
        {
            if (gLights[i].type == LIGHT_TYPE_POINT)
            {
                color += PointLight(i, position, normal, toCamera);
            }
            else if (gLights[i].type == LIGHT_TYPE_SPOT)
            {
                color += SpotLight(i, position, normal, toCamera);
            }
            else if (gLights[i].type == LIGHT_TYPE_DIRECTIONAL)
            {
                color += DirectionalLight(i, normal, toCamera);
            }
        }
    }
    
    return color;
}