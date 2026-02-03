struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer cameraBuffer : register(b0)
{
    float4x4 invV : packoffset(c0);
    float4x4 invP : packoffset(c4);
    float4x4 P : packoffset(c8);
    float3 cameraPos : packoffset(c12);
};

cbuffer LightVP : register(b1)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
};

cbuffer Light : register(b2)
{
    float3 lightPosition : packoffset(c0);
    float lightRange : packoffset(c0.w);
    float3 lightDirection : packoffset(c1);
    float lightAngle : packoffset(c1.w);
    float4 lightColor : packoffset(c2);
};

SamplerState smp : register(s0);
SamplerComparisonState cmpSmp : register(s1);

Texture2D<float4> tex : register(t0);
Texture2D<float> depthTex : register(t1);
Texture2D<float> shadowMap : register(t2);

float4 ReconstructWorldPositionFromDepth(float2 uv, float depth)
{
    float4 proj = float4(uv * 2.0f - 1.0f, depth, 1.0f); // NDC空間の座標に変換
    proj.y = -proj.y; // Y軸を反転（DirectXのNDCは左手系）
    
    float4 viewPos = mul(invP, proj);
    viewPos /= viewPos.w; // 視点座標に変換
    
    float4 worldPos = mul(invV, viewPos);
    
    return worldPos;
}

float4 TransformWorldToShadowCoord(float3 worldPos)
{
    float4 lightViewPos = mul(View, float4(worldPos, 1.0f));
    float4 lightProjPos = mul(Proj, lightViewPos);
    
    // NDC を [0,1] UV に変換
    float2 shadowUV = lightProjPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y;
    
    float writeDepth = lightProjPos.z / lightProjPos.w;
    
    return float4(shadowUV, writeDepth, 1.0f);
}

float Random(float2 seed)
{
    float s = dot(seed, float2(12.9898f, 78.233f));
    return frac(sin(s) * 43758.5453f);
}

float HenyeyGreenstein(float cos, float g)
{
    float g2 = g * g;
    return (1.0f - g2) / (4.0f * 3.14159265f * pow(1.0f + g2 - 2.0f * g * cos, 1.5f));
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 density = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float stepSize = 0.1f;
    float3 fogColor = float3(1.0f, 1.0f, 1.0f);
    float3 ambientFogColor = float3(0.0f, 0.0f, 0.0f);

    float4 albedo = tex.Sample(smp, input.uv);
    float d = depthTex.Sample(smp, input.uv);
    float4 worldPos = ReconstructWorldPositionFromDepth(input.uv, d);
    
    float3 ray = worldPos.xyz - cameraPos;
    float distance = length(ray);
    float3 rayDir = normalize(ray);
    
    float randomOffset = Random(input.uv) * stepSize;
    float currentDistance = 0.0f;
    float totalTransmittance = 1.0f;
    
    for (int i = 0; i < 64; ++i)
    {
        currentDistance += stepSize; // サンプリング間隔
        if (currentDistance > distance)
        {
            break;
        }
        
        float3 rayPos = cameraPos + rayDir * currentDistance;
        
        float stepTramsmittance = exp(-density * stepSize);
        float fogContribution = totalTransmittance * (1.0f - stepTramsmittance);
        
        float4 shadowCoord = TransformWorldToShadowCoord(rayPos);
        
        float shadowAttenuation = shadowMap.SampleCmp(cmpSmp, shadowCoord.xy, shadowCoord.z);
        
        // デバッグでshadowAttenuationを表示する
        albedo.rgb += float3(0.0f, shadowAttenuation, 0.0f) * 0.01f;
        
        //float cos = dot(-rayDir, lightDirection);
        //float phaseFunction = HenyeyGreenstein(cos, 1.0f);
        
        //float3 litColor = fogColor * shadowAttenuation;
        //float3 ambientColor = ambientFogColor * (1.0f - shadowAttenuation);
        //float3 finalFogColor = litColor + ambientColor;
        //albedo.rgb += finalFogColor * fogContribution;
        
        totalTransmittance *= stepTramsmittance;
        if (totalTransmittance < 0.01f)
        {
            break;
        }
    }
    
    return albedo;
}
