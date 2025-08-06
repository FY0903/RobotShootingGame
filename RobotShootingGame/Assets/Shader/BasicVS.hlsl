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
#if 0
    float3x3 TangentBasis : TANGENT_BASIS;
#else
    float3x3 InvTangentBaisis : INV_TANGENT_BASIS; // 接線空間の逆行列を使用
#endif
};

cbuffer Transform : register(b0)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
}

cbuffer CbMesh : register(b1)
{
    float4x4 World : packoffset(c0);
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
    
#if 0
    // 接線空間上でライティングする場合
    // 基底行列変換
    output.TangentBasis = float3x3(T, B, N); // 接線、従法線、法線の順で格納
#else
    // ワールド空間上でライティングする場合
    // 基底変換行列の逆行列
    output.InvTangentBaisis = transpose(float3x3(T, B, N)); // 接線、従法線、法線の逆行列を格納
#endif

    return output;
}