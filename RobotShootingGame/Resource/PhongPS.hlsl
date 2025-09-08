static const float F_PI = 3.14159265358979323846f;

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 WorldPos : WORLD_POS;
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
    float3 BaseColor : packoffset(c0);
    float Alpha : packoffset(c0.w);
    float Metallic : packoffset(c1);
    float Shininess : packoffset(c1.y);
};

SamplerState WrapSmp : register(s0);
Texture2D ColorMap : register(t0);

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    float3 N = normalize(input.Normal);
    float3 L = normalize(input.WorldPos.xyz - LightPosition);
    L *= -1.0f; // ライト位置からのベクトルを反転
    float3 V = normalize(CameraPosition - input.WorldPos.xyz); // カメラ位置からのベクトルを計算
    float3 R = normalize(-V + 2.0f * dot(N, V) * N); // リフレクションベクトルを計算
    
    float NL = saturate(dot(N, L)); // 法線とライトベクトルのドット積を計算し、0から1の範囲に制限
    float LR = saturate(dot(L, R)); // ライトベクトルとリフレクションベクトルのドット積を計算し、0から1の範囲に制限
    
    float4 color = ColorMap.Sample(WrapSmp, input.TexCoord);
    float3 diffuse = BaseColor * (1.0f - Metallic) * (1.0f / F_PI); // 拡散反射係数を計算
    float3 specular = BaseColor * Metallic * ((Shininess + 2.0f) / (2.0f * F_PI)) * pow(LR, Shininess); // 鏡面反射係数を計算

    output.Color = float4(LightColor * color.rgb * (diffuse + specular) * NL, color.a * Alpha);

    return output;
}