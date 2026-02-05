struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
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

#define MAX_STEPS 100
#define MATCH_SIZE 0.08f

// 球のSDF関数
// p: 点の位置, radius: 球の半径
// 中心に一番近いほど正の値、遠いほど負の値を返す
float SDFSphere(float3 p, float radius)
{
    return length(p) - radius; // 球のSDF
}

float Noise(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    
    f = f * f * (3.0f - 2.0f * f);
    
    float2 uv = (p.xy + float2(37.0f, 239.0f) * p.z) + f.xy;
    float2 col = tex.Sample(smp, (uv + 0.5f) / 256.0f).xy;
    
    return lerp(col.x, col.y, f.z) * 2.0f - 1.0f;
}

float FBM(float3 p)
{
    float3 q = p + time * 0.5f * float3(1.0f, -0.2f, -1.0f);
    
    float f = 0.0f;
    float scale = 0.5f;
    float factor = 2.02f;
    
    for (int i = 0; i < 6; ++i)
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
    float distance = SDFSphere(p, 1.0f); // シーン内のオブジェクトの距離関数
    
    float f = FBM(p);
    
    return -distance + f; // 内部が正、外部が負
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
            // 値が大きいほど濃くなる
            float4 color = float4(lerp(float3(1.0f, 1.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), density), density);
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
    
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float4 res = Raymarch(ro, rd);
    color = res.rgb;
    
    return float4(color, 1.0f);
}