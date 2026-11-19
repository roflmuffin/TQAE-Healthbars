#include "health_bar_config.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <system_error>

#include "../vendor/toml.hpp"

namespace {
constexpr auto kConfigFileName = "ormenos.toml";
constexpr auto kHealthBarTableName = "health_bar";

float ReadFloat(const toml::table& table, std::string_view key, float fallback)
{
    if (const auto value = table[key].value<double>())
    {
        return static_cast<float>(*value);
    }
    return fallback;
}

void ReadSize(const toml::node* node, ImVec2& size)
{
    const auto* values = node ? node->as_array() : nullptr;
    if (!values || values->size() != 2)
    {
        return;
    }

    const auto width = values->get(0)->value<double>();
    const auto height = values->get(1)->value<double>();
    if (width && height)
    {
        size.x = std::clamp(static_cast<float>(*width), 5.0f, 200.0f);
        size.y = std::clamp(static_cast<float>(*height), 5.0f, 200.0f);
    }
}

void ReadColor(const toml::node* node, ImColor& color)
{
    const auto* values = node ? node->as_array() : nullptr;
    if (!values || values->size() != 4)
    {
        return;
    }

    int components[4]{};
    for (size_t index = 0; index < 4; ++index)
    {
        const auto component = values->get(index)->value<int64_t>();
        if (!component)
        {
            return;
        }
        components[index] = static_cast<int>(std::clamp(*component, int64_t{0}, int64_t{255}));
    }

    color = ImColor(components[0], components[1], components[2], components[3]);
}

int ColorComponent(float component) { return std::clamp(static_cast<int>(std::lround(component * 255.0f)), 0, 255); }

void LoadHealthBarConfig(const toml::table& table, ormenos::HealthBarConfig& config)
{
    config.showFullHealthEnemies = table["show_full_health_enemies"].value_or(config.showFullHealthEnemies);
    config.showHealthBarForPlayers = table["show_health_bar_for_players"].value_or(config.showHealthBarForPlayers);
    config.showHealthBarForMonsters = table["show_health_bar_for_monsters"].value_or(config.showHealthBarForMonsters);
    config.showHealthAmountTextForPlayers =
        table["show_health_amount_text_for_players"].value_or(config.showHealthAmountTextForPlayers);
    config.showHealthAmountTextForMonsters =
        table["show_health_amount_text_for_monsters"].value_or(config.showHealthAmountTextForMonsters);
    config.showMinionHealthBars = table["show_minion_health_bars"].value_or(config.showMinionHealthBars);
    config.showDebugText = table["show_debug_text"].value_or(config.showDebugText);
    config.animatedDamageNumbersEnabled = table["animated_damage_numbers_enabled"].value_or(config.animatedDamageNumbersEnabled);
    config.goldAutolootEnabled = table["gold_autoloot_enabled"].value_or(config.goldAutolootEnabled);
    config.potionAutolootEnabled = table["potion_autoloot_enabled"].value_or(config.potionAutolootEnabled);
    config.relicAutolootEnabled = table["relic_autoloot_enabled"].value_or(config.relicAutolootEnabled);
    config.charmAutolootEnabled = table["charm_autoloot_enabled"].value_or(config.charmAutolootEnabled);
    config.newItemGlowEnabled = table["new_item_glow_enabled"].value_or(config.newItemGlowEnabled);
    ReadColor(table.get("new_item_glow_color"), config.newItemGlowColor);
    config.newItemGlowSizePx = static_cast<int>(std::clamp(
        table["new_item_glow_size_px"].value_or<int64_t>(config.newItemGlowSizePx), int64_t{1}, int64_t{10}));
    config.autolootDelayMs = static_cast<int>(std::clamp(
        table["autoloot_delay_ms"].value_or<int64_t>(config.autolootDelayMs), int64_t{0}, int64_t{3000}));
    config.autolootRange = static_cast<int>(std::clamp(
        table["autoloot_range"].value_or<int64_t>(config.autolootRange), int64_t{1}, int64_t{30}));
    config.hitFlashesEnabled = table["hit_flashes_enabled"].value_or(config.hitFlashesEnabled);
    config.redHoverRimEnabled = table["red_hover_rim_enabled"].value_or(config.redHoverRimEnabled);
    config.hoverRimWidth = static_cast<int>(std::clamp(
        table["hover_rim_width"].value_or<int64_t>(config.hoverRimWidth), int64_t{10}, int64_t{90}));
    config.hoverRimOpacity = static_cast<int>(std::clamp(
        table["hover_rim_opacity"].value_or<int64_t>(config.hoverRimOpacity), int64_t{0}, int64_t{100}));
    config.hitFlashDurationMs = static_cast<int>(std::clamp(
        table["hit_flash_duration_ms"].value_or<int64_t>(config.hitFlashDurationMs), int64_t{10}, int64_t{1000}));

    ReadSize(table.get("size"), config.size);
    config.trailingBarDelaySeconds =
        std::clamp(ReadFloat(table, "trailing_bar_delay_seconds", config.trailingBarDelaySeconds), 0.0f, 1.0f);
    config.trailingBarEaseSpeed =
        std::clamp(ReadFloat(table, "trailing_bar_ease_speed", config.trailingBarEaseSpeed), 1.0f, 30.0f);
    config.damageFlashDurationSeconds =
        std::clamp(ReadFloat(table, "damage_flash_duration_seconds", config.damageFlashDurationSeconds), 0.0f, 1.0f);
    config.damageFlashWidth = std::clamp(ReadFloat(table, "damage_flash_width", config.damageFlashWidth), 1.0f, 10.0f);
    config.textPadding = std::clamp(ReadFloat(table, "text_padding", config.textPadding), 0.0f, 30.0f);
    config.baseFontSize = std::clamp(ReadFloat(table, "base_font_size", config.baseFontSize), 8.0f, 72.0f);

    ReadColor(table.get("friendly_color"), config.friendlyColor);
    ReadColor(table.get("enemy_color"), config.enemyColor);
    ReadColor(table.get("trailing_bar_color"), config.trailingBarColor);

    if (const auto opacityValue = table["opacity"].value<int64_t>())
    {
        config.opacity = std::clamp(static_cast<int32_t>(*opacityValue), int32_t{ 0 }, int32_t{ 255 });
    }
}

void UpdateConfigDocument(toml::table& document, const ormenos::HealthBarConfig& config)
{
    auto* table = document[kHealthBarTableName].as_table();
    if (!table)
    {
        document.insert_or_assign(kHealthBarTableName, toml::table{});
        table = document[kHealthBarTableName].as_table();
    }

    table->insert_or_assign("show_full_health_enemies", config.showFullHealthEnemies);
    table->insert_or_assign("show_health_bar_for_players", config.showHealthBarForPlayers);
    table->insert_or_assign("show_health_bar_for_monsters", config.showHealthBarForMonsters);
    table->insert_or_assign("show_health_amount_text_for_players", config.showHealthAmountTextForPlayers);
    table->insert_or_assign("show_health_amount_text_for_monsters", config.showHealthAmountTextForMonsters);
    table->insert_or_assign("show_minion_health_bars", config.showMinionHealthBars);
    table->insert_or_assign("show_debug_text", config.showDebugText);
    table->insert_or_assign("animated_damage_numbers_enabled", config.animatedDamageNumbersEnabled);
    table->insert_or_assign("gold_autoloot_enabled", config.goldAutolootEnabled);
    table->insert_or_assign("potion_autoloot_enabled", config.potionAutolootEnabled);
    table->insert_or_assign("relic_autoloot_enabled", config.relicAutolootEnabled);
    table->insert_or_assign("charm_autoloot_enabled", config.charmAutolootEnabled);
    table->insert_or_assign("new_item_glow_enabled", config.newItemGlowEnabled);
    table->insert_or_assign("new_item_glow_color", toml::array{ ColorComponent(config.newItemGlowColor.Value.x),
                                                              ColorComponent(config.newItemGlowColor.Value.y),
                                                              ColorComponent(config.newItemGlowColor.Value.z),
                                                              ColorComponent(config.newItemGlowColor.Value.w) });
    table->insert_or_assign("new_item_glow_size_px", config.newItemGlowSizePx);
    table->insert_or_assign("autoloot_delay_ms", config.autolootDelayMs);
    table->insert_or_assign("autoloot_range", config.autolootRange);
    table->insert_or_assign("hit_flashes_enabled", config.hitFlashesEnabled);
    table->insert_or_assign("red_hover_rim_enabled", config.redHoverRimEnabled);
    table->insert_or_assign("hover_rim_width", config.hoverRimWidth);
    table->insert_or_assign("hover_rim_opacity", config.hoverRimOpacity);
    table->insert_or_assign("hit_flash_duration_ms", config.hitFlashDurationMs);
    table->insert_or_assign("size", toml::array{ config.size.x, config.size.y });
    table->insert_or_assign("trailing_bar_delay_seconds", config.trailingBarDelaySeconds);
    table->insert_or_assign("trailing_bar_ease_speed", config.trailingBarEaseSpeed);
    table->insert_or_assign("damage_flash_duration_seconds", config.damageFlashDurationSeconds);
    table->insert_or_assign("damage_flash_width", config.damageFlashWidth);
    table->insert_or_assign("text_padding", config.textPadding);
    table->insert_or_assign("base_font_size", config.baseFontSize);
    table->insert_or_assign("friendly_color", toml::array{ ColorComponent(config.friendlyColor.Value.x),
                                                           ColorComponent(config.friendlyColor.Value.y),
                                                           ColorComponent(config.friendlyColor.Value.z),
                                                           ColorComponent(config.friendlyColor.Value.w) });
    table->insert_or_assign("enemy_color", toml::array{ ColorComponent(config.enemyColor.Value.x),
                                                        ColorComponent(config.enemyColor.Value.y),
                                                        ColorComponent(config.enemyColor.Value.z),
                                                        ColorComponent(config.enemyColor.Value.w) });
    table->insert_or_assign("trailing_bar_color", toml::array{ ColorComponent(config.trailingBarColor.Value.x),
                                                               ColorComponent(config.trailingBarColor.Value.y),
                                                               ColorComponent(config.trailingBarColor.Value.z),
                                                               ColorComponent(config.trailingBarColor.Value.w) });
    table->insert_or_assign("opacity", config.opacity);
}
} // namespace

struct ormenos::HealthBarConfig::Persistence
{
    std::filesystem::path filePath;
    toml::table document;
};

ormenos::HealthBarConfig ormenos::g_healthBarConfig;

ormenos::HealthBarConfig::HealthBarConfig() : persistence_(std::make_unique<Persistence>()) {}

ormenos::HealthBarConfig::~HealthBarConfig() = default;

void ormenos::HealthBarConfig::initialize()
{
    std::error_code error;
    persistence_->filePath = std::filesystem::current_path(error);
    if (error)
    {
        persistence_->filePath = kConfigFileName;
    }
    else
    {
        persistence_->filePath /= kConfigFileName;
    }

    const bool configExists = std::filesystem::exists(persistence_->filePath, error);
    if (error)
    {
        return;
    }

    if (configExists)
    {
        try
        {
            persistence_->document = toml::parse_file(persistence_->filePath.string());
        }
        catch (const toml::parse_error&)
        {
            return;
        }
    }

    if (const auto* table = persistence_->document[kHealthBarTableName].as_table())
    {
        LoadHealthBarConfig(*table, *this);
    }

    // Populate missing values and create the file on first run.
    save();
}

void ormenos::HealthBarConfig::save()
{
    if (persistence_->filePath.empty())
    {
        return;
    }

    UpdateConfigDocument(persistence_->document, *this);
    std::ofstream configFile(persistence_->filePath, std::ios::trunc);
    if (configFile)
    {
        configFile << persistence_->document;
    }
}
