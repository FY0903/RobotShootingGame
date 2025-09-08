#ifndef BRDF_HLSLI
#define BRDF_HLSLI

#ifndef F_PI
#define F_PI        3.14159265358979323f   // 円周率
#endif

// Schlickの近似を使用してフレネル項を計算
// specular: 鏡面反射の色
// VH: 視線と反射ベクトルの間の角度のコサイン
float3 SchlickFresnel(float3 specular, float VH)
{
    return specular + (1.0f - specular) * pow((1.0f - VH), 5.0f);
}

// GGX BRDFの計算
// a: 粗さのパラメータ
// NH: 法線とハーフベクトルの間の角度のコサイン
float D_GGX(float a, float NH)
{
    float a2 = a * a;
    float f = (NH * NH) * (a2 - 1) + 1;
    return a2 / (F_PI * f * f);
}

// SmithのG関数を使用して、視線と光線の間の遮蔽を計算
// NL: 法線と光線の間の角度のコサイン
// NV: 法線と視線の間の角度のコサイン
// a: 粗さのパラメータ
float G2_Smith(float NL, float NV, float a)
{
    float a2 = a * a;
    float NL2 = NL * NL;
    float NV2 = NV * NV;

    float Lambda_V = (-1.0f + sqrt(a2 * (1.0f - NL2) / NL2 + 1.0f)) * 0.5f;
    float Lambda_L = (-1.0f + sqrt(a2 * (1.0f - NV2) / NV2 + 1.0f)) * 0.5f;
    return 1.0f / (1.0f + Lambda_V + Lambda_V);
}

// BRDFの計算
// Kd: 拡散反射の色
float3 ComputeLambert(float3 Kd)
{
    return Kd / F_PI;
}

// Phong BRDFの計算
// Ks: 鏡面反射の色
// shininess: 光沢の度合い
// LdotR: 光線と反射ベクトルの間の角度のコサイン
float3 ComputePhong(float3 Ks, float shininess, float LdotR)
{
    return Ks * ((shininess + 2.0f) / (2.0f * F_PI)) * pow(LdotR, shininess);
}

// GGX BRDFの計算
// Ks: 鏡面反射の色
// roughness: 粗さのパラメータ
// NdotH: 法線とハーフベクトルの間の角度のコサイン
// NdotV: 法線と視線の間の角度のコサイン
// NdotL: 法線と光線の間の角度のコサイン
float3 ComputeGGX(float3 Ks, float roughness, float NdotH, float NdotV, float NdotL)
{
    float a = roughness * roughness;
    float D = D_GGX(a, NdotH);
    float G = G2_Smith(NdotL, NdotV, a);
    float3 F = SchlickFresnel(Ks, NdotL);

    return (D * G * F) / (4.0f * NdotV * NdotL);
}
#endif
