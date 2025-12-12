struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct PSOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 worldPos : SV_Target2;
};

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    output.albedo = float4(1.0f, 0.0f, 0.0f, 1.0f); // 赤色
    output.normal = float4(0.0f, 0.0f, 1.0f, 0.0f); // 法線ベクトル
    output.worldPos = input.svpos; // ワールド座標
    
    return output;
}