#define MAX_BONES 64

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float4 color : COLOR;
    int4 boneIndex : BONEINDICES;
    float4 boneWeight : BONEWEIGHTS;
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer Transform : register(b0)
{
    float4x4 World : packoffset(c0);
    float4x4 View : packoffset(c4);
    float4x4 Proj : packoffset(c8);
}

cbuffer BoneMatrices : register(b1)
{
    float4x4 BoneMatrix[MAX_BONES] : packoffset(c12);
}

VSOutput main(VSInput input)
{
#if 0
    float4 skinnedPos = float4(0, 0, 0, 0);
    float3 skinnedNormal = float3(0, 0, 0);

    // スキニング
    for (int i = 0; i < 4; i++)
    {
        float4x4 boneMat = BoneMatrix[input.boneIndex[i]];
        skinnedPos += mul(boneMat, float4(input.pos, 1.0f)) * input.boneWeight[i];
        skinnedNormal += mul((float3x3) boneMat, input.normal) * input.boneWeight[i];
    }

    // ワールド・ビュー・プロジェクション変換
    float4 worldPos = mul(World, skinnedPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    VSOutput output;
    output.svpos = projPos;
    output.color = input.color;
    output.uv = input.uv;
#else
    VSOutput output = (VSOutput) 0;

    float4 localPos = float4(input.pos, 1.0f); // 頂点座標
    float4 worldPos = mul(World, localPos); // ワールド座標
    float4 viewPos = mul(View, worldPos); // ビュー座標
    float4 projPos = mul(Proj, viewPos); // 投影座標

    output.svpos = projPos;
    output.color = input.color;
    output.uv = input.uv;
#endif

    return output;
}