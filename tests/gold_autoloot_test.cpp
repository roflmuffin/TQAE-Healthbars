#include "../src/gold_autoloot.cpp"
#include "../src/loot_notifications.cpp"
#include "../sdk/GAME/RTTI_ClassInfo.hpp"
#include <cassert>
#include <cstdlib>
#include <limits>
#include <cstdio>
#include <filesystem>
#include <fstream>

namespace {
alignas(4) unsigned char testPlayerStorage[sizeof(GAME::Player)]{};
auto& testPlayer = *reinterpret_cast<GAME::Player*>(testPlayerStorage);
alignas(4) unsigned char itemStorage[sizeof(GAME::Item)]{};
auto& testItem = *reinterpret_cast<GAME::Item*>(itemStorage);
alignas(4) unsigned char controllerStorage[sizeof(GAME::ControllerPlayer)]{};
auto& controller = *reinterpret_cast<GAME::ControllerPlayer*>(controllerStorage);
GAME::ObjectManager manager{};
GAME::GameEngine engineInstance{};
GAME::StyleManager styleManager{};
int updates = 0, pickups = 0;
bool alive = true, inWorld = true, controllerAvailable = true, space = true;
bool destroyOnPickup = false, destroyed = false, asyncDrop = false, failCreate = false;
bool acceptItem = true, destroyOnAdd = false;
int added = 0, captured = 0, nativeFrees = 0;
std::wstring localizedLabel = L"32 Gold";
std::string itemStyle = "ItemGold";
GAME::RTTI_ClassInfo itemClasses[7]{};
const GAME::RTTI_ClassInfo* currentItemClass = nullptr;
void* itemVtable[114]{};
void* playerVtable[187]{};
void* controllerVtable[19]{};
GAME::FontStyle testFontStyle{}, testDamageFont{};
void* databaseVtable[7]{};
void** testDatabase = databaseVtable;
const char* damageRecord = "records/ui/fontstyles/damagenormal.dbr";
std::string drawnStyle;
struct DrawnPopup { std::wstring text; std::string style; float alpha; int x, y, height, shadowOffset; GAME::Color color; };
std::vector<DrawnPopup> drawn;

void __cdecl fake_native_delete(void* memory) { ++nativeFrees; std::free(memory); }
GAME::GameWideString* __fastcall fake_description(void*, void*, GAME::GameWideString* result, bool noColour, bool)
{
    assert(!destroyed && noColour);
    ++captured;
    result->size = static_cast<uint32_t>(localizedLabel.size());
    result->capacity = std::max(result->size, 7u);
    auto* text = result->inlineBuffer;
    if (result->capacity >= 8)
    {
        text = static_cast<wchar_t*>(std::malloc((result->size + 1) * sizeof(wchar_t)));
        std::memcpy(result->inlineBuffer, &text, sizeof(text));
    }
    std::memcpy(text, localizedLabel.c_str(), (result->size + 1) * sizeof(wchar_t));
    return result;
}
GAME::ItemClassification __fastcall fake_classification(void*, void*) { return GAME::ItemClassification::OneShot; }
GAME::GameTextClass __fastcall fake_text_class(const GAME::GameEngine*, void*, GAME::ItemClassification classification)
{
    assert(classification == GAME::ItemClassification::OneShot);
    return static_cast<GAME::GameTextClass>(107);
}
GAME::GameString* __fastcall fake_style(const GAME::GameEngine*, void*, GAME::GameString* result, GAME::GameTextClass textClass)
{
    assert(textClass == static_cast<GAME::GameTextClass>(107));
    result->size = static_cast<uint32_t>(itemStyle.size());
    result->capacity = std::max(result->size, 15u);
    auto* text = result->inlineBuffer;
    if (result->capacity >= 16)
    {
        text = static_cast<char*>(std::malloc(result->size + 1));
        std::memcpy(result->inlineBuffer, &text, sizeof(text));
    }
    std::memcpy(text, itemStyle.c_str(), result->size + 1);
    return result;
}
GAME::StyleManager* __cdecl fake_style_manager() { return &styleManager; }
void* __fastcall fake_database(void*, void*) { return &testDatabase; }
const char* __fastcall fake_database_string(void*, void*, const char* key, const char* fallback)
{
    assert(std::strcmp(key, "DamageNormalStyle") == 0 && !*fallback);
    return damageRecord;
}
const GAME::FontStyle* __fastcall fake_font_style(GAME::StyleManager* manager, void*, const GAME::GameString* style)
{
    assert(manager == &styleManager);
    drawnStyle.assign(GAME::string_data(*style), style->size);
    if (drawnStyle == damageRecord) return &testDamageFont;
    testFontStyle.color = drawnStyle == "ItemGold" ? GAME::Color{1, 0.8f, 0, 1} : GAME::Color{0, 0.5f, 1, 1};
    return &testFontStyle;
}
float __fastcall fake_render(void*, void*, int x, int y, const GAME::Color* color, const wchar_t* text,
                            const void* font, int height, int xAlign, int yAlign, bool shadow,
                            int shadowOffset, int renderStyle, bool outline)
{
    assert(xAlign == 2 && yAlign == 1 && font == testDamageFont.font && renderStyle == testDamageFont.renderStyle);
    assert(shadow == testDamageFont.shadow && outline == testDamageFont.outline);
    drawn.push_back({text, drawnStyle, color->a, x, y, height, shadowOffset, *color});
    return 0;
}
void __fastcall fake_project(GAME::WorldCamera*, void*, GAME::Vec2* result, GAME::WorldVec3* head, GAME::Viewport*)
{
    assert(head->position.y > testPlayer.coords.location.position.y + testPlayer.height);
    if (testPlayer.graphicsMeshInstance)
        assert(head->position.y > testPlayer.coords.location.position.y + testPlayer.height + testPlayer.graphicsMeshInstance->somethingY);
    *result = {400, 300};
}
bool __fastcall fake_add_item(void*, void*, uint32_t id, bool allSacks, int sack)
{
    assert(id == testItem.objectId && allSacks && sack == 0);
    ++added;
    if (acceptItem && destroyOnAdd) destroy_item(&testItem, nullptr);
    return acceptItem;
}

const GAME::RTTI_ClassInfo* __fastcall fake_item_class(GAME::Item*, void*) { return currentItemClass; }
void __fastcall fake_update(GAME::Item*, void*, int) { assert(!destroyed); ++updates; }
void __fastcall fake_one_shot_update(GAME::Item* item, void*, int dt)
{
    update_item(item, nullptr, dt);
    assert(!destroyed);
}
void __fastcall fake_destroy(GAME::Item*, void*) { destroyed = true; }
bool __fastcall fake_in_world(void*, void*) { return inWorld; }
bool __fastcall fake_alive(void*, void*) { return alive; }
bool __fastcall fake_allowed(void*, void*) { return true; }
bool __fastcall fake_space(void*, void*, const void*) { return space; }
GAME::Player* __fastcall fake_player(void*, void*) { return &testPlayer; }
GAME::ObjectManager* __cdecl fake_manager() { return &manager; }
GAME::ControllerCharacter* __fastcall fake_controller(GAME::ObjectManager*, void*, uint32_t id)
{
    assert(id == 42);
    return controllerAvailable ? &controller : nullptr;
}
GAME::Item* __fastcall fake_find_item(void*, void*, uint32_t id)
{
    assert(id == testItem.objectId);
    return &testItem;
}
void __fastcall fake_pickup(GAME::ControllerCharacter* target, void*, uint32_t id)
{
    assert(target == &controller && id == testItem.objectId);
    ++pickups;
    if (destroyOnPickup) destroy_item(&testItem, nullptr);
}
GAME::Item* __cdecl fake_create(const GAME::ItemReplicaInfo*) { return failCreate ? nullptr : &testItem; }
void __fastcall fake_drop(void*, void*, const void*, uint32_t, bool)
{
    if (!asyncDrop) create_item(nullptr);
}
void tick(int dt = 16)
{
    const auto kind = loot_kind(&testItem);
    if (kind == LootKind::Gold || kind == LootKind::Potion) update_one_shot(&testItem, nullptr, dt);
    else update_item(&testItem, nullptr, dt);
}
void reset_item(const GAME::RTTI_ClassInfo* type)
{
    currentItemClass = type;
    testItem.objectId++;
    testItem.replica.locallyDropped = false;
    lootStates.clear();
    pendingDrops.clear();
    destroyed = false;
    pickups = updates = 0;
    destroyOnPickup = false;
}
}

int main()
{
    using namespace ormenos;
    testDamageFont.font = reinterpret_cast<GAME::GraphicsFont*>(&testItem);
    testDamageFont.height = 16;
    testDamageFont.renderStyle = 2;
    testDamageFont.shadow = true;
    testDamageFont.shadowOffset = 3;
    testDamageFont.outline = true;
    itemVtable[0] = &fake_item_class;
    itemVtable[9] = &fake_in_world;
    itemVtable[63] = &fake_description;
    itemVtable[113] = &fake_classification;
    *reinterpret_cast<void***>(itemStorage) = itemVtable;
    playerVtable[9] = &fake_in_world;
    playerVtable[86] = &fake_alive;
    playerVtable[186] = &fake_space;
    controllerVtable[18] = &fake_pickup;
    *reinterpret_cast<void***>(testPlayerStorage) = playerVtable;
    *reinterpret_cast<void***>(controllerStorage) = controllerVtable;
    testPlayer.controllerId = 42;
    testItem.pickupOk = true;
    goldClass = &itemClasses[0];
    potionClasses[0] = &itemClasses[1];
    potionClasses[1] = &itemClasses[2];
    potionClasses[2] = &itemClasses[3];
    relicClass = &itemClasses[4];
    charmClass = &itemClasses[5];
    auto* region = reinterpret_cast<GAME::Region*>(&testPlayer);
    testPlayer.coords.location = {region, {0, 0, 0}};
    testItem.coords.location = {region, {3, 0, 0}};
    char name[] = "records/test/potion.dbr";
    testItem.objectName = GAME::borrowed_string(name, sizeof(name) - 1);
    assert(gold_in_pickup_range(testPlayer.coords.location, testItem.coords.location, 3));
    testItem.coords.location.position.x = 3.01f;
    assert(!gold_in_pickup_range(testPlayer.coords.location, testItem.coords.location, 3));
    testItem.coords.location.position = {0, 3.01f, 0};
    assert(!gold_in_pickup_range(testPlayer.coords.location, testItem.coords.location, 3));
    testItem.coords.location.position = {std::numeric_limits<float>::quiet_NaN(), 0, 0};
    assert(!gold_in_pickup_range(testPlayer.coords.location, testItem.coords.location, 20));
    testItem.coords.location = {nullptr, {0, 0, 0}};
    assert(!gold_in_pickup_range(testPlayer.coords.location, testItem.coords.location, 20));
    testItem.coords.location = {reinterpret_cast<GAME::Region*>(&testItem), {0, 0, 0}};
    assert(!gold_in_pickup_range(testPlayer.coords.location, testItem.coords.location, 20));
    testItem.coords.location = {region, {1, 0, 0}};

    auto* engine = &engineInstance;
    gameEngine = &engine;
    originalUpdate = reinterpret_cast<Update>(&fake_update);
    originalOneShotUpdate = reinterpret_cast<Update>(&fake_one_shot_update);
    originalDestroy = reinterpret_cast<Destroy>(&fake_destroy);
    originalDropCopy = reinterpret_cast<DropCopy>(&fake_drop);
    originalCreateItem = &fake_create;
    isAllowedInContent = reinterpret_cast<AllowedInContent>(&fake_allowed);
    getMainPlayer = reinterpret_cast<MainPlayer>(&fake_player);
    getObjectManager = &fake_manager;
    findController = reinterpret_cast<FindController>(&fake_controller);
    findItem = reinterpret_cast<FindItem>(&fake_find_item);
    originalAddItem = reinterpret_cast<AddItem>(&fake_add_item);
    nativeDelete = &fake_native_delete;
    getTextClass = reinterpret_cast<TextClass>(&fake_text_class);
    getTextStyle = reinterpret_cast<TextStyle>(&fake_style);
    renderText = reinterpret_cast<RenderText>(&fake_render);
    getStyleManager = &fake_style_manager;
    getFontStyle = reinterpret_cast<GetFontStyle>(&fake_font_style);
    getDatabase = reinterpret_cast<Database>(&fake_database);
    databaseVtable[0x18 / 4] = &fake_database_string;

    const GAME::RTTI_ClassInfo* classes[] = {goldClass, potionClasses[0], potionClasses[1], potionClasses[2], relicClass, charmClass};
    bool* toggles[] = {&g_healthBarConfig.goldAutolootEnabled, &g_healthBarConfig.potionAutolootEnabled,
        &g_healthBarConfig.potionAutolootEnabled, &g_healthBarConfig.potionAutolootEnabled,
        &g_healthBarConfig.relicAutolootEnabled, &g_healthBarConfig.charmAutolootEnabled};
    g_healthBarConfig.autolootDelayMs = 0;
    for (int i = 0; i < 6; ++i)
    {
        reset_item(classes[i]);
        for (auto* toggle : toggles) *toggle = true;
        *toggles[i] = false;
        tick();
        assert(pickups == 0 && updates == 1);
        for (auto* toggle : toggles) *toggle = false;
        *toggles[i] = true;
        tick();
        assert(pickups == 1 && updates == 1);
        tick();
        assert(pickups == 1 && updates == 2);
        lootStates[testItem.objectId].lastAttempt = GetTickCount64() - 1001;
        destroyOnPickup = true;
        tick();
        assert(pickups == 2 && updates == 2 && destroyed && lootStates.empty());
    }
    for (auto* toggle : toggles) *toggle = true;
    for (auto* type : classes)
    {
        for (int range : {1, 10, 30})
        {
            reset_item(type);
            g_healthBarConfig.autolootRange = range;
            testItem.coords.location.position = {static_cast<float>(range) + 0.01f, 0, 0};
            tick();
            assert(pickups == 0);
            testItem.coords.location.position.x = static_cast<float>(range);
            tick();
            assert(pickups == 1);
        }
    }
    g_healthBarConfig.autolootRange = 10;
    testItem.coords.location.position = {1, 0, 0};
    reset_item(&itemClasses[6]);
    tick();
    assert(pickups == 0 && updates == 1 && lootStates.empty());

    reset_item(potionClasses[1]);
    g_healthBarConfig.autolootDelayMs = 750;
    tick();
    tick(749);
    tick(0);
    tick(-1);
    assert(pickups == 0);
    tick(1);
    assert(pickups == 1);
    reset_item(relicClass);
    tick();
    tick(std::numeric_limits<int>::max());
    assert(pickups == 1 && lootStates[testItem.objectId].ageMs == 3000);
    g_healthBarConfig.autolootDelayMs = 0;

    reset_item(charmClass);
    alive = false; tick(); alive = true;
    inWorld = false; tick(); inWorld = true;
    testItem.pickupOk = false; tick(); testItem.pickupOk = true;
    controllerAvailable = false; tick(); controllerAvailable = true;
    space = false; tick(); space = true;
    testItem.hasBeenPickedUp = true; tick(); testItem.hasBeenPickedUp = false;
    testPlayer.observerMode = true; tick(); testPlayer.observerMode = false;
    testItem.replica.locallyDropped = true; tick(); testItem.replica.locallyDropped = false;
    assert(pickups == 0 && updates == 8);
    tick();
    assert(pickups == 1);
    reset_item(goldClass);
    space = false; tick(); space = true;
    assert(pickups == 1);

    // Drops stay excluded while disabled, beyond the delay, and across toggle changes.
    for (auto* type : classes)
    {
        reset_item(type);
        for (auto* toggle : toggles) *toggle = false;
        drop_copy(engine, nullptr, nullptr, testItem.objectId, true);
        assert(pendingDrops.empty() && lootStates[testItem.objectId].excluded);
        for (auto* toggle : toggles) *toggle = true;
        tick(3000);
        tick(3000);
        assert(pickups == 0);
        destroy_item(&testItem, nullptr);
        destroyed = false;
        tick();
        assert(pickups == 1);
    }
    reset_item(potionClasses[0]);
    asyncDrop = true;
    drop_copy(engine, nullptr, nullptr, testItem.objectId, true);
    drop_copy(engine, nullptr, nullptr, testItem.objectId, true);
    assert(pendingDrops.begin()->second.count == 2);
    failCreate = true;
    assert(create_item(nullptr) == nullptr && pendingDrops.begin()->second.count == 2);
    failCreate = false;
    ++testItem.objectId;
    create_item(nullptr);
    tick(3000);
    assert(pickups == 0 && pendingDrops.begin()->second.count == 1);
    ++testItem.objectId;
    create_item(nullptr);
    tick(3000);
    assert(pickups == 0 && pendingDrops.empty());
    ++testItem.objectId;
    create_item(nullptr);
    tick();
    assert(pickups == 1);

    reset_item(potionClasses[0]);
    drop_copy(engine, nullptr, nullptr, testItem.objectId, true);
    pendingDrops.begin()->second.expires = 0;
    ++testItem.objectId;
    create_item(nullptr);
    tick();
    assert(pickups == 1 && pendingDrops.empty());

    auto* inventory = &controller.inventoryCtrl;
    GAME::PlayerInventoryCtrl otherInventory{};
    reset_item(goldClass);
    assert(add_item(inventory, nullptr, testItem.objectId, true, 0));
    assert(captured == 0 && popups.empty()); // Manual pickup.
    tick();
    assert(add_item(&otherInventory, nullptr, testItem.objectId, true, 0));
    assert(captured == 0 && popups.empty()); // Another player's inventory.
    destroyOnAdd = true;
    assert(add_item(inventory, nullptr, testItem.objectId, true, 0));
    assert(destroyed && lootStates.empty() && captured == 1 && popups.size() == 1);
    assert(popups.back().content.text == L"+32 Gold" && popups.back().content.style == "ItemGold");
    assert(nativeFrees == 0); // Both native strings fit their inline storage.
    assert(add_item(inventory, nullptr, testItem.objectId, true, 0));
    assert(captured == 1 && popups.size() == 1); // No duplicate notification.
    destroyOnAdd = false;

    reset_item(relicClass);
    localizedLabel = L"\u795e\u8056\u306a\u30ec\u30ea\u30c3\u30af\u306e\u7834\u7247";
    itemStyle = "CustomLocalizedRelicStyle";
    tick();
    acceptItem = false;
    assert(!add_item(inventory, nullptr, testItem.objectId, true, 0));
    assert(popups.size() == 1 && captured == 2 && nativeFrees == 2);
    lootStates[testItem.objectId].lastAttempt = GetTickCount64() - 1001;
    tick();
    acceptItem = true;
    assert(add_item(inventory, nullptr, testItem.objectId, true, 0));
    assert(popups.size() == 2 && nativeFrees == 4);
    assert(popups.back().content.text == L"+" + localizedLabel && popups.back().content.style == itemStyle);

    reset_item(charmClass);
    tick();
    lootStates[testItem.objectId].lastAttempt = GetTickCount64() - 10001;
    assert(add_item(inventory, nullptr, testItem.objectId, true, 0));
    assert(popups.size() == 2 && captured == 3); // An expired request does not label a later manual pickup.

    GAME::GameCamera camera{};
    GAME::GraphicsCanvas canvas{};
    alignas(4) unsigned char meshStorage[sizeof(GAME::GraphicsMeshInstance)]{};
    auto& mesh = *reinterpret_cast<GAME::GraphicsMeshInstance*>(meshStorage);
    mesh.somethingY = 2.0f;
    testPlayer.graphicsMeshInstance = &mesh;
    camera.player = &testPlayer;
    state::g_pGameEngine = engine;
    state::g_pGameCamera = &camera;
    state::g_pViewport = {0, 0, 1920, 1080};
    state::fnWorldCameraProject = reinterpret_cast<state::ProjectFunc>(&fake_project);
    for (auto& popup : popups) popup.created = GetTickCount64();
    render_loot_notifications(&canvas);
    assert(drawn.size() == 2 && drawn[0].alpha == 1 && drawn[1].alpha == 1);
    assert(drawn[0].text == L"+" + localizedLabel && drawn[0].style == itemStyle);
    assert(drawn[0].height == 16 && drawn[0].shadowOffset == 3 && drawn[0].y <= 282);
    assert(drawn[0].color.r == 0 && drawn[0].color.g == 0.5f && drawn[0].color.b == 1);
    assert(drawn[1].color.r == 1 && drawn[1].color.g == 0.8f && drawn[1].color.b == 0);
    assert(drawn[1].x == 400 && drawn[1].y <= drawn[0].y - 28);
    drawn.clear();
    popups.front().created = GetTickCount64() - 1900;
    popups.back().created = GetTickCount64() - 1550;
    render_loot_notifications(&canvas);
    assert(popups.size() == 1 && drawn.size() == 1 && drawn[0].alpha > 0.4f && drawn[0].alpha <= 0.5f);
    drawn.clear();
    state::g_pViewport = {0, 0, 3840, 2160};
    testPlayer.graphicsMeshInstance = nullptr;
    testDamageFont.height = 19;
    testDamageFont.shadow = false;
    testDamageFont.shadowOffset = 4;
    testDamageFont.outline = false;
    render_loot_notifications(&canvas);
    assert(drawn.size() == 1 && drawn[0].height == 19 && drawn[0].shadowOffset == 4 && drawn[0].y <= 264);
    state::g_pViewport = {0, 0, 1920, 1080};
    for (int i = 0; i < 20; ++i) show_loot_notification({L"+1 Gold", "ItemGold"}, testPlayer.objectId);
    assert(popups.size() == 8);
    ++testPlayer.objectId;
    render_loot_notifications(&canvas);
    assert(popups.empty());
    show_loot_notification({L"+1 Gold", "ItemGold"}, testPlayer.objectId);
    camera.player = nullptr;
    render_loot_notifications(&canvas);
    assert(popups.empty());

    const auto oldPath = std::filesystem::current_path();
    const auto testPath = std::filesystem::temp_directory_path() /
        ("ormenos-loot-test-" + std::to_string(GetCurrentProcessId()));
    std::filesystem::create_directory(testPath);
    std::filesystem::current_path(testPath);
    {
        HealthBarConfig saved;
        saved.initialize();
        assert(saved.autolootRange == 10);
        assert(saved.newItemGlowEnabled);
        assert(saved.newItemGlowSizePx == 3);
        assert(std::lround(saved.newItemGlowColor.Value.w * 255) == 191);
        saved.newItemGlowEnabled = false;
        saved.newItemGlowSizePx = 7;
        saved.newItemGlowColor = ImColor(40, 100, 220, 128);
        saved.goldAutolootEnabled = false;
        saved.potionAutolootEnabled = true;
        saved.relicAutolootEnabled = false;
        saved.charmAutolootEnabled = true;
        saved.autolootDelayMs = 1250;
        saved.autolootRange = 12;
        saved.save();
        HealthBarConfig loaded;
        loaded.initialize();
        assert(!loaded.newItemGlowEnabled);
        assert(loaded.newItemGlowSizePx == 7);
        assert(std::lround(loaded.newItemGlowColor.Value.x * 255) == 40 &&
               std::lround(loaded.newItemGlowColor.Value.y * 255) == 100 &&
               std::lround(loaded.newItemGlowColor.Value.z * 255) == 220 &&
               std::lround(loaded.newItemGlowColor.Value.w * 255) == 128);
        assert(!loaded.goldAutolootEnabled && loaded.potionAutolootEnabled &&
            !loaded.relicAutolootEnabled && loaded.charmAutolootEnabled && loaded.autolootDelayMs == 1250);
        assert(loaded.autolootRange == 12);
        std::ofstream("ormenos.toml") << "[health_bar]\nautoloot_delay_ms = -50\nautoloot_range = -50\n"
            "new_item_glow_size_px = -50\nnew_item_glow_color = [900, -10, 9223372036854775807, -9223372036854775808]\n";
        loaded.initialize();
        assert(loaded.autolootDelayMs == 0);
        assert(loaded.autolootRange == 1);
        assert(loaded.newItemGlowSizePx == 1);
        assert(loaded.newItemGlowColor.Value.x == 1 && loaded.newItemGlowColor.Value.y == 0 &&
               loaded.newItemGlowColor.Value.z == 1 && loaded.newItemGlowColor.Value.w == 0);
        std::ofstream("ormenos.toml") << "[health_bar]\nautoloot_delay_ms = 999999\nautoloot_range = 999999\n"
            "new_item_glow_size_px = 999999\nnew_item_glow_color = [1, 2, \"bad\", 4]\n";
        loaded.initialize();
        assert(loaded.autolootDelayMs == 3000);
        assert(loaded.autolootRange == 30);
        assert(loaded.newItemGlowSizePx == 10);
        assert(loaded.newItemGlowColor.Value.x == 1 && loaded.newItemGlowColor.Value.y == 0 &&
               loaded.newItemGlowColor.Value.z == 1 && loaded.newItemGlowColor.Value.w == 0);
    }
    std::filesystem::current_path(oldPath);
    std::filesystem::remove(testPath / "ormenos.toml");
    std::filesystem::remove(testPath);
    std::puts("Autoloot categories, range, delay, inventory guards, drops, localized pickup popups, lifetime and persistence passed.");
}
