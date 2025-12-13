struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);

Texture2D<float4> albedoTex : register(t0);
Texture2D<float4> normalTex : register(t1);
Texture2D<float4> worldPosTex : register(t2);

float4 main(VSOutput input) : SV_TARGET
{
    float4 albedo = albedoTex.Sample(smp, input.uv);
    //float4 normal = normalTex.Sample(smp, input.uv);
    float4 worldPos = worldPosTex.Sample(smp, input.uv);
    
    // ‡¬ˆ—i—á: ’Pƒ‚É‰ÁZj
    //float4 finalColor = albedo + normal * 0.5f + worldPos * 0.1f;
    
    return worldPos;
}
