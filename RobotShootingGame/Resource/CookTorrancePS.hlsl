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
    float Roughness : packoffset(c1);
    float Metallic : packoffset(c1.y);
};

SamplerState WrapSmp : register(s0);
Texture2D ColorMap : register(t0);

// Schlickの近似を使用してフレネル項を計算
// specular: 鏡面反射の色
// VH: 視線と反射ベクトルの間の角度のコサイン
float3 SchlickFresnel(float3 specular, float VH)
{
    return specular + (1.0f - specular) * pow((1.0f - VH), 5.0f);
}

// Beckmann分布を使用してマイクロファセットの分布を計算
// m: マイクロファセットの粗さ
// NH: 法線とハーフベクトルの間の角度のコサイン
float D_Beckmann(float m, float NH)
{
    float c2 = NH * NH;
    float c4 = c2 * c2;
    float m2 = m * m;
    
    return (1.0f / (m2 * c4)) * exp((-1.0f / m2) * (1.0f - c2) / c2);
}

// V-cavityによるシャドウイング-マスキングを計算
// NH: 法線とハーフベクトルの間の角度のコサイン
// NV: 法線と視線ベクトルの間の角度のコサイン
// NL: 法線とライトベクトルの間の角度のコサイン
// VH: 視線とハーフベクトルの間の角度のコサイン
float G2_Vcavity(float NH, float NV, float NL, float VH)
{
    return min(1.0f, min(2.0f * NH * NV / VH, 2.0f * NH * NL / VH));
}

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    float3 N = normalize(input.Normal);
    float3 L = normalize(input.WorldPos - LightPosition);
    L *= -1.0f; // ライト位置からのベクトルを反転
    float3 V = normalize(CameraPosition - input.WorldPos); // カメラ位置からのベクトルを計算
    float3 R = normalize(-V + 2.0f * dot(N, V) * N); // リフレクションベクトルを計算
    float3 H = normalize(V + L); // ハーフベクトルを計算
    
    float NH = saturate(dot(N, H)); // 法線とハーフベクトルのドット積を計算し、0から1の範囲に制限
    float NV = saturate(dot(N, V)); // 法線と視線ベクトルのドット積を計算し、0から1の範囲に制限
    float NL = saturate(dot(N, L)); // 法線とライトベクトルのドット積を計算し、0から1の範囲に制限
    float VH = saturate(dot(V, H)); // 視線とハーフベクトルのドット積を計算し、0から1の範囲に制限
    
    float4 color = ColorMap.Sample(WrapSmp, input.TexCoord);
    float3 Kd = BaseColor * (1.0f - Metallic); // 拡散反射係数を計算
    float3 diffuse = Kd * (1.0f / F_PI); // 拡散反射の色を計算
    
    float3 Ks = BaseColor * Metallic; // 鏡面反射係数を計算
    float a = Roughness * Roughness; // 粗さを計算
    float D = D_Beckmann(a, NH); // マイクロファセットの分布を計算
    float G2 = G2_Vcavity(NH, NV, NL, VH); // V-cavityによるシャドウイング-マスキングを計算
    float3 Fr = SchlickFresnel(Ks, NL); // フレネル項を計算
    
    float3 specular = (D * G2 * Fr) / (4.0f * NV * NL); // 鏡面反射の色を計算
    
    output.Color = float4(LightColor * color.rgb * (diffuse + specular) * NL, color.a * Alpha);

    return output;
}