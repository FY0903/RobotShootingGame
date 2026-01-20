struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 worldPos : TEXCOORD1;
    float2 uv : TEXCOORD;
    float4 normal : TEXCOORD2;
};

struct PSOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 worldPos : SV_Target2;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    output.albedo = tex.Sample(smp, input.uv);
    output.albedo.a = 1.0f;
    output.normal = normalize(input.normal) * 0.5f + 0.5f; // –@ü‚ğ[0,1]”ÍˆÍ‚É•ÏŠ·
    output.worldPos = input.worldPos;
    
    return output;
}