struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 screenPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

float main(VSOutput input) : SV_TARGET
{
    float ndcZ = input.screenPos.z / input.screenPos.w;
    
    return ndcZ;
}