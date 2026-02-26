struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 worldPos : TEXCOORD0;
};

cbuffer Param : register(b1)
{
    float3 lightDir : packoffset(c0);
    float3 cameraPos : packoffset(c1);
};

#define CAMERA_HEIGHT 1.000001
#define INV_WAVE_LENGTH float3( 1.0 / pow(0.68, 4.0), 1.0 / pow(0.55, 4.0), 1.0 / pow(0.44, 4.0) )
#define INNER_RADIUS 1.0
#define OUTER_RADIUS 1.025
#define ESUN 10.0
#define KR 0.0025
#define KM 0.0015
#define SCALE_DEPTH 0.25
#define SAMPLES 2
#define G -0.99
#define GROUND_COLOR float3( 1.0, 1.0, 1.0 )
#define GAMMA 1.0 / 2.2

#define PI 3.14159265

// スケーリング関数
// fCos: 光の入射角の余弦
float scale(float fCos)
{
    float x = 1.0 - fCos;
    return SCALE_DEPTH * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

// 指定されたレイと球の交点
// pos: レイの原点
// dir: レイの方向（正規化されていること）
// dist2: レイの原点から球の中心までの距離の二乗
// rad2: 球の半径の二乗
float2 getIntersections(float3 pos, float3 dir, float dist2, float rad2)
{
    // 二次方程式の係数
    float B = 2.0 * dot(pos, dir);  // B項
    float C = dist2 - rad2; // C項

    float det = max(0.0, B * B - 4.0 * C); // 判別式
    
    // 解の公式を使用して交点を計算
    return 0.5 * float2(
        (-B - sqrt(det)), // 近い交点
        (-B + sqrt(det)) // 遠い交点
    );
}

// レイリー散乱の位相関数を計算
// fCos2: 光の入射角の余弦の二乗
float getRayleighPhase(float fCos2)
{
    return 0.75 * (2.0 + 0.5 * fCos2);
}

// ミー散乱の位相関数を計算
// fCos: 光の入射角の余弦
// fCos2: 光の入射角の余弦の二乗
// g: ミー散乱の非対称性パラメータ
// g2: ミー散乱の非対称性パラメータの二乗
float getMiePhase(float fCos, float fCos2, float g, float g2)
{
    return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2)
        / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = (float4) 0;
    
    // 事前計算
    float fInnerRadius2 = INNER_RADIUS * INNER_RADIUS;
    float fOuterRadius2 = OUTER_RADIUS * OUTER_RADIUS;
    float fKrESun = KR * ESUN;
    float fKmESun = KM * ESUN;
    float fKr4PI = KR * 4.0 * PI;
    float fKm4PI = KM * 4.0 * PI;
    float fScale = 1.0 / (OUTER_RADIUS - INNER_RADIUS);
    float fScaleOverScaleDepth = fScale / SCALE_DEPTH;
    float fG2 = G * G;

    // ライトの方向を正規化
    float3 v3LightDir = normalize(-lightDir);

    // レイの原点と方向を計算
    float3 v3RayOri = float3(0.0, CAMERA_HEIGHT, 0.0);
    float3 v3RayDir = normalize(input.worldPos.xyz - v3RayOri);
    
    // レイの原点から球の中心までの距離の二乗と、球の半径の二乗を使用して交点を計算
    float fCameraHeight = length(v3RayOri);
    float fCameraHeight2 = fCameraHeight * fCameraHeight;

    // レイと内側の球、外側の球の交点を計算
    float2 v2InnerIsects = getIntersections(v3RayOri, v3RayDir, fCameraHeight2, fInnerRadius2);
    float2 v2OuterIsects = getIntersections(v3RayOri, v3RayDir, fCameraHeight2, fOuterRadius2);
    
    // レイが地面に当たっているかどうかを判定
    bool isGround = 0.0 < v2InnerIsects.x;

    if (v2OuterIsects.x == v2OuterIsects.y) // レイが外側の球と交差しない場合は、空を描画
    {
        color = float4(0.0, 0.0, 0.0, 1.0);
        return color;
    }

    // レイが地面に当たっている場合は、内側の球との交点を使用してサンプリングを開始
    float fNear = max(0.0, v2OuterIsects.x);
    float fFar = isGround ? v2InnerIsects.x : v2OuterIsects.y;
    float3 v3FarPos = v3RayOri + v3RayDir * fFar;
    float3 v3FarPosNorm = normalize(v3FarPos);

    // サンプリングの開始点を計算
    float3 v3StartPos = v3RayOri + v3RayDir * fNear;
    float fStartPosHeight = length(v3StartPos);
    float3 v3StartPosNorm = v3StartPos / fStartPosHeight;
    float fStartAngle = dot(v3RayDir, v3StartPosNorm);
    float fStartDepth = exp(fScaleOverScaleDepth * (INNER_RADIUS - fStartPosHeight));
    float fStartOffset = fStartDepth * scale(fStartAngle);

    // カメラの角度を計算して、スケーリングとオフセットを適用
    float fCameraAngle = dot(-v3RayDir, v3FarPosNorm);
    float fCameraScale = scale(fCameraAngle);
    float fCameraOffset = exp((INNER_RADIUS - fCameraHeight) / SCALE_DEPTH) * fCameraScale;

    // ライトの角度を計算して、スケーリングとオフセットを適用
    float fTemp = scale(dot(v3FarPosNorm, v3LightDir)) + scale(dot(v3FarPosNorm, -v3RayDir));

    // サンプリングの長さと方向を計算
    float fSampleLength = (fFar - fNear) / float(SAMPLES);
    float fScaledLength = fSampleLength * fScale;
    float3 v3SampleDir = v3RayDir * fSampleLength;
    float3 v3SamplePoint = v3StartPos + v3SampleDir * 0.5;

    float3 v3FrontColor = (float3) 0;
    float3 v3Attenuate;
    
    // サンプリングループ
    for (int i = 0; i < SAMPLES; i++)
    {
        float fHeight = length(v3SamplePoint); // サンプルポイントの高さを計算
        float fDepth = exp(fScaleOverScaleDepth * (INNER_RADIUS - fHeight)); // サンプルポイントの深さを計算
        float fLightAngle = dot(v3LightDir, v3SamplePoint) / fHeight; // サンプルポイントのライトの角度を計算
        float fCameraAngle = dot(v3RayDir, v3SamplePoint) / fHeight; // サンプルポイントのカメラの角度を計算
       
        // サンプルポイントの散乱を計算
        float fScatter = isGround
            ? fDepth * fTemp - fCameraOffset // 地面に当たっている場合は、ライトとカメラのオフセットを使用して散乱を計算
            : (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle))); // 地面に当たっていない場合は、開始オフセットとライトとカメラのスケーリングを使用して散乱を計算
        
        v3Attenuate = exp(-fScatter * (INV_WAVE_LENGTH * fKr4PI + fKm4PI)); // サンプルポイントの減衰を計算
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength); // サンプルポイントの前方の色を計算
        v3SamplePoint += v3SampleDir;
    }

    // 前方の色をクランプして、レイリー散乱とミー散乱の係数を適用して、最終的な色を計算
    v3FrontColor = clamp(v3FrontColor, 0.0, 3.0);
    float3 c0 = v3FrontColor * (INV_WAVE_LENGTH * fKrESun);
    float3 c1 = v3FrontColor * fKmESun;

    // 地面に当たっている場合は、レイリー散乱とミー散乱を組み合わせて色を計算し、ガンマ補正を適用して返す
    if (isGround)
    {
        float3 v3RayleighColor = c0 + c1;
        float3 v3MieColor = clamp(v3Attenuate, 0.0, 3.0);
        
        // 地面の色を加算して、ガンマ補正を適用して返す
        color = float4(1.0 - exp(-(v3RayleighColor + GROUND_COLOR * v3MieColor)), 1.0);
        color.xyz = pow(color.xyz, (float3) GAMMA);
        return color;
    }

    float fCos = dot(-v3LightDir, v3RayDir);
    float fCos2 = fCos * fCos;

    // 地面に当たっていない場合は、レイリー散乱とミー散乱を組み合わせて色を計算し、ガンマ補正を適用して返す
    color = float4(
        getRayleighPhase(fCos2) * c0 + getMiePhase(fCos, fCos2, G, fG2) * c1,
        1.0
    );
    color.xyz = pow(color.xyz, (float3) GAMMA);
    
    return color;
}
