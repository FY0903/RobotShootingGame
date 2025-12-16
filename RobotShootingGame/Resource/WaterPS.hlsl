struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

cbuffer TimeBuffer : register(b0)
{
    float time : packoffset(c0);
}

float2 Random(float2 seed)
{
    float2 s = float2(dot(seed, float2(127.1f, 311.7f)), dot(seed, float2(269.5f, 183.3f)));
    return -1.0f + 2.0f * frac(sin(s) * 43758.5453123f);
}

float Noize(float2 uv)
{
    float2 p = floor(uv);
    float2 f = frac(uv);
    
    float w00 = dot(Random(p), f);
    float w10 = dot(Random(p + float2(1.0f, 0.0f)), f - float2(1.0f, 0.0f));
    float w01 = dot(Random(p + float2(0.0f, 1.0f)), f - float2(0.0f, 1.0f));
    float w11 = dot(Random(p + float2(1.0f, 1.0f)), f - float2(1.0f, 1.0f));
    
    float2 u = f * f * (3.0f - 2.0f * f);
    
    return lerp(lerp(w00, w10, u.x), lerp(w01, w11, u.x), u.y);
}

float4 main(VSOutput input) : SV_TARGET
{
    //return tex.Sample(smp, input.uv);
    
    float scale = 10.0f;
    float n = Noize(input.uv * scale * time);
    
    return float4(n, n, n, 1.0f);
}