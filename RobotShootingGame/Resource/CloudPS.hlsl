struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer lightBuffer : register(b1)
{
    float3 lightPosition : packoffset(c0);
    float lightRange : packoffset(c0.w);
    float3 lightDirection : packoffset(c1);
    float lightAngle : packoffset(c1.w);
    float4 lightColor : packoffset(c2);
};

cbuffer TimeBuffer : register(b2)
{
    float time : packoffset(c0);
}

SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);

#define MAX_STEPS 32
#define MATCH_SIZE 0.1f

// https://www.shadertoy.com/view/WdXGRj
float hash(float n)
{
    return frac(sin(n) * 43758.5453f);
}

// 球のSDF関数
// p: 点の位置, radius: 球の半径
// 中心に一番近いほど正の値、遠いほど負の値を返す
float SDFSphere(float3 p, float radius)
{
    return length(p) - radius; // 球のSDF
}

// https://www.shadertoy.com/view/WdXGRj
float Noise(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    
    f = f * f * (3.0f - 2.0f * f);
    
    float n = p.x + p.y * 57.0f + 113.0f * p.z;
    float res = lerp(lerp(lerp(hash(n + 0.0f), hash(n + 1.0f), f.x),
                          lerp(hash(n + 57.0f), hash(n + 58.0f), f.x), f.y),
                     lerp(lerp(hash(n + 113.0f), hash(n + 114.0f), f.x),
                          lerp(hash(n + 170.0f), hash(n + 171.0f), f.x), f.y), f.z);
    return res;
}

// https://thebookofshaders.com/13/?lan=jp
float FBM(float3 p)
{
    // アニメーションのために時間を加える
    float3 q = p + time * 0.1f * float3(1.0f, -0.2f, -1.0f);

    float f = 0.0f;
    float scale = 0.5f;
    float factor = 2.02f;

    for (int i = 0; i < 6; i++)
    {
        f += scale * Noise(q);
        q *= factor;
        factor += 0.21f;
        scale *= 0.5f;
    }

    return f;
}

float Scene(float3 p)
{
    // より大きなスケールのノイズを追加
    float largeFBM = FBM(p * 2.0f); // 大きな雲の塊
    float detailFBM = FBM(p * 7.0f); // 細かいディテール
    
    float density = largeFBM * 0.1f + detailFBM * 0.8f;
    
    float distance = SDFSphere(p, 1.0f);
    
    //return -distance + FBM(p);
    
    return -0.5f + detailFBM * 0.95f;
    
    return density - 0.5f; // 閾値を調整
}

float4 Raymarch(float3 rayOrigin, float3 rayDirection)
{
    float depth = 0.0f;
    float3 p = rayOrigin + rayDirection * depth;
    
    float4 res = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < MAX_STEPS; ++i)
    {
        float density = Scene(p);
        
        // 描画するものがある場合
        if (density > 0.0f)
        {
            // ライトの方向に対する影の濃さを計算
            // https://iquilezles.org/articles/derivative/
            float diffuse = saturate(Scene(p) - Scene(p + 0.1f * -float3(lightDirection.x, -lightDirection.z, lightDirection.y)));
            
            // ライトの色と拡散光を計算
            float3 lin = float3(0.9f, 0.9f, 0.9f) + lightColor.rgb * diffuse;
            
            // 値が大きいほど濃くなる
            float4 color = float4(lerp(float3(1.0f, 1.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), density), density);
            color.rgb *= lin;
            color.rgb *= color.a;
            res += color * (1.0f - res.a);
        }
        
        depth += MATCH_SIZE;
        p = rayOrigin + rayDirection * depth;
    }
    
    return res;
}

float4 main(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv;
    uv -= 0.5f; // 中心を原点に移動
    
    float3 ro = float3(0.0f, 0.0f, 5.0f); // カメラ位置
    float3 rd = normalize(float3(uv, -1.0f)); // レイ方向
    
    float3 color = float3(0.6f, 0.6f, 0.75f);
    
    float4 res = Raymarch(ro, rd);
    color = color * (1.0f - res.a) + res.rgb;
    
    return float4(color, 1.0f);
}