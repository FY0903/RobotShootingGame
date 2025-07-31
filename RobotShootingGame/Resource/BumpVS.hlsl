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
    float4 WorldPos : WORLD_POS;
    float3x3 TangentBasis : TANGENT_BASIS;
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
    output.WorldPos = worldPos;
    
    // 基底ベクトル
    float3 N = normalize(mul((float3x3) World, input.Normal));
    float3 T = normalize(mul((float3x3) World, input.Tangent));
    float3 B = normalize(cross(N, T)); // 法線と接線から従法線を計算
    
    // 基底変換行列
    output.TangentBasis = float3x3(T, B, N); // 接線、従法線、法線の順で格納

    return output;
}