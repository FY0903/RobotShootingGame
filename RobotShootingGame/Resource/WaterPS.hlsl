struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

cbuffer TimeBuffer : register(b1)
{
    // 経過時間（秒）
    float time : packoffset(c0);
}

float2 Random(float2 seed)
{
    // ハッシュ関数的なノイズ生成
    float2 s = float2(dot(seed, float2(127.1f, 311.7f)), dot(seed, float2(269.5f, 183.3f)));
    
    // -1.0 ~ 1.0 の範囲でランダム値を生成
    return -1.0f + 2.0f * frac(sin(s) * 43758.5453f);
}

// バリューノイズ関数
float Noize(float2 uv)
{
    // グリッドの四隅のランダム勾配ベクトルを取得
    float2 p = floor(uv); // uv の整数部分
    float2 f = frac(uv); // uv の小数部分
    
    // 内積を計算
    float w00 = dot(Random(p), f);
    float w10 = dot(Random(p + float2(1.0f, 0.0f)), f - float2(1.0f, 0.0f));
    float w01 = dot(Random(p + float2(0.0f, 1.0f)), f - float2(0.0f, 1.0f));
    float w11 = dot(Random(p + float2(1.0f, 1.0f)), f - float2(1.0f, 1.0f));
    
    // スムーズステップで補間
    float2 u = f * f * (3.0f - 2.0f * f);
    
    // バイリニア補間
    return lerp(lerp(w00, w10, u.x), lerp(w01, w11, u.x), u.y);
}

float Voronoi(float2 uv, float time)
{
    float2 p = floor(uv);
    float2 f = frac(uv);
    
    float m_dist = 10.0f; // 最小距離
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            // 隣接セルのランダムポイントを取得
            float2 neighbor = float2(float(i), float(j));
            
            // ランダムポイントの位置を計算
            float2 pt = Random(p + neighbor);
            
            // アニメーションを加える
            pt = 0.5f + 0.5f * sin(time + 6.2831f * pt);
            
            // 距離を計算
            float2 diff = neighbor + pt - f;
            
            // 距離の長さ
            float dist = length(diff);
            
            // 最小距離を更新
            m_dist = min(m_dist, dist);
        }
    }
    
    return m_dist;
}

float2 RadialShear(float2 uv, float2 center, float2 strength)
{
    float2 dir = uv - center;
    float d = dot(dir, dir);
    float2 offset = d * strength;
    return uv + float2(dir.y, -dir.x) * offset;
}

float3 HeightToNormal_Derivative(float height, float normalStrength)
{
    // 高さから法線を計算（微分を使用）
    float3 du = float3(1.0f, 0.0f, ddx(height) * normalStrength);
    float3 dv = float3(0.0f, 1.0f, ddy(height) * normalStrength);
    
    // 法線ベクトルを計算
    return normalize(cross(du, dv));
}

float4 main(VSOutput input) : SV_TARGET
{   
    float3 color = float3(0.0f, 0.55f, 0.75f);
    float2 uv = RadialShear(input.uv, float2(0.5f, 0.5f), float2(8.0f, 8.0f));
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float scale = 50.0f;
    
    float h = Noize(input.uv * scale + time * 0.3f) * 0.5f + 0.5f;
    float3 n = HeightToNormal_Derivative(h, 50.0f);
    n = n * 0.5f + 0.5f;
    
    // めんどいからライトの向きを固定
    float3 L = normalize(float3(1.0f, 1.0f, 0.0f));
    float d = saturate(dot(n, L));
    color.rgb *= d;
    
    float v1 = Voronoi(uv * scale, time);
    float v2 = Voronoi(uv * scale, time * 2.0f);
    v1 = pow(max(v1, 0.0f), 7.0f);
    v2 = pow(max(v2, 0.0f), 7.0f);
    
    float v = min(v1, v2);
    
    float3 voronoiCol = v * color;
    
    finalColor.rgb = lerp(color, voronoiCol, 0.5f);
    
    return finalColor;
}