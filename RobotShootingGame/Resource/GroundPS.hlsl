struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 posLVP : TEXCOORD1;
};

SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> shadowMap : register(t1);

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);
    
    float zLVP = input.posLVP.z / input.posLVP.w;
    
    if (zLVP >= 0.0f && zLVP <= 1.0f)
    {
        float2 shadowUV = input.posLVP.xy / input.posLVP.w;
        shadowUV *= float2(0.5f, -0.5f);
        shadowUV += 0.5f;
        
        if (shadowUV.x >= 0.0f && shadowUV.x <= 1.0f &&
            shadowUV.y >= 0.0f && shadowUV.y <= 1.0f)
        {
            float2 shadow = shadowMap.Sample(smp, shadowUV).xy;
            if (zLVP >= shadow.r)
            {
                color *= 0.5f;
            }
        }
    }
    
    return color;
}