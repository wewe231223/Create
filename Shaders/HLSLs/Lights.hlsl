
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
#define Intensity 1.f


float4 SpotLight(int index, float3 position, float3 normal, float3 toCamera)
{
    // 광원의 위치 및 방향
    float3 lightPos = gLights[index].position; // 광원 위치
    float3 lightDir = normalize(gLights[index].direction); // 광원의 바라보는 방향

    // 픽셀에서 광원으로의 방향 벡터
    float3 toLight = normalize(position - lightPos);
    float distance = length(position - lightPos);

    // 광원의 범위를 초과하면 0 반환
    if (distance > gLights[index].range)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // 스포트라이트 방향 계산 (광원 중심 축과 픽셀 방향 간의 내적)
    float angleBetween = dot(toLight, lightDir); // `toLight`와 `lightDir` 사이의 각도
    float alpha = saturate(angleBetween); // 방향성을 0~1 사이 값으로 제한

    // Spotlight 강도 계산: 내부 및 외부 각도 처리
    float spotFactor = saturate((alpha - gLights[index].externalThetha) /
                                (gLights[index].internalTheta - gLights[index].externalThetha));

    // Falloff을 사용한 스포트라이트 부드럽게 처리
    spotFactor = pow(spotFactor, gLights[index].falloff);

    // 거리 감쇠 계산 (1 / 거리^2)
    float attenuation = 1.0f / dot(gLights[index].attenuation,
                                    float3(1.0f, distance, distance * distance));

    // Ambient 조명
    float3 ambient = gLights[index].ambient.rgb;

    // Diffuse 조명 계산
    float diffuseFactor = max(dot(normal, -toLight), 0.0f);
    float3 diffuse = gLights[index].diffuse.rgb * diffuseFactor;

    // Specular 조명 계산
    float3 halfwayDir = normalize(-toLight + toCamera);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), gLights[index].falloff);
    float3 specular = gLights[index].specular.rgb * specularFactor;

    // 최종 조명 결과 반환 (spotFactor와 거리 감쇠 적용)
    float3 lightColor = (ambient + (diffuse + specular) * spotFactor) * attenuation;
    return float4(lightColor, 1.0f);
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