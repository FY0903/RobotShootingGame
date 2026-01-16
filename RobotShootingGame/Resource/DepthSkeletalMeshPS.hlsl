struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 screenPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

float main(VSOutput input) : SV_TARGET
{
    float ndcZ = input.screenPos.z / input.screenPos.w;
    
    return ndcZ;
}