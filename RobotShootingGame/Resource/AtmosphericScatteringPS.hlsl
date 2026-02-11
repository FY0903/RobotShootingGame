struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
};

cbuffer SkyBuffer : register(b1)
{
    float InnerRadius : packoffset(c0); // 大気の内半径
    float OuterRadius : packoffset(c0.w); // 大気の外半径

    float Kr : packoffset(c1); // レイリー散乱定数
    float Km : packoffset(c1.w); // ミー散乱定数
};

cbuffer CameraBuffer : register(b2)
{
    float4x4 invV : packoffset(c0);
    float4x4 invP : packoffset(c4);
    float4x4 P : packoffset(c8);
    float3 cameraPos : packoffset(c12);
};

cbuffer lightBuffer : register(b3)
{
    float3 lightPosition : packoffset(c0);
    float lightRange : packoffset(c0.w);
    float3 lightDirection : packoffset(c1);
    float lightAngle : packoffset(c1.w);
    float4 lightColor : packoffset(c2);
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

#define PI 3.14159265f

static const float Samples = 2.0f;

static const float3 threePrimaryColors = float3(0.68f, 0.55f, 0.44f); // RGB各色の波長(micrometers)
static const float3 InvWaveLength = 1.0f / pow(threePrimaryColors, 4.0f); // 波長の4乗の逆数

static const float ESun = 20.0f; // 太陽の輝度
static const float KrSun = Kr * ESun; // レイリー散乱係数と太陽の輝度の積
static const float KmSun = Km * ESun; // ミー散乱係数と太陽の輝度の積

static const float Kr4PI = Kr * 4.0f * PI; // レイリー散乱定数と4πの積
static const float Km4PI = Km * 4.0f * PI; // ミー散乱定数と4πの積

static const float Scale = 1.0f / (OuterRadius - InnerRadius); // 大気の厚さの逆数
static const float ScaleDepth = 0.25f; // 大気の密度スケール深度
static const float ScaleOverScaleDepth = Scale / ScaleDepth; // 大気の厚さの逆数とスケール深度の比

static const float g = -0.999f; // ミー散乱の非対称パラメータ
static const float g2 = g * g; // gの2乗

float ScaleFunc(float cos)
{
    float x = 1.0f - cos;
    
    return ScaleDepth * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.8f + x * 5.25f))));
}

float4 main(VSOutput input) : SV_TARGET
{    
    float3 worldPos = input.worldPos.xyz;
    float3 camPos = cameraPos;
    camPos.y = InnerRadius; // カメラの高さを地表に固定
    float3 lightDir = -normalize(lightDirection);
    
    float3 ray = worldPos - camPos;
    float far = length(ray);
    ray /= far; // レイの方向を正規化
    
    float3 start = camPos;
    float camHeight = length(camPos);
    float startAngle = dot(ray, camPos) / camHeight;
    float startDepth = exp(ScaleOverScaleDepth * (InnerRadius - camHeight));
    float startOffset = startDepth * ScaleFunc(startAngle);
    
    float sampleLength = far / Samples;
    float scaledLength = sampleLength * Scale;
    float3 sampleRay = ray * sampleLength;
    float3 samplePoint = start + sampleRay * 0.5f;
    
    float3 frontColor = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < int(Samples); ++i)
    {
        float height = length(samplePoint);
        float depth = exp(ScaleOverScaleDepth * (InnerRadius - height));
        float lightAngle = dot(lightDir, samplePoint) / height;
        float camAngle = dot(ray, samplePoint) / height;
        float scatter = (startOffset + depth * (ScaleFunc(lightAngle) - ScaleFunc(camAngle)));
        float3 attenuate = exp(-scatter * (InvWaveLength * (Kr4PI + Km4PI)));
        frontColor += attenuate * (depth * scaledLength);
        samplePoint += sampleRay;
    }
    
    float3 c0 = frontColor * (InvWaveLength * KrSun);
    float3 c1 = frontColor * (KmSun);
    float3 dir = cameraPos - worldPos;
    
    float cos = dot(lightDir, dir) / length(dir);
    float cos2 = cos * cos;
    
    float rayleighPhase = 0.75f * (1.0f + cos2);
    float miePhase = 1.5f * ((1.0f - g2) * (1.0f + cos2)) / pow(1.0f + g2 - 2.0f * g * cos, 1.5f);
    
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    color.rgb = c0 * rayleighPhase + c1 * miePhase;
    return color;
}