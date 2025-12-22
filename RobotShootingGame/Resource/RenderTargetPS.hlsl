struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer lightBuffer : register(b0)
{
    float3 lightDirection : packoffset(c0);
    float4 lightColor : packoffset(c1);
};

SamplerState smp : register(s0);

Texture2D<float> depthTex : register(t0);
Texture2D<float4> albedoTex : register(t1);
Texture2D<float4> normalTex : register(t2);
Texture2D<float4> worldPosTex : register(t3);

float4 main(VSOutput input) : SV_TARGET
{
    float4 albedo = albedoTex.Sample(smp, input.uv);
    float4 normal = normalTex.Sample(smp, input.uv);
    normal.xyz = normalize(normal.xyz * 2.0f - 1.0f); // 法線ベクトルを[-1,1]範囲に変換
    float4 worldPos = worldPosTex.Sample(smp, input.uv);
    
    float d = depthTex.Sample(smp, input.uv);
    
    return float4(d, d, d, 1.0f); // デプス値をグレースケールで出力
    
    if (albedo.a < 1.0f)
    {
        discard; // 背景を透過させる(処理しない)
    }
    
    float NdotL = saturate(dot(normal.xyz, -lightDirection));
    
    float3 diffuse = lightColor.rgb * NdotL;
    
    float3 finalColor = albedo.rgb * diffuse;
    
    return float4(finalColor, 1.0f);
}
