struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
    float4 normal : NORMAL;
};

struct PSOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 worldPos : SV_Target2;
};

SamplerState smp : register(s0);
Texture2D tex : register(t0);

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    output.albedo = tex.Sample(smp, input.uv);
    output.normal.xyz = normalize(input.normal.xyz) * 0.5f + 0.5f; // 法線ベクトルを[0,1]範囲に変換
    output.worldPos.xyz = normalize(input.worldPos.xyz) * 0.5f + 0.5f; // ワールド座標を[0,1]範囲に変換
    
    return output;
}