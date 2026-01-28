struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer lightBuffer : register(b0)
{
    float3 lightPosition : packoffset(c0);
    float lightRange : packoffset(c0.w);
    float3 lightDirection : packoffset(c1);
    float lightAngle : packoffset(c1.w);
    float4 lightColor : packoffset(c2);
};

SamplerState smp : register(s0);

Texture2D<float4> albedoTex : register(t0);
Texture2D<float4> normalTex : register(t1);
Texture2D<float4> worldPosTex : register(t2);

float4 main(VSOutput input) : SV_TARGET
{
    float4 albedo = albedoTex.Sample(smp, input.uv);
    float4 normal = normalTex.Sample(smp, input.uv);
    normal.xyz = normalize(normal.xyz * 2.0f - 1.0f); // 法線ベクトルを[-1,1]範囲に変換
    
    float3 L = lightDirection;
    float3 N = normalize(normal.xyz);
    float NdotL = saturate(dot(N, normalize(-L)));
    float3 diffuse = lightColor.rgb * NdotL;
    float3 ambient = float3(0.1f, 0.1f, 0.1f);
    
    if (albedo.a <= 0.0f)
    {
        discard; // 背景を透過させる(処理しない)
    }
    
    albedo.rgb *= (diffuse + ambient);
    
    return albedo;
}
