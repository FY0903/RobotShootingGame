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
    float4 normal = normalTex.Sample(smp, input.uv);
    normal.xyz = normalize(normal.xyz * 2.0f - 1.0f); // 法線ベクトルを[-1,1]範囲に変換
    float4 worldPos = worldPosTex.Sample(smp, input.uv);
    worldPos.xyz = worldPos.xyz * 2.0f - 1.0f; // ワールド座標を[-1,1]範囲に変換
    
    // 合成処理（例: 単純に加算）
    //float4 finalColor = albedo + normal * 0.5f + worldPos * 0.1f;
    
    return worldPos;
}
