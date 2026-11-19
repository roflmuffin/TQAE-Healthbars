#pragma once

#include <algorithm>
#include <cstdint>
#include <string_view>

namespace ormenos {
void initialize_damage_numbers();

constexpr bool is_damage_number_style(std::string_view style)
{
    style.remove_prefix(style.find_last_of("/\\") == std::string_view::npos ? 0 : style.find_last_of("/\\") + 1);
    for (const auto prefix : {std::string_view("damage"), std::string_view("criticalhit")})
    {
        if (style.size() < prefix.size()) continue;
        bool matches = true;
        for (size_t i = 0; i < prefix.size(); ++i)
        {
            const char c = style[i] >= 'A' && style[i] <= 'Z' ? style[i] + ('a' - 'A') : style[i];
            if (c != prefix[i]) matches = false;
        }
        if (matches) return true;
    }
    return false;
}

struct DamageNumberAnimation
{
    float scale;
    float alpha;
    float x;
    float y;
};

constexpr DamageNumberAnimation animate_damage_number(int lifetimeMs, int remainingMs, uint32_t seed)
{
    if (lifetimeMs <= 0) return {1.0f, 0.0f, 0.0f, 0.0f};
    const float progress = std::clamp(1.0f - float(remainingMs) / lifetimeMs, 0.0f, 1.0f);
    const float grow = std::clamp(progress / 0.45f, 0.0f, 1.0f);
    const float shrink = std::clamp((progress - 0.65f) / 0.35f, 0.0f, 1.0f);
    const float growEase = grow * grow * (3.0f - 2.0f * grow);
    const float shrinkEase = shrink * shrink * (3.0f - 2.0f * shrink);
    const float age = progress * lifetimeMs * 0.001f;
    const float travel = age / (1.0f + 0.8f * age);
    const float direction = seed & 1 ? 1.0f : -1.0f;
    return {1.0f + 0.4f * growEase - 0.85f * shrinkEase, 1.0f - shrinkEase,
            direction * (90.0f + float((seed >> 1) % 61)) * travel,
            -(40.0f + float((seed >> 7) % 26)) * travel};
}
}
