#pragma once

inline constexpr char kHitFlashShader[] = R"(
Texture2D baseTexture : register(t0);
SamplerState baseSampler : register(s0);
cbuffer HitFlash : register(b13) { float flashAmount; float hoverRim; float rimWidth; float rimOpacity; };

float4 main(float4 position : SV_Position, float2 uv : TEXCOORD0,
            float3 viewDirection : TEXCOORD1) : SV_Target
{
    float coverage = baseTexture.Sample(baseSampler, uv).a;
    clip(coverage - 0.5);
    // Geometric normals avoid turning normal-map details into bright interior patches.
    float facing = abs(viewDirection.z) / max(length(viewDirection), 0.0001);
    float rim = smoothstep(1.0 - rimWidth, 1.0 - rimWidth / 3.0, 1.0 - saturate(facing)) * hoverRim * rimOpacity;
    float alpha = flashAmount + rim * (1.0 - flashAmount);
    float3 premultiplied = flashAmount.xxx + float3(1.0, 0.08, 0.03) * rim * (1.0 - flashAmount);
    return float4(premultiplied, alpha);
}
)";
