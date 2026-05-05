#include <cmath>
#define _CRT_SECURE_NO_WARNINGS

#include "shared.h"
#include <algorithm>
#include <cfloat>
#include <cstdint>
#include <cstdio>
#include <string>
#include <unordered_map>

#include "../sdk/GAME/Character.hpp"
#include "../sdk/GAME/GraphicsMesh.hpp"
#include "../sdk/GAME/GraphicsMeshInstance.hpp"
#include "../sdk/GAME/Monster.hpp"
#include "../sdk/GAME/Player.hpp"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "d3d11_impl.h"

static int opacity = 255;

using namespace GAME;

namespace {
struct HealthBarAnimState
{
    float displayedHealth = 0.0f;
    float trailingHealth = 0.0f;
    float trailingDelay = 0.0f;
    float damageFlashTimer = 0.0f;
    float damageFlashHealth = 0.0f;
    bool initialized = false;
};

std::unordered_map<Character*, HealthBarAnimState> g_healthBarStates;

} // namespace

static void RenderPointerRow(const char* name, const void* pointer)
{
    char pointerText[32];
    snprintf(pointerText, sizeof(pointerText), "%p", pointer);

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(name);

    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(pointerText);

    ImGui::TableSetColumnIndex(2);
    std::string buttonLabel = std::string("Copy##") + name;
    if (ImGui::SmallButton(buttonLabel.c_str()))
    {
        ImGui::SetClipboardText(pointerText);
    }
}

struct HealthBarConfig
{
    bool showFullHealthEnemies = true;
    bool showHealthBarForPlayers = true;
    bool showHealthBarForMonsters = true;
    bool showHealthAmountTextForPlayers = true;
    bool showHealthAmountTextForMonsters = false;
    bool showMinionHealthBars = false;
    bool showDebugText = false;

    ImVec2 size = ImVec2(55, 5);
    float trailingBarDelaySeconds = 0.20f;
    float trailingBarEaseSpeed = 8.0f;
    float damageFlashDurationSeconds = 0.18f;
    float damageFlashWidth = 2.0f;

    ImColor friendlyColor = ImColor(22, 145, 22, 255);
    ImColor enemyColor = ImColor(164, 28, 24, 255);
    ImColor trailingBarColor = ImColor(174, 109, 109, 255);
};

static HealthBarConfig g_healthBarConfig = {};

void RenderHealthBar(Character* character, ImVec2 pos, ImVec2 size)
{
    const float scaleFactor = std::max(impl::state::globalScaleFactor, 0.01f);
    size.x *= scaleFactor;
    size.y *= scaleFactor;

    float health = character->bio.currentLife;
    float maxHealth = character->bio.maxLife;
    auto* monster = reinterpret_cast<Monster*>(character);
    auto borderThickness = 1.0f * scaleFactor;

    auto* player = impl::state::g_pGameCamera ? impl::state::g_pGameCamera->player : nullptr;
    auto isPlayer = character->team.major == GAME::TeamMajor_Human;
    auto isMonster = character->team.major == GAME::TeamMajor_Monster;
    auto isPlayerMinion = character->team.major == GAME::TeamMajor_Human &&
                          static_cast<uint32_t>(character->team.minor) == (player ? player->objectId : -1) &&
                          character->objectId != (player ? player->objectId : -1);

    if (isPlayerMinion && !g_healthBarConfig.showMinionHealthBars)
    {
        return;
    }

    if (maxHealth <= 0.0f)
    {
        return;
    }

    switch (monster->classification)
    {
        case MonsterClassification::Champion:
            size.x *= 1.5f;
            size.y += 4.0f * scaleFactor;
            borderThickness = 2.0f * scaleFactor;
            break;
        case MonsterClassification::Hero:
            size.x *= 1.75f;
            size.y += 6.0f * scaleFactor;
            borderThickness = 2.0f * scaleFactor;
            break;
        case MonsterClassification::Boss:
        case MonsterClassification::Quest:
            size.x *= 2.0f;
            size.y += 7.0f * scaleFactor;
            borderThickness = 2.0f * scaleFactor;

        default:
            break;
    }

    ImDrawList* draw = ImGui::GetForegroundDrawList();
    const ImVec2 shadowOffset(2.0f * scaleFactor, 2.0f * scaleFactor);
    ImFont* font = ImGui::GetFont();
    const float fontSize = font ? (font->LegacySize * scaleFactor) : (ImGui::GetFontSize() * scaleFactor);

    // Calculate health percentage
    float healthPct = std::clamp(health / maxHealth, 0.0f, 1.0f);

    ImGuiIO& io = ImGui::GetIO();
    auto& animState = g_healthBarStates[character];
    if (!animState.initialized)
    {
        animState.displayedHealth = healthPct;
        animState.trailingHealth = healthPct;
        animState.damageFlashHealth = healthPct;
        animState.initialized = true;
    }

    const float previousDisplayedHealth = animState.displayedHealth;
    animState.displayedHealth = healthPct;

    if (healthPct < previousDisplayedHealth)
    {
        animState.trailingDelay = g_healthBarConfig.trailingBarDelaySeconds;
        animState.damageFlashTimer = g_healthBarConfig.damageFlashDurationSeconds;
        animState.damageFlashHealth = healthPct;
    }
    else if (healthPct > previousDisplayedHealth)
    {
        animState.damageFlashTimer = 0.0f;
        animState.damageFlashHealth = healthPct;
    }

    if (healthPct > animState.trailingHealth)
    {
        animState.trailingHealth = healthPct;
        animState.trailingDelay = 0.0f;
    }
    else if (healthPct < animState.trailingHealth)
    {
        if (animState.trailingDelay > 0.0f)
        {
            animState.trailingDelay = std::max(0.0f, animState.trailingDelay - io.DeltaTime);
        }
        else
        {
            const float easeAlpha = 1.0f - std::exp(-g_healthBarConfig.trailingBarEaseSpeed * io.DeltaTime);
            animState.trailingHealth = std::max(healthPct, animState.trailingHealth + ((healthPct - animState.trailingHealth) * easeAlpha));
        }
    }

    animState.damageFlashTimer = std::max(0.0f, animState.damageFlashTimer - io.DeltaTime);

    // Center the health bar on the position
    ImVec2 topLeft = ImVec2(pos.x - (size.x * 0.5f), pos.y);
    ImVec2 bottomRight = ImVec2(pos.x + (size.x * 0.5f), pos.y + size.y);

    // Background (empty health)
    draw->AddRectFilled(topLeft, bottomRight, IM_COL32(7, 7, 7, opacity));

    ImU32 healthColor = IM_COL32(0, 255, 0, opacity);
    if (character->team.major == GAME::TeamMajor_Friendly || character->team.major == GAME::TeamMajor_Human)
    {
        healthColor = g_healthBarConfig.friendlyColor; // Green for friendly
    }
    else
    {
        healthColor = g_healthBarConfig.enemyColor; // Red for enemies
    }

    draw->AddRectFilled(topLeft, ImVec2(topLeft.x + (size.x * healthPct), pos.y + size.y), healthColor);

    if (animState.trailingHealth > healthPct)
    {
        const float currentHealthX = topLeft.x + (size.x * healthPct);
        const float trailingHealthX = topLeft.x + (size.x * animState.trailingHealth);
        draw->AddRectFilled(ImVec2(currentHealthX, topLeft.y), ImVec2(trailingHealthX, bottomRight.y), g_healthBarConfig.trailingBarColor);
    }

    // Border
    draw->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 0, opacity), 0.0f, 0, borderThickness);

    if (animState.damageFlashTimer > 0.0f && animState.damageFlashHealth > 0.0f)
    {
        const float flashAlpha = animState.damageFlashTimer / g_healthBarConfig.damageFlashDurationSeconds;
        const float flashX = topLeft.x + (size.x * animState.damageFlashHealth);
        const float flashHalfWidth = (g_healthBarConfig.damageFlashWidth * scaleFactor) * 0.5f;
        draw->AddRectFilled(ImVec2(flashX - flashHalfWidth, topLeft.y - scaleFactor - borderThickness),
                            ImVec2(flashX + flashHalfWidth, bottomRight.y + scaleFactor + borderThickness),
                            IM_COL32(255, 255, 255, static_cast<int>(opacity * flashAlpha)));
    }

    if (g_healthBarConfig.showHealthAmountTextForMonsters || g_healthBarConfig.showHealthAmountTextForPlayers)
    {
        if (isPlayer && !g_healthBarConfig.showHealthAmountTextForPlayers)
        {
            return;
        }

        if (isMonster && !g_healthBarConfig.showHealthAmountTextForMonsters)
        {
            return;
        }

        // Optional: Health text
        char text[64];
        snprintf(text, sizeof(text), "%d / %d", (int)std::ceil(health), (int)std::ceil(maxHealth));

        ImVec2 textSize = font ? font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text) : ImGui::CalcTextSize(text);

        ImVec2 textPos(pos.x - (textSize.x * 0.5f), pos.y - (22.0f * scaleFactor) + ((size.y - textSize.y) * 0.5f));
        draw->AddText(font, fontSize, ImVec2(textPos.x + shadowOffset.x, textPos.y + shadowOffset.y), IM_COL32(0, 0, 0, opacity), text);
        draw->AddText(font, fontSize, textPos, IM_COL32(255, 255, 255, opacity), text);
    }

    if (g_healthBarConfig.showDebugText)
    {
        std::wstring description;
        const char* descriptionTag = character->descriptionTag.c_str();
        if (descriptionTag && *descriptionTag)
        {
            description = std::wstring(impl::state::fnLocalizationGetText(impl::state::g_pLocalizationManager, descriptionTag));
        }
        char text2[255];
        snprintf(text2, sizeof(text2), "%p, %ls", character, description.c_str());

        ImVec2 textSize2 = font ? font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text2) : ImGui::CalcTextSize(text2);

        ImVec2 textPos2(pos.x - (textSize2.x * 0.5f), pos.y - (42.0f * scaleFactor) + ((size.y - textSize2.y) * 0.5f));
        draw->AddText(font, fontSize, ImVec2(textPos2.x + shadowOffset.x, textPos2.y + shadowOffset.y), IM_COL32(0, 0, 0, opacity), text2);
        draw->AddText(font, fontSize, textPos2, IM_COL32(255, 255, 255, opacity), text2);
    }
}

void impl::showExampleWindow(const char* comment)
{
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_::ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_::ImGuiCond_FirstUseEver);

    if (ImGui::Begin("KieroHookTest"))
    {
        ImGui::Text("Renderer: %s", comment);

        if (ImGui::BeginTable("Pointers", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.40f);
            ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthStretch, 0.45f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch, 0.15f);
            ImGui::TableHeadersRow();

            RenderPointerRow("Engine Base", reinterpret_cast<const void*>(impl::state::engineBase));
            RenderPointerRow("Game Base", reinterpret_cast<const void*>(impl::state::gameBase));
            RenderPointerRow("Engine Pointer", impl::state::g_pEngine);
            RenderPointerRow("Game Engine Pointer", impl::state::g_pGameEngine);
            RenderPointerRow("Camera Pointer", impl::state::g_pGameCamera);
            RenderPointerRow("Localization Manager Pointer", impl::state::g_pLocalizationManager);
            RenderPointerRow("TimeScale Pointer", impl::state::g_pTimeScale);

            auto* player = impl::state::g_pGameCamera ? impl::state::g_pGameCamera->player : nullptr;
            RenderPointerRow("Player Pointer", player);

            ImGui::EndTable();

            if (player)
            {
                ImGui::Checkbox("Player Invisible", &player->isInvisible);
            }
        }

        ImGui::Text("Viewport: %i x %i", impl::state::g_pViewport.w, impl::state::g_pViewport.h);
        ImGui::Separator();
        ImGui::Text("Characters: %zu", impl::state::allCharacters.size());
        ImGui::SliderInt("Opacity", &opacity, 0, 255);
        if (impl::state::g_pTimeScale)
        {
            ImGui::SliderFloat("Time Scale", impl::state::g_pTimeScale, 0.0f, 10.0f, "%.2f");
        }

        if (ImGui::Button("Increment Enemy Rarity"))
        {
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

                auto* monster = reinterpret_cast<Monster*>(character);

                if (monster->classification == MonsterClassification::Quest)
                {
                    monster->classification = MonsterClassification::Normal;
                }
                else
                {
                    monster->classification = static_cast<MonsterClassification>(static_cast<int>(monster->classification) + 1);
                }
            }
        }

        if (ImGui::CollapsingHeader("Health Bar Config"))
        {
            ImGui::Checkbox("Show on Full Health Enemies", &g_healthBarConfig.showFullHealthEnemies);
            ImGui::Checkbox("Show Health Bar for Players", &g_healthBarConfig.showHealthBarForPlayers);
            ImGui::Checkbox("Show Health Bar for Monsters", &g_healthBarConfig.showHealthBarForMonsters);
            ImGui::Checkbox("Show Health Amount Text for Players", &g_healthBarConfig.showHealthAmountTextForPlayers);
            ImGui::Checkbox("Show Health Amount Text for Monsters", &g_healthBarConfig.showHealthAmountTextForMonsters);
            ImGui::Checkbox("Display Minion Health Bars", &g_healthBarConfig.showMinionHealthBars);
            ImGui::Checkbox("Show Debug Text", &g_healthBarConfig.showDebugText);
            ImGui::Separator();
            ImGui::SliderFloat2("Size", (float*)&g_healthBarConfig.size, 5.0f, 200.0f, "%.0f");
            ImGui::SliderFloat("Trailing Bar Delay (seconds)", &g_healthBarConfig.trailingBarDelaySeconds, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("Trailing Bar Ease Speed", &g_healthBarConfig.trailingBarEaseSpeed, 1.0f, 30.0f, "%.1f");
            ImGui::SliderFloat("Damage Flash Duration (seconds)", &g_healthBarConfig.damageFlashDurationSeconds, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("Damage Flash Width", &g_healthBarConfig.damageFlashWidth, 1.0f, 10.0f, "%.1f");
            ImGui::ColorEdit4("Friendly Color", (float*)&g_healthBarConfig.friendlyColor);
            ImGui::ColorEdit4("Enemy Color", (float*)&g_healthBarConfig.enemyColor);
            ImGui::ColorEdit4("Trailing Bar Color", (float*)&g_healthBarConfig.trailingBarColor);
        }
    }
    ImGui::End();

    if (!impl::state::g_pGameCamera || !impl::state::g_pGameCamera->player || !impl::state::fnWorldCameraProject)
    {
        return;
    }

    for (auto it = g_healthBarStates.begin(); it != g_healthBarStates.end();)
    {
        if (!it->first || impl::state::allCharacters.find(it->first) == impl::state::allCharacters.end())
        {
            it = g_healthBarStates.erase(it);
        }
        else
        {
            ++it;
        }
    }

    ImGui::PushFont(impl::state::villageFont);
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

        if (!g_healthBarConfig.showFullHealthEnemies && character->bio.currentLife >= character->bio.maxLife)
        {
            continue;
        }

        if (character->team.major == GAME::TeamMajor_Friendly ||
            (character->team.major == GAME::TeamMajor_Human && character->team.minor == GAME::TeamMinor_Friendly))
        {
            continue;
        }

        if (!g_healthBarConfig.showHealthBarForPlayers && character->team.major == GAME::TeamMajor_Human)
        {
            continue;
        }

        if (!g_healthBarConfig.showHealthBarForMonsters && character->team.major == GAME::TeamMajor_Monster)
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

        Vec2 screenPos{};
        impl::state::fnWorldCameraProject(impl::state::g_pGameCamera, &screenPos, &headCoords, &impl::state::g_pViewport);

        if (screenPos.x <= 0.0f || screenPos.y <= 0.0f)
        {
            continue;
        }

        RenderHealthBar(character, ImVec2(screenPos.x, screenPos.y), g_healthBarConfig.size);
    }
    ImGui::PopFont();
}
