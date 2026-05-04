#define _CRT_SECURE_NO_WARNINGS

#include "shared.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>

#include "../sdk/GAME/Character.hpp"
#include "../sdk/GAME/GraphicsMesh.hpp"
#include "../sdk/GAME/GraphicsMeshInstance.hpp"
#include "../sdk/GAME/Monster.hpp"
#include "../sdk/GAME/Player.hpp"
#include "../vendor/imgui/imgui.h"
#include "d3d11_impl.h"

static int opacity = 255;

using namespace GAME;

void RenderHealthBar(Character* character, ImVec2 pos, ImVec2 size)
{
    float health = character->bio.currentLife;
    float maxHealth = character->bio.maxLife;
    auto* monster = reinterpret_cast<Monster*>(character);
    auto borderThickness = 1.0f;

    switch (monster->classification)
    {
        case MonsterClassification::Champion:
            size.x *= 1.1f;
            size.y += 3;
            borderThickness = 2;
            break;
        case MonsterClassification::Hero:
            size.x *= 1.2f;
            size.y += 5;
            borderThickness = 2;
            break;
        case MonsterClassification::Boss:
            size.x *= 1.3f;
            size.y += 7;
            borderThickness = 2;
        default:
            break;
    }

    ImDrawList* draw = ImGui::GetForegroundDrawList();

    // Calculate health percentage
    float healthPct = health / maxHealth;
    healthPct = std::min(healthPct, 1.0f);

    // Center the health bar on the position
    ImVec2 topLeft = ImVec2(pos.x - (size.x * 0.5f), pos.y);
    ImVec2 bottomRight = ImVec2(pos.x + (size.x * 0.5f), pos.y + size.y);

    // Background (empty health)
    draw->AddRectFilled(topLeft, bottomRight, IM_COL32(0, 0, 0, opacity));

    // Health fill
    ImU32 healthColor = IM_COL32(255, 0, 0, opacity); // Red
    if (healthPct > 0.5f) healthColor = IM_COL32(0, 255, 0, opacity); // Green
    else if (healthPct > 0.25f)
        healthColor = IM_COL32(255, 165, 0, opacity); // Orange

    draw->AddRectFilled(topLeft, ImVec2(topLeft.x + (size.x * healthPct), pos.y + size.y), healthColor);

    // Border
    draw->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 0, opacity), 0.0f, 0, borderThickness);

    // Optional: Health text
    char text[64];
    snprintf(text, sizeof(text), "%.0f / %.0f", health, maxHealth);

    ImVec2 textSize = ImGui::CalcTextSize(text);

    draw->AddText(ImVec2(pos.x - (textSize.x * 0.5f), pos.y - 22 + ((size.y - textSize.y) * 0.5f)), IM_COL32(255, 255, 255, opacity), text);

    char text2[64];
    snprintf(text2, sizeof(text2), "%p", character);

    ImVec2 textSize2 = ImGui::CalcTextSize(text2);

    draw->AddText(ImVec2(pos.x - (textSize2.x * 0.5f), pos.y - 22 - 20 + ((size.y - textSize2.y) * 0.5f)), IM_COL32(255, 255, 255, opacity),
                  text2);
}

static bool showFullHealthEnemies = true;

void impl::showExampleWindow(const char* comment)
{
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(420, 220), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("KieroHookTest"))
    {
        ImGui::Text("Renderer: %s", comment);
        ImGui::Text("Engine Pointer: %p", impl::state::g_pEngine);
        ImGui::Text("Game Engine Pointer: %p", impl::state::g_pGameEngine);
        ImGui::Text("Camera Pointer: %p", impl::state::g_pGameCamera);
        ImGui::Text("Viewport: %i x %i", impl::state::g_pViewport.w, impl::state::g_pViewport.h);

        if (impl::state::g_pGameCamera && impl::state::g_pGameCamera->player)
        {
            ImGui::Text("Player Pointer: %p", impl::state::g_pGameCamera->player);
            ImGui::Checkbox("Player Invisible", &impl::state::g_pGameCamera->player->isInvisible);
        }
        else
        {
            ImGui::Text("Player Pointer: <null>");
        }

        ImGui::Separator();
        ImGui::Text("Characters: %zu", impl::state::allCharacters.size());
        ImGui::Checkbox("Show Full Health Enemies", &showFullHealthEnemies);
        ImGui::SliderInt("Opacity", &opacity, 0, 255);
    }
    ImGui::End();

    if (!impl::state::g_pGameCamera || !impl::state::g_pGameCamera->player || !impl::state::fnWorldCameraProject)
    {
        return;
    }

    for (auto* character : impl::state::allCharacters)
    {
        if (!character)
        {
            continue;
        }

        if (character->lifeState != LifeState::Alive)
        {
            continue;
        }

        if (!showFullHealthEnemies && character->bio.currentLife >= character->bio.maxLife)
        {
            continue;
        }

        if (character->coords.location.region == nullptr) continue;
        if (character->graphicsMeshInstance == nullptr) continue;

        // Get head position in world coordinates
        WorldVec3 headCoords;
        headCoords = { .region = character->coords.location.region,
                       .position = { .x = character->coords.location.position.x - (character->graphicsMeshInstance->somethingX / 2),
                                     .y = character->coords.location.position.y +
                                          ((character->graphicsMeshInstance->somethingY + character->height)),
                                     .z = character->coords.location.position.z - (character->graphicsMeshInstance->somethingX / 2) } };

        // WorldVec3 centreOfMassCoords = {
        //     .region = character->coords.location.region,
        //     .position = {
        //         .x = character->coords.location.position.x +
        //         character->graphicsMeshInstance->graphicsMesh->centerOfMass.x, .y
        //         = character->coords.location.position.y +
        //         character->graphicsMeshInstance->graphicsMesh->centerOfMass.y +
        //         (character->height), .z = character->coords.location.position.z +
        //         character->graphicsMeshInstance->graphicsMesh->centerOfMass.z
        //     }
        // };

        Vec2 screenPos{};
        impl::state::fnWorldCameraProject(impl::state::g_pGameCamera, &screenPos, &headCoords, &impl::state::g_pViewport);

        if (screenPos.x <= 0.0f || screenPos.y <= 0.0f)
        {
            continue;
        }

        RenderHealthBar(character, ImVec2(screenPos.x, screenPos.y), ImVec2(75, 6));
    }
}
