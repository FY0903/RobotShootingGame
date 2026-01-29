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

cbuffer Light : register(b1)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
};

SamplerState smp : register(s0);

Texture2D<float4> tex : register(t0);
Texture2D<float> depthTex : register(t1);
Texture2D<float4> shadowMap : register(t2);

float3 ReconstructWorldPositionFromDepth(float2 uv, float depth)
{
    float4 proj = float4(uv * 2.0f - 1.0f, depth, 1.0f); // NDC空間の座標に変換
    proj.y = -proj.y; // Y軸を反転（DirectXのNDCは左手系）
    
    float4x4 invVP = mul(invV, invP);
    float4 pos = mul(invVP, proj);
    
    return pos.xyz / pos.w;
}

float4 TransformWorldToShadowCoord(float3 worldPos)
{
    float4 shadowCoord = mul(View, float4(worldPos, 1.0f));
    shadowCoord = mul(Proj, shadowCoord);
    shadowCoord.xy /= shadowCoord.w; // NDC空間に変換
    shadowCoord.xy = shadowCoord.xy * 0.5f + 0.5f; // [0,1]範囲に変換
    shadowCoord.y = 1.0f - shadowCoord.y; // Y軸反転
    shadowCoord.z /= shadowCoord.w; // 深度値を正規化

    return float4(shadowCoord.xyz, 0.0f);
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 albedo = tex.Sample(smp, input.uv);
    float d = depthTex.Sample(smp, input.uv);
    float4 shadow = shadowMap.Sample(smp, input.uv);
    
    return albedo;
    return shadow;
    
    float3 worldPos = ReconstructWorldPositionFromDepth(input.uv, d);
   
    float4 lightPos = float4(worldPos, 1.0f);
    lightPos = mul(View, lightPos);
    lightPos = mul(Proj, lightPos);
    float zLVP = lightPos.z / lightPos.w; // ライトビュー射影空間での深度値
    
    return float4(zLVP, 0.0f, 0.0f, 1.0f);
    
    float4 shadowCoord = TransformWorldToShadowCoord(worldPos);
    float4 color = shadowMap.Sample(smp, shadowCoord.xy);

    //return shadowCoord;
    //return color;

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
        
        float3 rayPos = cameraPos + rayDir * currentDistance;       
        
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
