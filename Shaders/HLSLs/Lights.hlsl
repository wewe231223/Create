
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
#define Intensity 5.f

//float4 SpotLight(int index, float3 position, float3 normal, float3 toCamera)
//{

//    float3 toLight = position - gLights[index].position;
//    //float3 toLight = gLights[index].direction;
//    float distance = length(toLight);


//    if (distance > gLights[index].range)
//    {
//        return float4(0.0f, 0.0f, 0.0f, 0.0f);
//    }


//    toLight /= distance;

//    float alpha = max(dot(toLight, normalize(gLights[index].direction)), 0.0f);


//    float spotFactor = saturate((alpha - gLights[index].externalThetha) /
//                                (gLights[index].internalTheta - gLights[index].externalThetha));
    
//    spotFactor = pow(spotFactor, gLights[index].falloff);

  
//    float attenuation = 1.0f / dot(gLights[index].attenuation,
//                                    float3(1.0f, distance, distance * distance));


//    float3 ambient = gLights[index].ambient;

//    float diffuseFactor = max(dot(normal, toLight), 0.0f);
//    float3 diffuse = gLights[index].diffuse * diffuseFactor ;

//    float3 halfwayDir = normalize(toLight + toCamera);
//    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), 0.5f);
//    float3 specular = gLights[index].specular * specularFactor * spotFactor;


//    return float4((ambient + diffuse * Intensity + specular * Intensity) * attenuation * spotFactor, 1.0f);
//}


float4 SpotLight(int index, float3 position, float3 normal, float3 toCamera)
{
    // 벡터 계산: 광원 -> 픽셀
    float3 toLight = position - gLights[index].position;
    float distance = length(toLight);

    // 광원이 범위를 초과하면 0을 반환
    if (distance > gLights[index].range)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // Normalize toLight 벡터
    toLight /= distance;

    // 광원의 방향 정규화
    float3 lightDir = normalize(gLights[index].direction);

    // 방향 기반 스포트라이트 계산
    float alpha = max(dot(toLight, lightDir), 0.0f);

    // Spot factor 계산
    float spotFactor = saturate((alpha - gLights[index].externalThetha) /
                                (gLights[index].internalTheta - gLights[index].externalThetha));

    // Falloff 적용
    spotFactor = pow(spotFactor, gLights[index].falloff);

    // 거리 감쇠 계산
    float attenuation = 1.0f / dot(gLights[index].attenuation,
                                    float3(1.0f, distance, distance * distance));

    // Ambient 계산
    float3 ambient = gLights[index].ambient.rgb;

    // Diffuse 계산
    float diffuseFactor = max(dot(normal, toLight), 0.0f);
    float3 diffuse = gLights[index].diffuse.rgb * diffuseFactor;

    // Specular 계산
    float3 halfwayDir = normalize(toLight + toCamera);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), gLights[index].falloff);
    float3 specular = gLights[index].specular.rgb * specularFactor;

    // 최종 조명 출력 (spotFactor와 감쇠 반영)
    return float4((ambient + (diffuse + specular) * spotFactor) , 1.0f);
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