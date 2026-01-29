struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 worldPos : TEXCOORD1;
};

cbuffer Light : register(b1)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
}

SamplerState smp : register(s0);
SamplerComparisonState cmpSmp : register(s1);
Texture2D<float4> tex : register(t0);
Texture2D<float> shadowMap : register(t1);

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);
    
    float4 lightPos = float4(input.worldPos, 1.0f);
    lightPos = mul(View, lightPos);
    lightPos = mul(Proj, lightPos);
    
    float lightDepth = lightPos.z / lightPos.w; // NDC空間の深度値
    
    float2 shadowUV = lightPos.xy * 0.5f + 0.5f; // [0,1]範囲に変換
    shadowUV.y = 1.0f - shadowUV.y; // Y軸反転
    
    float4 shadowMap = shadowMap.Sample(smp, input.uv);
    
    lightDepth -= 0.005f; // シャドウアキネーション

    if (shadowMap.x < lightDepth)
    {
        color *= 0.5f; // シャドウ内は暗くする
    }
    
    return color;
}