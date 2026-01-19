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

float4 main(VSOutput input) : SV_TARGET
{
    float4 albedo = tex.Sample(smp, input.uv);
    float d = depthTex.Sample(smp, input.uv);
    
    float3 worldPos = ReconstructWorldPositionFromDepth(input.uv, d);
    
    float3 ray = worldPos - cameraPos;
    float distance = length(ray);
    float3 rayDir = normalize(ray);
    
    float currentDistance = 0.0f;
    float totalTransmittance = 1.0f;
    float4 density = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float stepSize = 0.1f;
    for (int i = 0; i < 64; ++i)
    {
        currentDistance += stepSize; // サンプリング間隔
        if (currentDistance > distance)
        {
            break;
        }
        
        float stepTramsmittance = exp(-density * stepSize);
        float fogContribution = totalTransmittance * (1.0f - stepTramsmittance);
        albedo += fogContribution;
        
        totalTransmittance *= stepTramsmittance;
        
        if (totalTransmittance < 0.01f)
        {
            break;
        }
    }
    
    return albedo;
}
