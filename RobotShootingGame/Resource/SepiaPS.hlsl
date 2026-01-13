struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    color = tex.Sample(smp, input.uv);

    // モノクロ化
    float Y = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
    color.r = Y;
    color.g = Y;
    color.b = Y;

    // モノクロからセピアに変換
    color.r = color.r * 107 / 107;
    color.g = color.g * 74 / 107;
    color.b = color.b * 43 / 107;

    return color;
}