struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer ShadowVPC : register(b0)
{
    float4x4 LightViewProj : packoffset(c0);
}

SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> shadowMap : register(t1);

float4 main(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv) * shadowMap.Sample(smp, input.uv);
}