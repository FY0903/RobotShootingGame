#include "Common.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float4 color : COLOR;
    int4 BoneIndex : BONEINDEX;
    float4 BoneWeight : BONEWEIGHT;
    int BoneCount : BONECOUNT;
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
    float4 normal : NORMAL;
};

cbuffer BoneMatrixBuffer : register(b1)
{
    float4x4 BoneMatrices[400];
}

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    float4 localPos = (float4) 0;
    
    // ボーンが存在する場合、スキニングを行う
    if (input.BoneCount > 0)
    {
        float4x4 skinMat = (float4x4) 0;
    
        for (int i = 0; i < 4; ++i)
        {
            skinMat += BoneMatrices[input.BoneIndex[i]] * input.BoneWeight[i];
        }
    
        localPos = float4(input.pos, 1.0f); // 頂点座標
        localPos = mul(skinMat, localPos); // スキニング後の頂点座標        
    }
    else // ボーンが存在しない場合、そのまま頂点座標を使用
    {
        localPos = float4(input.pos, 1.0f); // 頂点座標
    }
    
    float4 worldPos = mul(World, localPos); // ワールド座標
    float4 viewPos = mul(View, worldPos); // ビュー座標
    float4 projPos = mul(Proj, viewPos); // 投影座標
    
    output.svpos = projPos;
    output.uv = input.uv;
    output.worldPos = worldPos;
    output.normal = mul(World, float4(input.normal, 0.0f));
    
    return output;
}