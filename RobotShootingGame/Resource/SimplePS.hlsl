struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PSOutput
{
    float4 Color : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput)0;
    output.Color = input.color;
    
    return output;
}