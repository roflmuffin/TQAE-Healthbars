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
#include "health_bar_config.h"

static int opacity = 255;

using namespace GAME;
using ormenos::g_healthBarConfig;

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

std::unordered_map<MonsterClassification, ImColor> g_monsterClassificationColors = {
    { MonsterClassification::Normal, ImColor(255, 255, 255, 255) },
    { MonsterClassification::Champion, ImColor(252, 252, 47, 255) },
    { MonsterClassification::Hero, ImColor(252, 177, 82, 255) },
    { MonsterClassification::Boss, ImColor(252, 71, 0, 255) },
    { MonsterClassification::Quest, ImColor(204, 160, 215, 255) }
};

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

static void RenderHealthBar(ImDrawList& drawList, Character* character, ImVec2 pos, ImVec2 size)
{
    const float scaleFactor = std::max(ormenos::state::globalScaleFactor, 0.01f);
    size.x *= scaleFactor;
    size.y *= scaleFactor;

    ImVec2 runningPos = ImVec2(pos.x, pos.y);

    float health = character->bio.currentLife;
    float maxHealth = character->bio.maxLife;
    auto* monster = reinterpret_cast<Monster*>(character);
    auto borderThickness = 1.0f * scaleFactor;

    auto* player = ormenos::state::g_pGameCamera ? ormenos::state::g_pGameCamera->player : nullptr;
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

    ImDrawList* draw = &drawList;
    const ImVec2 shadowOffset(1.0f * scaleFactor, 1.0f * scaleFactor);
    ImFont* font = ormenos::state::villageFont ? ormenos::state::villageFont : ImGui::GetFont();
    const float fontSize = font ? (font->LegacySize * scaleFactor) : (ImGui::GetFontSize() * scaleFactor);
    const float healthBarFontSize = g_healthBarConfig.baseFontSize * scaleFactor;

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
    draw->AddRectFilled(topLeft, bottomRight, IM_COL32(7, 7, 7, g_healthBarConfig.opacity));

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
    draw->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 0, g_healthBarConfig.opacity), 0.0f, 0, borderThickness);

    if (animState.damageFlashTimer > 0.0f && animState.damageFlashHealth > 0.0f)
    {
        const float flashAlpha = animState.damageFlashTimer / g_healthBarConfig.damageFlashDurationSeconds;
        const float flashX = topLeft.x + (size.x * animState.damageFlashHealth);
        const float flashHalfWidth = (g_healthBarConfig.damageFlashWidth * scaleFactor) * 0.5f;
        draw->AddRectFilled(ImVec2(flashX - flashHalfWidth, topLeft.y - scaleFactor - borderThickness),
                            ImVec2(flashX + flashHalfWidth, bottomRight.y + scaleFactor + borderThickness),
                            IM_COL32(255, 255, 255, static_cast<int>(g_healthBarConfig.opacity * flashAlpha)));
    }

    const float textPadding = g_healthBarConfig.textPadding * scaleFactor;

    runningPos.y -= (size.y * 0.5f);
    runningPos.y -= textPadding * 2.0f;

    if (g_healthBarConfig.showHealthAmountTextForMonsters || g_healthBarConfig.showHealthAmountTextForPlayers)
    {
        if ((isPlayer && g_healthBarConfig.showHealthAmountTextForPlayers) ||
            (isMonster && g_healthBarConfig.showHealthAmountTextForMonsters))
        {
            // Optional: Health text
            char text[64];
            snprintf(text, sizeof(text), "%d / %d", (int)std::ceil(health), (int)std::ceil(maxHealth));

            ImVec2 textSize = font ? font->CalcTextSizeA(healthBarFontSize, FLT_MAX, 0.0f, text) : ImGui::CalcTextSize(text);

            ImVec2 textPos(runningPos.x - (textSize.x * 0.5f), runningPos.y + ((size.y - textSize.y) * 0.5f));
            draw->AddText(font, healthBarFontSize, ImVec2(textPos.x + shadowOffset.x, textPos.y + shadowOffset.y),
                          IM_COL32(0, 0, 0, g_healthBarConfig.opacity), text);
            draw->AddText(font, healthBarFontSize, textPos, IM_COL32(255, 255, 255, g_healthBarConfig.opacity), text);

            runningPos.y -= textSize.y + textPadding;
        }
    }

    if (isMonster)
    {
        auto* monster = reinterpret_cast<Monster*>(character);
        if (static_cast<uint32_t>(monster->classification) > 0 || character->graphicsMeshInstance->highlight)
        {
            std::wstring description;
            const char* descriptionTag = character->descriptionTag.c_str();
            if (descriptionTag && *descriptionTag)
            {
                description = std::wstring(ormenos::state::fnLocalizationGetText(ormenos::state::g_pLocalizationManager, descriptionTag));
            }
            char text2[255];
            snprintf(text2, sizeof(text2), "%ls", description.c_str());

            ImVec2 textSize2 = font ? font->CalcTextSizeA(healthBarFontSize, FLT_MAX, 0.0f, text2) : ImGui::CalcTextSize(text2);

            ImVec2 textPos2(runningPos.x - (textSize2.x * 0.5f), runningPos.y + ((size.y - textSize2.y) * 0.5f));
            draw->AddText(font, healthBarFontSize, ImVec2(textPos2.x + shadowOffset.x, textPos2.y + shadowOffset.y),
                          IM_COL32(0, 0, 0, g_healthBarConfig.opacity), text2);

            g_monsterClassificationColors[monster->classification].Value.w = g_healthBarConfig.opacity / 255.0f;
            draw->AddText(font, healthBarFontSize, textPos2, g_monsterClassificationColors[monster->classification], text2);

            runningPos.y -= textSize2.y + textPadding;
        }
    }

    if (g_healthBarConfig.showDebugText)
    {
        std::wstring description;
        const char* descriptionTag = character->descriptionTag.c_str();
        if (descriptionTag && *descriptionTag)
        {
            description = std::wstring(ormenos::state::fnLocalizationGetText(ormenos::state::g_pLocalizationManager, descriptionTag));
        }
        char text2[255];
        snprintf(text2, sizeof(text2), "%p, %ls", character, description.c_str());

        ImVec2 textSize2 = font ? font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text2) : ImGui::CalcTextSize(text2);

        ImVec2 textPos2(pos.x - (textSize2.x * 0.5f), pos.y + ((size.y - textSize2.y) * 0.5f));
        draw->AddText(font, fontSize, ImVec2(textPos2.x + shadowOffset.x, textPos2.y + shadowOffset.y),
                      IM_COL32(0, 0, 0, g_healthBarConfig.opacity), text2);
        draw->AddText(font, fontSize, textPos2, IM_COL32(255, 255, 255, g_healthBarConfig.opacity), text2);

        runningPos.y -= textSize2.y + textPadding;
    }
}

void ormenos::render_health_bars(ImDrawList& draw)
{
    if (!ormenos::state::g_pGameCamera || !ormenos::state::g_pGameCamera->player || !ormenos::state::fnWorldCameraProject)
    {
        return;
    }

    for (auto it = g_healthBarStates.begin(); it != g_healthBarStates.end();)
    {
        if (!it->first || ormenos::state::allCharacters.find(it->first) == ormenos::state::allCharacters.end())
        {
            it = g_healthBarStates.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (auto* character : ormenos::state::allCharacters)
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
        ormenos::state::fnWorldCameraProject(ormenos::state::g_pGameCamera, &screenPos, &headCoords, &ormenos::state::g_pViewport);

        if (screenPos.x <= 0.0f || screenPos.y <= 0.0f)
        {
            continue;
        }

        RenderHealthBar(draw, character, ImVec2(screenPos.x, screenPos.y), g_healthBarConfig.size);
    }
}

bool m_is_ui_focused = false;
bool m_draw_ui = false;
bool m_last_draw_ui = false;
bool m_ui_option_transparent = false;

void ormenos::show_config_window()
{
    m_is_ui_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

    if (m_ui_option_transparent)
    {
        auto& style = ImGui::GetStyle();
        if (m_is_ui_focused)
        {
            style.Alpha = 1.0f;
        }
        else
        {
            if (ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow))
            {
                style.Alpha = 0.9f;
            }
            else
            {
                style.Alpha = 0.8f;
            }
        }
    }
    else
    {
        auto& style = ImGui::GetStyle();
        style.Alpha = 1.0f;
    }

    auto fontSize = ImGui::GetFontSize();
    ImGui::SetNextWindowPos(ImVec2(fontSize * 2, fontSize * 2), ImGuiCond_::ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(fontSize * 32, fontSize * 40), ImGuiCond_::ImGuiCond_FirstUseEver);

    bool is_open = true;
    ImGui::Begin("TQ Healthbars", &is_open);

    ImGui::Text("Default Menu Key: Insert");
    ImGui::Checkbox("Transparency", &m_ui_option_transparent);

    if (ImGui::CollapsingHeader("Health Bar Config", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool configChanged = false;
        configChanged |= ImGui::Checkbox("Show on Full Health Enemies", &g_healthBarConfig.showFullHealthEnemies);
        configChanged |= ImGui::Checkbox("Show Health Bar for Players", &g_healthBarConfig.showHealthBarForPlayers);
        configChanged |= ImGui::Checkbox("Show Health Bar for Monsters", &g_healthBarConfig.showHealthBarForMonsters);
        configChanged |= ImGui::Checkbox("Show Health Amount Text for Players", &g_healthBarConfig.showHealthAmountTextForPlayers);
        configChanged |= ImGui::Checkbox("Show Health Amount Text for Monsters", &g_healthBarConfig.showHealthAmountTextForMonsters);
        configChanged |= ImGui::Checkbox("Display Minion Health Bars", &g_healthBarConfig.showMinionHealthBars);
        configChanged |= ImGui::Checkbox("Show Debug Text", &g_healthBarConfig.showDebugText);
        ImGui::Separator();
        configChanged |= ImGui::SliderFloat2("Size", (float*)&g_healthBarConfig.size, 5.0f, 200.0f, "%.0f");
        configChanged |= ImGui::SliderFloat("Trailing Bar Delay (seconds)", &g_healthBarConfig.trailingBarDelaySeconds, 0.0f, 1.0f, "%.2f");
        configChanged |= ImGui::SliderFloat("Trailing Bar Ease Speed", &g_healthBarConfig.trailingBarEaseSpeed, 1.0f, 30.0f, "%.1f");
        configChanged |=
            ImGui::SliderFloat("Damage Flash Duration (seconds)", &g_healthBarConfig.damageFlashDurationSeconds, 0.0f, 1.0f, "%.2f");
        configChanged |= ImGui::SliderFloat("Damage Flash Width", &g_healthBarConfig.damageFlashWidth, 1.0f, 10.0f, "%.1f");
        configChanged |= ImGui::SliderFloat("Padding", &g_healthBarConfig.textPadding, 0, 30.0f, "%.1f");
        configChanged |= ImGui::SliderFloat("Base Font Size", &g_healthBarConfig.baseFontSize, 8.0f, 72.0f, "%.0f");
        configChanged |= ImGui::ColorEdit4("Friendly Color", (float*)&g_healthBarConfig.friendlyColor);
        configChanged |= ImGui::ColorEdit4("Enemy Color", (float*)&g_healthBarConfig.enemyColor);
        configChanged |= ImGui::ColorEdit4("Trailing Bar Color", (float*)&g_healthBarConfig.trailingBarColor);
        configChanged |= ImGui::SliderInt("Opacity", &g_healthBarConfig.opacity, 0, 255);

        if (configChanged)
        {
            g_healthBarConfig.save();
        }
    }

    if (ImGui::CollapsingHeader("Developer Tools"))
    {
        if (ImGui::BeginTable("Pointers", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.40f);
            ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthStretch, 0.45f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch, 0.15f);
            ImGui::TableHeadersRow();

            RenderPointerRow("Engine Base", reinterpret_cast<const void*>(ormenos::state::engineBase));
            RenderPointerRow("Game Base", reinterpret_cast<const void*>(ormenos::state::gameBase));
            RenderPointerRow("Engine Pointer", ormenos::state::g_pEngine);
            RenderPointerRow("Game Engine Pointer", ormenos::state::g_pGameEngine);
            RenderPointerRow("Camera Pointer", ormenos::state::g_pGameCamera);
            RenderPointerRow("Localization Manager Pointer", ormenos::state::g_pLocalizationManager);
            RenderPointerRow("Dialog Manager Pointer", ormenos::state::fnGetDialogManager
                                                           ? ormenos::state::fnGetDialogManager(ormenos::state::g_pGameEngine)
                                                           : nullptr);
            RenderPointerRow("TimeScale Pointer", ormenos::state::g_pTimeScale);
            RenderPointerRow("Last Created Widget", ormenos::state::lastCreatedWidget);
            RenderPointerRow("Render Rect Solid Function", reinterpret_cast<const void*>(ormenos::state::fnRenderRectSolid));

            auto* player = ormenos::state::g_pGameCamera ? ormenos::state::g_pGameCamera->player : nullptr;
            RenderPointerRow("Player Pointer", player);

            ImGui::EndTable();

            if (player)
            {
                ImGui::Checkbox("Player Invisible", &player->isInvisible);
                ImGui::Checkbox("Player Invincible", &player->invincible);
                ImGui::Checkbox("Player Observer Mode", &player->observerCheatMode);
            }
        }

        ImGui::Text("Viewport: %i x %i", ormenos::state::g_pViewport.w, ormenos::state::g_pViewport.h);
        ImGui::Separator();
        ImGui::Text("Characters: %zu", ormenos::state::allCharacters.size());
        if (ormenos::state::g_pTimeScale)
        {
            ImGui::InputFloat("Time Scale", ormenos::state::g_pTimeScale, 0.1f, 0.5f, "%.2f");
        }

        if (ImGui::Button("Increment Enemy Rarity"))
        {
            for (auto* character : ormenos::state::allCharacters)
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
    }

    ImGui::End();

    if (!is_open)
    {
        bool prev_state = m_draw_ui;
        m_draw_ui = is_open;
    }
    else if (m_draw_ui != m_last_draw_ui)
    {
        bool prev_state = m_draw_ui;
        m_draw_ui = true;
    }
}
