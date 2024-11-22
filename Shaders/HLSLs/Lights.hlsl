
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
#define Intensity 2.f

float4 SpotLight(int index, float3 position, float3 normal, float3 toCamera)
{
    // Calculate vector to the light and its distance
    float3 toLight = position - gLights[index].position;
    float distance = length(toLight);

    // If the point is outside the light's range, return no contribution
    if (distance > gLights[index].range)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // Normalize the direction to the light
    toLight /= distance;

    // Spotlight effect using the light direction and spot angles
    float alpha = max(dot(toLight, normalize(gLights[index].direction)), 0.0f);

    // Compute the spotlight factor (falloff)
    float spotFactor = saturate((alpha - gLights[index].externalThetha) /
                                (gLights[index].internalTheta - gLights[index].externalThetha));
    spotFactor = pow(spotFactor, gLights[index].falloff);

    // Compute attenuation
    float attenuation = 1.0f / dot(gLights[index].attenuation,
                                    float3(1.0f, distance, distance * distance));

    // Calculate ambient, diffuse, and specular components
    float3 ambient = gLights[index].ambient;

    float diffuseFactor = max(dot(normal, toLight), 0.0f);
    float3 diffuse = gLights[index].diffuse * diffuseFactor ;

    float3 halfwayDir = normalize(toLight + toCamera);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), 0.5f);
    float3 specular = gLights[index].specular * specularFactor * spotFactor;

    // Combine all components and apply attenuation and spotlight factor
    return float4((ambient + diffuse * Intensity + specular * Intensity) * attenuation * spotFactor, 1.0f);
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