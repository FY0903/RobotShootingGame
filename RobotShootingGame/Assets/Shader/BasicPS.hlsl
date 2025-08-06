#include "BRDF.hlsli"

#ifndef MIN_DIST
#define MIN_DIST (0.01f)    // 最小距離
#endif

#ifndef OPTIMIZATION
#define OPTIMIZATION (1.0f) // 最適化係数
#endif

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 WorldPos : WORLD_POS;
    float3x3 InvTangentBasis : INV_TANGENT_BASIS; // 接線空間の逆行列
};

struct PSOutput
{
    float4 Color : SV_TARGET0; // 出力カラー
};

cbuffer CbLight : register(b1)
{
    float3 LightPosition : packoffset(c0);
    float LightInvSqrRadius : packoffset(c0.w); // 光源の逆二乗半径
    float3 LightColor : packoffset(c1);
    float LightIntensity : packoffset(c1.w); // 光源の強度
};

cbuffer CbCamera : register(b2)
{
    float3 CameraPosition : packoffset(c0);
}

Texture2D BaseColorMap : register(t0);
SamplerState BaseColorSmp : register(s0);

Texture2D MetallicMap : register(t1);
SamplerState MetallicSmp : register(s1);

Texture2D RoughnessMap : register(t2);
SamplerState RoughnessSmp : register(s2);

Texture2D NormalMap : register(t3);
SamplerState NormalSmp : register(s3);

float SmoothDistanceAttenuation(float squaredDistance, float invSqrAttRadius)
{
    float factor = squaredDistance * invSqrAttRadius;
    float smoothFactor = saturate(1.0f - factor * factor);
    
    return smoothFactor * smoothFactor; // スムーズな減衰
}

#ifndef OPTIMIZATION
// 距離減衰を計算する関数
// unnormalizedLightVector: 光源からの未正規化ベクトル
float GetDistanceAttenuation(float3 unnormalizedLightVector)
{
    float sqrDist = dot(unnormalizedLightVector, unnormalizedLightVector);
    float attenuation = 1.0f / (max(sqrDist, MIN_DIST * MIN_DIST));
    
    return attenuation; // 距離減衰
}

// 点光源の評価
// N: 法線
// worldPos: ワールド空間での位置
// lightPos: 光源の位置
// lightColor: 光源の色
float EvaluatePointLight(float3 N, float3 worldPos, float3 lightPos, float3 lightColor)
{
    float3 dif = lightPos - worldPos;
    float3 L = normalize(dif);
    float att = GetDistanceAttenuation(dif);
    
    return saturate(dot(N, L)) * lightColor * att / (4.0f * F_PI);
}
#else
// 距離減衰を計算する関数
// unnormalizedLightVector: 光源からの未正規化ベクトル
// invSqrAttRadius: 光源の逆二乗半径
float GetDistanceAttenuation(float3 unnormalizedLightVector, float invSqrAttRadius)
{
    float sqrDist = dot(unnormalizedLightVector, unnormalizedLightVector);
    float attenuation = 1.0f / (max(sqrDist, MIN_DIST * MIN_DIST));
    
    // 窓関数によって滑らかにゼロとなるようにする
    attenuation *= SmoothDistanceAttenuation(sqrDist, invSqrAttRadius);
    
    return attenuation; // 距離減衰
}

// 点光源の評価
// N: 法線
// worldPos: ワールド空間での位置
// lightPos: 光源の位置
// lightInvRadiusSq: 光源の逆二乗半径
// lightColor: 光源の色 
float3 EvaluatePointLight(float3 N, float3 worldPos, float3 lightPos, float lightInvRadiusSq, float3 lightColor)
{
    float3 dif = lightPos - worldPos;
    float3 L = normalize(dif);
    float att = GetDistanceAttenuation(dif, lightInvRadiusSq);
    
    return saturate(dot(N, L)) * lightColor * att / (4.0f * F_PI);
}

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput)0;
    
    float3 L = normalize(LightPosition - input.WorldPos);
    float3 V = normalize(CameraPosition - input.WorldPos);
    float3 H = normalize(V + L);
    float3 N = NormalMap.Sample(NormalSmp, input.TexCoord).xyz * 2.0f - 1.0f; // 法線マップから法線を取得
    N = mul(input.InvTangentBasis, N); // 接線空間からワールド空間に変換
    
    float NV = saturate(dot(N, V));
    float NH = saturate(dot(N, H));
    float NL = saturate(dot(N, L));
    
    float3 baseColor = BaseColorMap.Sample(BaseColorSmp, input.TexCoord).rgb;
    float metallic = MetallicMap.Sample(MetallicSmp, input.TexCoord).r;
    float roughness = RoughnessMap.Sample(RoughnessSmp, input.TexCoord).r;
    
    // BRDFの計算
    float Kd = baseColor * (1.0f - metallic);
    float3 diffuse = ComputeLambert(Kd);
    
    // 鏡面反射の計算
    float3 Ks = baseColor * metallic;
    float3 specular = ComputeGGX(Ks, roughness, NH, NV, NL);
    
    // フレネル項の計算
    float3 BRDF = diffuse + specular;
#ifndef OPTIMIZATION
    // 点光源の評価
    float3 lit = EvaluatePointLight(N, input.WorldPos, LightPosition, LightColor) * LightIntensity;
#else
    // 最適化された距離減衰を使用して点光源を評価
    float3 lit = EvaluatePointLight(N, input.WorldPos, LightPosition, LightInvSqrRadius, LightColor) * LightIntensity;
#endif
    
    output.Color.rgb = lit * BRDF;
    output.Color.a = 1.0f;
    
    return output;
}
#endif