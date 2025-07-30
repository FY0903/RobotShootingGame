struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float4 WorldPos : WORLD_POS;
};

cbuffer Transform : register(b0)
{
    float4x4 World : packoffset(c0);
    float4x4 View : packoffset(c4);
    float4x4 Proj : packoffset(c8);
}

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
   
    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);
    
    output.position = projPos;
    output.TexCoord = input.TexCoord;
    output.Normal = normalize(mul((float3x3) World, input.Normal)); // ワールド空間の法線を計算
    output.WorldPos = worldPos;

    return output;
}