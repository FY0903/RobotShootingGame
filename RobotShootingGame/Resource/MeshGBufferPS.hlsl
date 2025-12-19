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
    
    float4 color = float4(1, 1, 1, 1);
    float2 halfGrid = floor(abs(input.uv) * 2.0f);
    float2 quatGrid = floor(abs(input.uv) * 8.0f);

    float fHalf = fmod(halfGrid.x + halfGrid.y, 2.0f);
    float quat = fmod(quatGrid.x + quatGrid.y, 2.0f);

    output.albedo = ((fHalf * 0.1f) * quat + 0.45f) + (1 - quat) * 0.05f;
    output.albedo.a = 1.0f;
    output.normal = normalize(input.normal) * 0.5f + 0.5f; // ñ@ê¸Ç[0,1]îÕàÕÇ…ïœä∑
    output.worldPos = input.worldPos;
    
    return output;
}