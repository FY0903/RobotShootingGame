struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer Param : register(b1)
{
    float InnerRadius : packoffset(c0); // ‘å‹C‚Ì“à”¼Œa
    float OuterRadius : packoffset(c0.w); // ‘å‹C‚ÌŠO”¼Œa

    float Kr : packoffset(c1); // ƒŒƒCƒŠ[U—’è”
    float Km : packoffset(c1.w); // ƒ~[U—’è”
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

static const float Samples = 2.0f;

static const float3 threePrimaryColors = float3(0.68f, 0.55f, 0.44f); // RGBŠeF‚Ì”g’·(micrometers)
static const float3 InvWaveLength = 1.0f / pow(threePrimaryColors, 4.0f); // ”g’·‚Ì4æ‚Ì‹t”

static const float ESun = 20.0f; // ‘¾—z‚Ì‹P“x
static const float KrSun = Kr * ESun;

float4 main(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv);
}