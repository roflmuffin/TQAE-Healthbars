#pragma once
#include <cstdint>
#include "GameString.hpp"
namespace GAME {
struct EntityList;
}
namespace GAME {
struct Region;
}
namespace GAME {
struct WorldFrustum;
}
namespace GAME {
#pragma pack(push, 1)
struct World {
    private: char pad_0[0x8]; public:
    bool debugEnabled; // 0x8
    private: char pad_9[0x7]; public:
    GAME::GameString fileName; // 0x10
    void GetEntitiesInFrustum(GAME::EntityList* entities, GAME::WorldFrustum* frustum, bool flag, int listType, bool loaded, bool recurse);
    bool AreRegionsConnected(GAME::Region* first, GAME::Region* second, int depth);
}; // Size: 0x28
#pragma pack(pop)
}
