struct VSInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
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
    
    float4 localPos = float4(input.pos, 1.0f); // 頂点座標
    float4 worldPos = mul(World, localPos); // ワールド座標
    float4 viewPos = mul(View, worldPos); // ビュー座標
    float4 projPos = mul(Proj, viewPos); // 投影座標
    
    output.svpos = projPos;
    output.uv = input.uv;
    
    return output;
}