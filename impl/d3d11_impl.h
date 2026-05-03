#pragma once


#include <set>

#include "shared.h"
#include "../sdk/Engine.hpp"
#include "../sdk/Entity.hpp"
#include "../sdk/GameCamera.hpp"
#include "../sdk/Vec2.hpp"
#include "../sdk/Viewport.hpp"
#include "../sdk/WorldCoords.hpp"

struct Character;

namespace impl {
    namespace d3d11 {
        void init();
    }

    namespace state {
        inline Engine *g_pEngine;
        inline Engine *g_pGameEngine;
        inline GameCamera *g_pGameCamera;
        inline Viewport g_pViewport {};
        inline std::set<Character*> allCharacters{};

        typedef void (__thiscall  *ProjectFunc)(WorldCamera* pThis, Vec2* result, WorldVec3* worldPos, Viewport* viewport);

        inline ProjectFunc fnWorldCameraProject = nullptr;
    }
}
