struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 screenPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

float4 main(VSOutput input) : SV_TARGET
{
    float ndcZ = input.screenPos.z / input.screenPos.w;
    
    return float4(input.screenPos.z, 0.0f, 0.0f, 1.0f);
}