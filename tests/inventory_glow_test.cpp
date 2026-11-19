#include "../src/inventory_glow.cpp"
#include "../sdk/GAME/GraphicsTexture.hpp"
#include "../sdk/GAME/InventorySack.hpp"
#include <cassert>
#include <cstdio>
#include <vector>

namespace {
alignas(4) unsigned char playerStorage[sizeof(GAME::Player)]{};
auto& player = *reinterpret_cast<GAME::Player*>(playerStorage);
GAME::Player* currentPlayer = &player;
alignas(4) unsigned char controllerStorage[sizeof(GAME::ControllerPlayer)]{};
auto& controller = *reinterpret_cast<GAME::ControllerPlayer*>(controllerStorage);
auto& inventory = controller.inventoryCtrl;
alignas(4) unsigned char itemStorage[sizeof(GAME::Item)]{};
auto& item = *reinterpret_cast<GAME::Item*>(itemStorage);
void* itemVtable[10]{};
GAME::ObjectManager manager{};
GAME::InventorySack sack{};
GAME::InventorySack* sacks[65]{};
GAME::PotionStack potionStack{};
struct PickupCommand : GAME::CharPickUpConfigCmd { void Execute() override {} } command{};
GAME::InventoryIcon icon{};
GAME::GraphicsTexture texture{};
GAME::GraphicsCanvas canvas{};
GAME::Vec2 origin{100, 200};
GAME::Name testName{};
struct DrawnRect { GAME::Rect rect; GAME::Color color; };
std::vector<DrawnRect> drawn;
bool nativeRendered = false;
std::unordered_set<uint32_t> bag;
bool inWorld = true, acceptPickup = true, merge = false;
int executions = 0, pushes = 0, renders = 0, rectangles = 0;

GAME::Player* __fastcall fake_player(void*, void*) { return currentPlayer; }
GAME::ObjectManager* __cdecl fake_manager() { return &manager; }
GAME::ControllerCharacter* __fastcall fake_controller(GAME::ObjectManager*, void*, uint32_t id) { assert(id == 42); return &controller; }
GAME::Item* __fastcall fake_item(GAME::ObjectManager*, void*, uint32_t id) { assert(id == command.itemId); return &item; }
bool __fastcall fake_in_world(void*, void*) { return inWorld; }
bool __fastcall fake_contains(const GAME::InventorySack* target, void*, uint32_t id) { assert(target == &sack); return bag.contains(id); }
void __fastcall fake_push(void*, void*, uint32_t) { ++pushes; }
void __fastcall fake_execute(void*, void*)
{
    ++executions;
    if (!acceptPickup) return;
    if (merge) push_stack(&potionStack, nullptr, command.itemId);
    bag.insert(merge ? 999 : command.itemId);
}
uint32_t __fastcall fake_under_point(void*, void*, GAME::Vec2, int) { return command.itemId; }
void __fastcall fake_draw_icon(void* target, void*, GAME::GraphicsCanvas* targetCanvas,
                               const GAME::Vec2* targetOrigin, int pass, float opacity)
{
    assert(target == &icon && targetCanvas == &canvas && targetOrigin == &origin && pass == 1 && opacity == 1);
    nativeRendered = true;
    ++renders;
}
void __fastcall fake_rect(GAME::GraphicsCanvas* targetCanvas, void*, const GAME::Rect& targetRect,
                          const GAME::Color& glow, const void* shader, const GAME::Name& name)
{
    assert(nativeRendered && targetCanvas == &canvas && !shader && &name == &testName);
    assert(glow.a > 0 && glow.a <= 1.0f);
    drawn.push_back({targetRect, glow});
    ++rectangles;
}
void draw()
{
    nativeRendered = false;
    drawn.clear();
    draw_icon(&icon, nullptr, &canvas, &origin, 1, 1);
}
float alpha_at(float x, float y)
{
    float alpha = 0;
    for (const auto& quad : drawn)
    {
        const auto& r = quad.rect;
        if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h)
        {
            assert(alpha == 0); // Corners must not receive two overlapping strips.
            alpha = quad.color.a;
        }
    }
    return alpha;
}
}

int main()
{
    GAME::GameEngine engineInstance{};
    auto* engine = &engineInstance;
    player.controllerId = 42;
    inventory.parent = &player;
    itemVtable[9] = &fake_in_world;
    *reinterpret_cast<void***>(itemStorage) = itemVtable;
    potionStack.visiblePotionId = 999;
    icon.rect = {10, 20, 32, 32};
    icon.scale = 1;
    icon.texture = &texture;
    gameEngine = &engine;
    player.objectId = 123;
    command.parentId = player.objectId;
    command.itemId = 456;
    icon.itemId = command.itemId;
    icon.ownerId = player.objectId;
    icon.playerOwned = 1;
    inventoryHoverReturn = 0x1073f9;
    originalExecute = reinterpret_cast<Execute>(&fake_execute);
    originalPushStack = reinterpret_cast<PushStack>(&fake_push);
    originalItemUnderPoint = reinterpret_cast<ItemUnderPoint>(&fake_under_point);
    originalDrawIcon = reinterpret_cast<RenderIcon>(&fake_draw_icon);
    ormenos::state::fnRenderRectSolid = reinterpret_cast<ormenos::state::RenderRectSolid_t>(&fake_rect);
    noName = &testName;
    getMainPlayer = reinterpret_cast<MainPlayer>(&fake_player);
    getObjectManager = &fake_manager;
    findController = reinterpret_cast<FindController>(&fake_controller);
    findItem = reinterpret_cast<FindItem>(&fake_item);
    containsItem = reinterpret_cast<Contains>(&fake_contains);

    bag.insert(command.itemId);
    assert(!in_inventory(&inventory, command.itemId));
    sacks[1] = &sack;
    inventory.sacksBegin = sacks;
    inventory.sacksEnd = sacks;
    assert(!in_inventory(&inventory, command.itemId));
    inventory.sacksEnd = sacks + 65;
    assert(!in_inventory(&inventory, command.itemId));
    inventory.sacksEnd = sacks + 2;
    assert(in_inventory(&inventory, command.itemId));
    bag.clear();

    draw();
    assert(renders == 1 && rectangles == 0);
    acceptPickup = false;
    execute_pickup(&command, nullptr);
    assert(executions == 1 && unseenItems.empty());
    acceptPickup = true;
    inWorld = false;
    execute_pickup(&command, nullptr);
    assert(executions == 2 && unseenItems.empty());
    inWorld = true;
    ++command.parentId;
    execute_pickup(&command, nullptr);
    assert(executions == 3 && unseenItems.empty());
    --command.parentId;
    ormenos::g_healthBarConfig.newItemGlowEnabled = false;
    execute_pickup(&command, nullptr);
    assert(executions == 4 && unseenItems.empty());
    ormenos::g_healthBarConfig.newItemGlowEnabled = true;
    currentPlayer = nullptr;
    execute_pickup(&command, nullptr);
    assert(executions == 5 && unseenItems.empty());
    currentPlayer = &player;
    execute_pickup(&command, nullptr);
    assert(executions == 6 && unseenItems.contains(command.itemId) && !activePickup);
    draw();
    assert(renders == 2 && rectangles == 16);

    ormenos::g_healthBarConfig.newItemGlowEnabled = false;
    draw();
    ormenos::g_healthBarConfig.newItemGlowEnabled = true;
    icon.playerOwned = 0;
    draw();
    icon.playerOwned = 1;
    ++icon.ownerId;
    draw();
    --icon.ownerId;
    assert(renders == 5 && rectangles == 16);

    // Other inventory queries and another player's inventory do not dismiss the glow.
    assert(item_under_point(&inventory, nullptr, {}, 0) == command.itemId);
    acknowledge_hover(inventoryHoverReturn + 1, &inventory, command.itemId);
    inventory.parent = nullptr;
    acknowledge_hover(inventoryHoverReturn, &inventory, command.itemId);
    assert(unseenItems.contains(command.itemId));
    inventory.parent = &player;
    acknowledge_hover(inventoryHoverReturn, &inventory, command.itemId);
    draw();
    assert(unseenItems.empty() && renders == 6 && rectangles == 16);
    execute_pickup(&command, nullptr);
    assert(unseenItems.contains(command.itemId));
    ormenos::forget_inventory_glow(command.itemId);
    assert(unseenItems.empty());

    bag.clear();
    merge = true;
    execute_pickup(&command, nullptr);
    assert(unseenItems.size() == 1 && unseenItems.contains(999) && pushes == 1 && !activePickup);
    icon.itemId = 999;
    draw();
    assert(rectangles == 32);
    acknowledge_hover(inventoryHoverReturn, &inventory, 999);
    push_stack(&potionStack, nullptr, command.itemId);
    assert(unseenItems.empty() && pushes == 2);
    execute_pickup(&command, nullptr);
    assert(unseenItems.contains(999));
    ++player.objectId;
    icon.ownerId = player.objectId;
    draw();
    assert(unseenItems.empty() && rectangles == 32);

    unseenItems.insert(icon.itemId);
    struct DisplayCase { float scale; int height; int sizePx; };
    const DisplayCase displays[] = {
        {1.0f, 1080, 3}, {1.5f, 1080, 3}, {2.0f, 1080, 3},
        {1.0f, 1440, 3}, {1.0f, 2160, 3}, {2.0f, 2160, 3}, {1.0f, 2160, 1}
    };
    for (const auto size : {GAME::Vec2{32, 32}, GAME::Vec2{32, 96}, GAME::Vec2{96, 32}})
    {
        for (const auto& display : displays)
        {
            const auto [scale, height, sizePx] = display;
            ormenos::g_healthBarConfig.newItemGlowSizePx = sizePx;
            const float step = static_cast<float>(sizePx);
            ormenos::state::g_pViewport.h = height;
            icon.rect.w = size.x;
            icon.rect.h = size.y;
            icon.scale = scale;
            draw();
            assert(drawn.size() == 16);
            assert(drawn.front().rect.h == step);
            const float x = icon.rect.x + origin.x, y = icon.rect.y + origin.y;
            const float w = size.x * scale, h = size.y * scale;
            const float edge = alpha_at(x + step / 2, y + step / 2);
            assert(edge >= 0.549f && edge <= 0.751f);
            assert(alpha_at(x + w - step / 2, y + step / 2) == edge);
            assert(alpha_at(x + step / 2, y + h - step / 2) == edge);
            assert(alpha_at(x + w - step / 2, y + h - step / 2) == edge);
            assert(alpha_at(x + w / 2, y + step / 2) == edge);
            assert(alpha_at(x + step / 2, y + h / 2) == edge);
            assert(alpha_at(x + w / 2, y + h / 2) == 0);
            const float inner = alpha_at(x + 3.5f * step, y + h / 2);
            assert(inner > 0 && inner < edge * 0.1f);
            for (const auto& quad : drawn)
            {
                assert(quad.rect.w > 0 && quad.rect.h > 0);
                assert(quad.rect.x >= x && quad.rect.y >= y);
                assert(quad.rect.x + quad.rect.w <= x + w && quad.rect.y + quad.rect.h <= y + h);
            }
        }
    }
    ormenos::g_healthBarConfig.newItemGlowColor = ImColor(0.2f, 0.4f, 0.9f, 0.5f);
    draw();
    for (const auto& quad : drawn)
        assert(quad.color.r == 0.2f && quad.color.g == 0.4f && quad.color.b == 0.9f);
    assert(drawn.front().color.a >= 0.366f && drawn.front().color.a <= 0.5f);
    ormenos::g_healthBarConfig.newItemGlowColor.Value.w = 0;
    draw();
    assert(drawn.empty() && unseenItems.contains(icon.itemId));
    ormenos::g_healthBarConfig.newItemGlowColor.Value.w = 1;
    ormenos::g_healthBarConfig.newItemGlowSizePx = 10;
    icon.rect = {10, 20, 8, 10};
    icon.scale = 1;
    draw();
    assert(!drawn.empty());
    for (const auto& quad : drawn)
    {
        assert(quad.rect.w > 0 && quad.rect.h > 0);
        assert(quad.rect.x >= 110 && quad.rect.y >= 220);
        assert(quad.rect.x + quad.rect.w <= 118 && quad.rect.y + quad.rect.h <= 230);
    }
    assert(alpha_at(114, 225) > 0.73f);
    icon.texture = nullptr;
    draw();
    assert(drawn.empty());
    icon.texture = &texture;
    icon.rect.w = 0;
    draw();
    assert(drawn.empty());
    std::puts("Inventory glow pickup, stacks, hover, ownership, RGBA, pixel size, cleanup and four-edge geometry checks passed.");
}
