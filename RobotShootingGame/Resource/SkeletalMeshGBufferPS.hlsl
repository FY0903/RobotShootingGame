struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
    float4 normal : NORMAL;
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
    output.normal = input.normal;
    output.worldPos = input.worldPos; // ÉèÅ[ÉãÉhç¿ïW
    
    return output;
}