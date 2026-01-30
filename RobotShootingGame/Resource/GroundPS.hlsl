struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 lightVP : TEXCOORD1;
};

SamplerState smp : register(s0);
SamplerComparisonState cmpSmp : register(s1);
Texture2D<float4> tex : register(t0);
Texture2D<float> shadowMap : register(t1);

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);
    
    // NDC を [0,1] UV に変換
    float2 shadowUV = input.lightVP.xy * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y;
    
    if (shadowUV.x > 0.0f && shadowUV.x < 1.0f
        && shadowUV.y > 0.0f && shadowUV.y < 1.0f)
    {
        float4 shadow = shadowMap.Sample(smp, shadowUV);
        color.rgb *= shadow.rgb;
    }
    
    return color;
    
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