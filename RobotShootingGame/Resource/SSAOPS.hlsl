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

cbuffer KernelBuffer : register(b1)
{
    float4 kernel[64] : packoffset(c0);
};

SamplerState smp : register(s0);

Texture2D<float4> tex : register(t0);
Texture2D<float> depthTex : register(t1);

float3 ReconstructWorldPositionFromDepth(float2 uv, float depth)
{
    float4 proj = float4(uv * 2.0f - 1.0f, depth, 1.0f); // NDC空間の座標に変換
    proj.y = -proj.y; // Y軸を反転（DirectXのNDCは左手系）
    
    float4x4 invVP = mul(invV, invP);
    float4 pos = mul(invVP, proj);
    
    return pos.xyz / pos.w;
}

float3 ReconstructViewPositionFromDepth(float2 uv, float depth)
{
    float4 proj = float4(uv * 2.0f - 1.0f, depth, 1.0f); // NDC空間の座標に変換
    proj.y = -proj.y; // Y軸を反転（DirectXのNDCは左手系）
    
    float4 pos = mul(invP, proj);
    
    return pos.xyz / pos.w;
}

float3x3 GetTBN(float3 viewNormal)
{
    float3 tanget = float3(1.0f, 0.0f, 0.0f); // 本来はノイズテクスチャなどから取得するべき
    float3 bitanget = cross(viewNormal, tanget);
    float3x3 TBN = float3x3(tanget, bitanget, viewNormal);
    
    return TBN;
}

float4 main(VSOutput input) : SV_TARGET
{   
    float4 albedo = tex.Sample(smp, input.uv);
    float d = depthTex.Sample(smp, input.uv);
    
    float3 worldPos = ReconstructWorldPositionFromDepth(input.uv, d);
    float3 viewPos = ReconstructViewPositionFromDepth(input.uv, d);
    
    float3 dpdx = ddx(viewPos);
    float3 dpdy = ddy(viewPos);
    
    float3 n = normalize(cross(dpdx, dpdy));
    float3 viewNormal = normalize(n);
    
    int occlusion = 0;
    float radius = 0.2f;
    for (int i = 0; i < 64; i++)
    {
        float3 sample = mul(GetTBN(viewNormal), kernel[i].xyz); // TBN行列でビュー空間の法線に変換
        sample = sample * radius + viewPos.xyz;
        
        float4 offset = float4(sample, 1.0f); // オフセット位置を計算
        float4 offsetClipPos = mul(P, offset); // クリップ空間に変換
        float2 offsetNDCPos = offsetClipPos.xy / offsetClipPos.w; // NDC空間に変換
        
        offsetNDCPos.y = -offsetNDCPos.y; // Y軸を反転（DirectXのNDCは左手系）
        
        float2 sampleUV = offsetNDCPos * 0.5f + 0.5f; // テクスチャUVに変換
        
        float sampleDepth = depthTex.Sample(smp, sampleUV).r; // サンプリングしたデプス値を取得
        float3 sampledViewPos = ReconstructViewPositionFromDepth(sampleUV, sampleDepth); // ビュー空間のZ座標を再構築
        
        float rangeCheck = abs(viewPos.z - sampledViewPos.z) < radius ? 1.0f : 0.0f;
        occlusion += (sampleDepth <= sample.z ? 1 : 0) * rangeCheck;
    }
    
    float aoRate = 1.0f - clamp((float) occlusion / 64.0f, 0.0f, 1.0f);
    
    float4 aoColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
    
    albedo = lerp(albedo, aoColor, aoRate);
    
    return albedo;
}
