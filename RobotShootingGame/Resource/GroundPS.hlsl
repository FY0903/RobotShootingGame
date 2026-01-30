struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 worldPos : TEXCOORD1;
};

cbuffer Light : register(b1)
{
    float4x4 View : packoffset(c0);
    float4x4 Proj : packoffset(c4);
}

SamplerState smp : register(s0);
SamplerComparisonState cmpSmp : register(s1);
Texture2D<float4> tex : register(t0);
Texture2D<float> shadowMap : register(t1);

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);

    // ワールド位置をライト空間に投影
    float4 lightPos = float4(input.worldPos, 1.0f);
    lightPos = mul(View, lightPos);
    lightPos = mul(Proj, lightPos);

    // UV に変換（通常の手順）
    float2 shadowUV = lightPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y;

    float4 shadow = shadowMap.Sample(smp, shadowUV);
    
    return shadow;
    
    //// 深度（NDCの z）。DirectX を想定して [0,1] の範囲になっている前提。
    //float writeDepth = lightPos.z / lightPos.w;

    //// バイアスを差し引いて比較（範囲外のUVはクランプするか扱いを決める）
    //float bias = 0.005f;
    //// ハードウェア比較を使用（SampleCmp は参照 <= テクセル ? 1 : 0 を返す（フィルタ有効時は 0..1））
    //float cmp = shadowMap.SampleCmp(cmpSmp, shadowUV, writeDepth - bias);

    //// cmp が 1 = ライトから見て可視（影なし）、0 = 覆われている（影）
    //// そのまま色に乗算すれば影の強さになる
    //color.rgb *= cmp;

    //return color;
}