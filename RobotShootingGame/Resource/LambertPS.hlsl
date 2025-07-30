struct VSOutput
{
    float4 position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float4 WorldPos : WORLD_POS;
};

struct PSOutput
{
    float4 Color : SV_TARGET0;
};

cbuffer LightBuffer : register(b1)
{
    float3 LightPosition : packoffset(c0);
    float3 LightColor : packoffset(c1);
};

cbuffer MaterialBuffer : register(b2)
{
    float3 Diffuse : packoffset(c0);
    float Alpha : packoffset(c0.w);
};

SamplerState WrapSmp : register(s0);
Texture2D ColorMap : register(t0);

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    float3 N = normalize(input.Normal);
    float3 L = normalize(input.WorldPos.xyz - LightPosition);
    L *= -1.0f; // ライト位置からのベクトルを反転
    
    float4 color = ColorMap.Sample(WrapSmp, input.TexCoord);
    float3 diffuse = LightColor * Diffuse * saturate(dot(L, N)); // ライトの色と拡散係数を掛け合わせ、法線とライトベクトルのドット積を計算

    output.Color = float4(color.rgb * diffuse, color.a * Alpha);

    return output;
}