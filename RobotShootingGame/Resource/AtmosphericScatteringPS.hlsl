struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer CB : register(b1)
{
    float3 frustumRayTL : packoffset(c0);
    float3 frustumRayTR : packoffset(c1);
    float3 frustumRayBL : packoffset(c2);
    uint2 windowSize : packoffset(c3);
}

cbuffer SkyCB : register(b2)
{
    float3 cameraPos;
    float cameraHeight;
    
    float3 lightPos;
    float cameraHeight2;
    
    float3 invWavelength;
    float scale;
    
    float outerRadius;
    float outerRadius2;
    float innerRadius;
    float innerRadius2;
    
    float krESun;
    float kmESun;
    float kr4PI;
    float km4PI;
    
    float scaleDepth;
    float scaleOverScaleDepth;
    float g;
    float exposure;
}

float3 IntersectionPos(float3 rayPos, float3 rayDir, float sphereRadius)
{
    const float A = dot(rayDir, rayDir);
    const float B = 2.0f * dot(rayDir, rayPos);
    const float C = dot(rayPos, rayPos) - sphereRadius * sphereRadius;
    const float discriminant = B * B - 4.0f * A * C;
    
    if (discriminant < 0.000001f)
    {
        return float3(0.0f, 0.0f, 0.0f); // No intersection
    }
    
    const float t = 0.5f * (-B + sqrt(discriminant)) / A;
    return rayPos + t * rayDir;
}

float IntegralApproximation(float fCos)
{
    float x = 1.0f - fCos;
    return scaleDepth * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.80f + x * 5.25f))));
}

float4 main(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv;
    
    float3 deltaX = (frustumRayTR - frustumRayTL) * uv.x;
    float3 deltaY = (frustumRayBL - frustumRayTL) * uv.y;
    float3 rayDir = normalize(frustumRayTL + deltaX + deltaY);
    
    float3 skyPos = IntersectionPos(cameraPos, rayDir, outerRadius);
    if (!skyPos)
    {
        clip(-1.0f); // No intersection with the sky sphere
    }
    
    float3 pos = skyPos;
    float3 ray = pos - cameraPos;
    float far = length(ray);
    ray /= far; // Normalize ray
    
    float3 start = cameraPos;
    float height = length(start);
    float depth = exp(scaleOverScaleDepth * (innerRadius - height));
    float startAngle = dot(ray, start) / height;
    float startOffset = depth * IntegralApproximation(startAngle);
    
    const float samples = 5.0f;
    
    float sampleLength = far / samples;
    float scaledLength = sampleLength * scale;
    float3 sampleRay = ray * sampleLength;
    float3 samplePoint = start + sampleRay * 0.5f;
    
    float3 frontColor = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < (int)samples; i++)
    {
        float height = length(samplePoint);
        float depth = exp(scaleOverScaleDepth * (innerRadius - height));
        float lightAngle = dot(lightPos, samplePoint) / height;
        float cameraAngle = dot(ray, samplePoint) / height;
        float scatter = startOffset + depth * (IntegralApproximation(lightAngle) - IntegralApproximation(cameraAngle));
        
        float3 attenuate = exp(-scatter * (invWavelength * kr4PI + km4PI));
        frontColor += attenuate * (depth * scaledLength);
        
        samplePoint += sampleRay;
    }
    
    float4 primaryColor = float4(frontColor * (invWavelength * krESun), 1.0f);
    float4 secondaryColor = float4(frontColor * kmESun, 1.0f);
    float3 direction = cameraPos - pos;
    
    float fCos = dot(lightPos, direction) / length(direction);
    float rayPhase = 0.75f * (1.0f + fCos * fCos);
    
    const float g2 = g * g;
    float miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + fCos * fCos) / pow(abs(1.0f + g2 - 2.0f * g * fCos), 1.5f);
    
    float3 rayColor = primaryColor * rayPhase;
    float3 mieColor = secondaryColor * miePhase;
    
    float3 c = float3(1.0f, 1.0f, 1.0f) - exp(-exposure * (rayColor + mieColor));
    
    return float4(saturate(c), 1.0f);
}
