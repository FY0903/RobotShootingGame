struct VSOutput
{
    float4 position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float4 WorldPos : WORLD_POS;
    float3x3 TangentBasis : TANGENT_BASIS;
};

struct PSOutput
{
    float4 Color : SV_TARGET0;
};

cbuffer LightBuffer : register(b1)
{
    float3 LightPosition : packoffset(c0);
    float3 LightColor : packoffset(c1);
    float3 CameraPosition : packoffset(c2);
};

cbuffer MaterialBuffer : register(b2)
{
    float3 Diffuse : packoffset(c0);
    float Alpha : packoffset(c0.w);
    float3 Specular : packoffset(c1);
    float Shininess : packoffset(c1.w);
};

SamplerState WrapSmp : register(s0);
Texture2D ColorMap : register(t0);
Texture2D NormalMap : register(t1);

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    // ライトベクトル
    float3 L = normalize(input.WorldPos.xyz - LightPosition);
    L *= -1.0f; // ライト位置からのベクトルを反転
    L = mul(input.TangentBasis, L); // 接線空間に変換
    
    // 視線ベクトル
    float3 V = normalize(CameraPosition - input.WorldPos.xyz);
    V = mul(input.TangentBasis, V); // 接線空間に変換
    
    // 法線ベクトル
    float3 N = NormalMap.Sample(WrapSmp, input.TexCoord).rgb * 2.0f - 1.0f; // 法線マップから法線を取得し、[-1, 1]の範囲に変換
    
    // 反射ベクトル
    float3 R = normalize(-reflect(V, N));
    
    float4 color = ColorMap.Sample(WrapSmp, input.TexCoord); // テクスチャから色を取得
    float3 diffuse = LightColor * Diffuse * saturate(dot(L, N)); // ライトの色と拡散係数を掛け合わせ、法線とライトベクトルのドット積を計算
    float3 specular = LightColor * Specular * pow(saturate(dot(N, R)), Shininess); // スペキュラ反射の計算
    
    output.Color = float4(color.rgb * (diffuse + specular), color.a * Alpha); // 拡散とスペキュラの合計を計算し、アルファ値を掛ける

    return output;
}