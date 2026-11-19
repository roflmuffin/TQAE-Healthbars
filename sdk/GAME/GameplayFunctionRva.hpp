#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
enum class GameplayFunctionRva : uint32_t {
    FindControllerPlayer = 29440,
    ResolvePortalTarget = 345440,
    UpdateStopTimer = 352672,
    HandleKeyEvent = 362304,
    SkillInProgress = 367120,
    HandleMouseEvent = 371280,
    HasOpenWindow = 1279440,
};
#pragma pack(pop)
}
