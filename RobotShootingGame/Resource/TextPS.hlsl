struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

cbuffer TimeBuffer : register(b0)
{
    // åoâﬂéûä‘ÅiïbÅj
    float time : packoffset(c0);
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);
    color.a *= sin(time) * 0.5f + 0.5f;
    
    return color;
}