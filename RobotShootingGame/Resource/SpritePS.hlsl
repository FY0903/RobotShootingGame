struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

float4 main(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv);
}