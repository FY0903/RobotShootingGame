struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
};

cbuffer Light : register(b1)
{
    float4x4 LView : packoffset(c0);
    float4x4 LProj : packoffset(c4);
}

SamplerState smp : register(s0);
SamplerComparisonState cmpSmp : register(s1);
Texture2D<float4> tex : register(t0);
Texture2D<float> shadowMap : register(t1);

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);
    
    float4 lightViewPos = mul(LView, input.worldPos);
    float4 lightProjPos = mul(LProj, lightViewPos);
    
    // NDC を [0,1] UV に変換
    float2 shadowUV = lightProjPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y;
    
    float writeDepth = lightProjPos.z / lightProjPos.w;
    
    // PCFによるシャドウマッピング
    // 影の部分が0、影の外が1を返す
    float shadow = shadowMap.SampleCmpLevelZero(cmpSmp, shadowUV, writeDepth);
    
    color.rgb *= lerp(color.rgb * 0.2f, color.rgb, shadow);
    
    return color;
}