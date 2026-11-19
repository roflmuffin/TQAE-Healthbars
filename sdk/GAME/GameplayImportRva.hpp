#pragma once
#include <cstdint>
namespace GAME {
#pragma pack(push, 1)
enum class GameplayImportRva : uint32_t {
    ObjectManagerGet = 2019868,
    IsGameTimePaused = 2020296,
    PutOnFloor = 2020952,
    IsGameWaiting = 2023272,
    GetMainPlayer = 2023664,
    SetTargetPoint = 2023864,
    GetPathPosition = 2023912,
    ForceSetRepeated = 2023916,
    HandleMoveTo = 2023920,
    IsTeleporting = 2023940,
    IsGameLoading = 2023944,
};
#pragma pack(pop)
}
