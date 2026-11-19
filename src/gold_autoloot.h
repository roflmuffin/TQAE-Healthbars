#pragma once

#include "../sdk/GAME/WorldVec3.hpp"

namespace ormenos {
void initialize_gold_autoloot();

inline bool gold_in_pickup_range(const GAME::WorldVec3& player, const GAME::WorldVec3& gold, float range)
{
    if (!player.region || player.region != gold.region) return false;
    const float x = player.position.x - gold.position.x;
    const float y = player.position.y - gold.position.y;
    const float z = player.position.z - gold.position.z;
    return x * x + y * y + z * z <= range * range;
}
}
