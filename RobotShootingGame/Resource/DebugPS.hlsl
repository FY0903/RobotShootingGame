struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = float4(1, 1, 1, 1);
    float2 halfGrid = floor(abs(input.uv) * 2.0f);
    float2 quatGrid = floor(abs(input.uv) * 8.0f);

    float fHalf = fmod(halfGrid.x + halfGrid.y, 2.0f);
    float quat = fmod(quatGrid.x + quatGrid.y, 2.0f);

    color.rgb = ((fHalf * 0.1f) * quat + 0.45f) + (1 - quat) * 0.05f;
    return color;
}