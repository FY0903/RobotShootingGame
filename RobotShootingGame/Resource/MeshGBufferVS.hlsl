#include "Common.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 worldPos : TEXCOORD1;
    float2 uv : TEXCOORD;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    
    float4 localPos = float4(input.pos, 1.0f); // 頂点座標
    float4 worldPos = mul(World, localPos); // ワールド座標
    float4 viewPos = mul(View, worldPos); // ビュー座標
    float4 projPos = mul(Proj, viewPos); // 投影座標
    
    output.svpos = projPos;
    output.worldPos = worldPos;
    output.uv = input.uv;
    
    return output;
}