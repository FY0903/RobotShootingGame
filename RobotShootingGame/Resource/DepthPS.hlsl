struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

float4 main(VSOutput input) : SV_TARGET
{
    float ndcZ = input.svpos.z / input.svpos.w;
    
    return float4(ndcZ, ndcZ, ndcZ, 1.0f);
}