#include "hit_flash.h"
#include "../sdk/GAME/ParametersCombat.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <unordered_map>
#define NOMINMAX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#include "d3d11_impl.h"
#include "health_bar_config.h"
#include "hit_flash_shader.h"
#include "mem.hpp"
#include "../sdk/GAME/Player.hpp"
#include "../sdk/GAME/GraphicsSceneRenderer.hpp"
#include "../sdk/GAME/GraphicsMeshInstance.hpp"
#include <MinHook.h>

namespace {
using namespace GAME;
bool enabled = true;
bool redHoverRim = true;
float hoverRimWidth = 0.6f;
float hoverRimOpacity = 1.0f;
uint64_t durationMs = 150;
using Microsoft::WRL::ComPtr;
ComPtr<ID3D11PixelShader> highlightShader;
ComPtr<ID3D11Buffer> highlightConstants;
ComPtr<ID3D11BlendState> highlightBlend;
ID3D11DeviceContext* renderContext = nullptr;
GAME::Name highlightStyle{};

constexpr bool ShouldFlash(bool hit, AttackStyle style, float before, float after)
{
    return hit && style <= AttackStyle::Projectile && before > 0.0f && after < before;
}

constexpr float FlashAmount(uint64_t age, uint64_t duration)
{
    // Hold full white for the first third so the first rendered frame has a strong impact.
    return age >= duration ? 0.0f : std::min(1.0f, 1.5f * (1.0f - static_cast<float>(age) / duration));
}

static_assert(ShouldFlash(true, AttackStyle::Melee, 100, 80));
static_assert(ShouldFlash(true, AttackStyle::Projectile, 100, -10));
static_assert(!ShouldFlash(false, AttackStyle::Melee, 100, 100));
static_assert(!ShouldFlash(true, AttackStyle::Direct, 100, 100));
static_assert(!ShouldFlash(true, AttackStyle::Retaliation, 100, 80));
static_assert(!ShouldFlash(true, AttackStyle::PeriodicDebuff, 100, 80));
static_assert(!ShouldFlash(true, AttackStyle::Melee, 0, -10));
static_assert(FlashAmount(0, 150) == 1.0f && FlashAmount(40, 150) == 1.0f);
static_assert(FlashAmount(75, 150) == 0.75f);
static_assert(FlashAmount(150, 150) == 0.0f && FlashAmount(UINT64_MAX, 150) == 0.0f);
static_assert(FlashAmount(0, 0) == 0.0f && FlashAmount(500, 1000) == 0.75f);

struct Flash
{
    uint32_t objectId;
    World* world;
    uint64_t started;
};

std::mutex flashesMutex;
std::unordered_map<Character*, Flash> flashes;
struct MeshHighlight
{
    Character* owner = nullptr;
    float flash = 0.0f;
    bool redRim = false;
    bool hovered = false;
    float rimWidth = 0.6f;
    float rimOpacity = 1.0f;
};
std::unordered_map<GraphicsMeshInstance*, MeshHighlight> meshHighlights;
thread_local MeshHighlight traversingHighlight;

struct HighlightPass
{
    MeshHighlight effect;
    ComPtr<ID3D11PixelShader> shader;
    ComPtr<ID3D11Buffer> constants;
    ComPtr<ID3D11BlendState> blend;
    float blendFactor[4]{};
    UINT sampleMask = 0;
};
thread_local HighlightPass* currentPass = nullptr;
using TakeAttack = bool(__thiscall*)(Character*, ParametersCombat*);
using ApplyHighlight = void(__thiscall*)(Character*, Entity*, const Color&, float);
using ActorHighlight = void(__thiscall*)(Actor*, Entity*, const Color&, float);
using AddRenderable = void(__thiscall*)(GraphicsSceneRenderer*, GraphicsRenderable*, Region*, bool);
using RenderPass = void(__thiscall*)(GraphicsMeshInstance*, const RenderablePass*, const GAME::Name&, GraphicsCanvas*, const GraphicsSceneRenderer*);
using DrawIndexed = void(STDMETHODCALLTYPE*)(ID3D11DeviceContext*, UINT, UINT, INT);
using DestroyCharacter = void(__thiscall*)(Character*);
using IsFoe = bool(__thiscall*)(const Team*, const Team&);
using GetMainPlayer = Player*(__thiscall*)(const GameEngine*);
TakeAttack originalTakeAttack = nullptr;
ApplyHighlight originalApplyHighlight = nullptr;
ActorHighlight originalActorHighlight = nullptr;
AddRenderable originalAddRenderable = nullptr;
RenderPass originalRenderPass = nullptr;
DrawIndexed originalDrawIndexed = nullptr;
DestroyCharacter originalDestroyCharacter = nullptr;
IsFoe isFoe = nullptr;
GetMainPlayer getMainPlayer = nullptr;

bool __fastcall OnTakeAttack(Character* character, void*, ParametersCombat* parameters)
{
    const auto style = parameters->style;
    const float before = character->bio.currentLife;
    const bool hit = originalTakeAttack(character, parameters);
    if (!ShouldFlash(hit, style, before, character->bio.currentLife)) return hit;

    auto* engine = ormenos::state::g_pEngine;
    auto* game = ormenos::state::g_pGameEngine;
    auto* player = game ? getMainPlayer(game) : nullptr;
    if (engine && engine->world && player && player != character && isFoe(&player->team, character->team))
    {
        std::lock_guard lock(flashesMutex);
        if (enabled) flashes[character] = {character->objectId, engine->world, GetTickCount64()};
    }
    return hit;
}

void __fastcall OnApplyHighlight(Character* character, void*, Entity* root, const Color& color, float intensity)
{
    MeshHighlight effect;
    effect.owner = character;
    effect.hovered = character->highlightEnabled;
    {
        std::lock_guard lock(flashesMutex);
        const auto it = flashes.find(character);
        if (it != flashes.end())
        {
            auto* engine = ormenos::state::g_pEngine;
            if (enabled && it->second.objectId == character->objectId && engine && it->second.world == engine->world)
                effect.flash = FlashAmount(GetTickCount64() - it->second.started, durationMs);
            if (effect.flash == 0.0f) flashes.erase(it);
        }
        effect.redRim = redHoverRim && effect.hovered;
        effect.rimWidth = hoverRimWidth;
        effect.rimOpacity = hoverRimOpacity;
    }
    if (effect.redRim)
    {
        auto* game = ormenos::state::g_pGameEngine;
        auto* player = game ? getMainPlayer(game) : nullptr;
        effect.redRim = player && player != character && isFoe(&player->team, character->team);
    }
    const auto previous = traversingHighlight;
    traversingHighlight = effect;
    originalApplyHighlight(character, root, color, intensity);
    traversingHighlight = previous;
}

void __fastcall OnActorHighlight(Actor* actor, void*, Entity* root, const Color& color, float intensity)
{
    if (traversingHighlight.owner && actor->graphicsMeshInstance &&
        (traversingHighlight.flash > 0.0f || traversingHighlight.redRim))
    {
        std::lock_guard lock(flashesMutex);
        meshHighlights[actor->graphicsMeshInstance] = traversingHighlight;
    }
    originalActorHighlight(actor, root, color, intensity);
}

void __fastcall OnAddRenderable(GraphicsSceneRenderer* scene, void*, GraphicsRenderable* renderable, Region* region, bool highlighted)
{
    {
        std::lock_guard lock(flashesMutex);
        const auto it = meshHighlights.find(reinterpret_cast<GraphicsMeshInstance*>(renderable));
        if (it != meshHighlights.end() && it->second.flash > 0.0f) highlighted = true;
    }
    // Queue the existing highlight pass without changing the game's selected character.
    originalAddRenderable(scene, renderable, region, highlighted);
}

void STDMETHODCALLTYPE OnDrawIndexed(ID3D11DeviceContext* context, UINT count, UINT start, INT base)
{
    auto* pass = currentPass;
    if (!pass || context != renderContext)
    {
        originalDrawIndexed(context, count, start, base);
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(context->Map(highlightConstants.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        originalDrawIndexed(context, count, start, base);
        return;
    }
    const float values[4] = {pass->effect.flash, pass->effect.redRim ? 1.0f : 0.0f,
                             pass->effect.rimWidth, pass->effect.rimOpacity};
    std::memcpy(mapped.pData, values, sizeof(values));
    renderContext->Unmap(highlightConstants.Get(), 0);

    renderContext->PSGetShader(pass->shader.ReleaseAndGetAddressOf(), nullptr, nullptr);
    renderContext->PSGetConstantBuffers(13, 1, pass->constants.ReleaseAndGetAddressOf());
    renderContext->OMGetBlendState(pass->blend.ReleaseAndGetAddressOf(), pass->blendFactor, &pass->sampleMask);
    renderContext->PSSetShader(highlightShader.Get(), nullptr, 0);
    ID3D11Buffer* buffer = highlightConstants.Get();
    renderContext->PSSetConstantBuffers(13, 1, &buffer);
    renderContext->OMSetBlendState(highlightBlend.Get(), nullptr, 0xffffffff);
    originalDrawIndexed(context, count, start, base);
    renderContext->PSSetShader(pass->shader.Get(), nullptr, 0);
    buffer = pass->constants.Get();
    renderContext->PSSetConstantBuffers(13, 1, &buffer);
    renderContext->OMSetBlendState(pass->blend.Get(), pass->blendFactor, pass->sampleMask);
}

void __fastcall OnRenderPass(GraphicsMeshInstance* mesh, void*, const RenderablePass* pass,
                             const GAME::Name& style, GraphicsCanvas* canvas, const GraphicsSceneRenderer* scene)
{
    HighlightPass highlight;
    {
        std::lock_guard lock(flashesMutex);
        const auto it = meshHighlights.find(mesh);
        if (it != meshHighlights.end()) highlight.effect = it->second;
    }
    // Preserve the native dissolve effect instead of drawing a solid dying/spawning mesh.
    if (mesh->isDissolvingOne || mesh->isDissolvingTwo) highlight.effect = {};
    const float originalIntensity = mesh->highlight;
    if (highlight.effect.hovered)
        mesh->highlight = std::max(0.0f, originalIntensity - 0.6f);

    auto* previous = currentPass;
    // shortcut: the shipped mesh highlightStyle supplies UV and tangent-space view direction; recheck custom shaders.
    currentPass = highlight.effect.owner && std::ranges::equal(style.digest, highlightStyle.digest) ? &highlight : nullptr;
    originalRenderPass(mesh, pass, style, canvas, scene);
    currentPass = previous;
    mesh->highlight = originalIntensity;
}

void __fastcall OnDestroyCharacter(Character* character, void*)
{
    {
        std::lock_guard lock(flashesMutex);
        flashes.erase(character);
        std::erase_if(meshHighlights, [=](const auto& entry) { return entry.second.owner == character; });
    }
    originalDestroyCharacter(character);
}
}

void ormenos::detach_hit_flash_resources_on_process_exit()
{
    // The graphics driver can unload before our static COM destructors run.
    highlightBlend.Detach();
    highlightConstants.Detach();
    highlightShader.Detach();
}

void ormenos::expire_hit_flashes()
{
    const auto now = GetTickCount64();
    const auto* world = state::g_pEngine ? state::g_pEngine->world : nullptr;
    std::lock_guard lock(flashesMutex);
    // Publish UI settings under the same lock used by combat and rendering hooks.
    enabled = g_healthBarConfig.hitFlashesEnabled;
    redHoverRim = g_healthBarConfig.redHoverRimEnabled;
    hoverRimWidth = std::clamp(g_healthBarConfig.hoverRimWidth, 10, 90) / 100.0f;
    hoverRimOpacity = std::clamp(g_healthBarConfig.hoverRimOpacity, 0, 100) / 100.0f;
    meshHighlights.clear();
    durationMs = static_cast<uint64_t>(std::clamp(g_healthBarConfig.hitFlashDurationMs, 10, 1000));
    std::erase_if(flashes, [=](const auto& entry) {
        return !enabled || entry.second.world != world || now - entry.second.started >= durationMs;
    });
}

void ormenos::initialize_hit_flashes(ID3D11Device* device, ID3D11DeviceContext* context)
{
    static bool attempted = false;
    if (attempted) return;
    attempted = true;
    expire_hit_flashes();

    ComPtr<ID3DBlob> bytecode;
    ComPtr<ID3DBlob> errors;
    HRESULT result = D3DCompile(kHitFlashShader, sizeof(kHitFlashShader) - 1, "hit_flash", nullptr, nullptr,
                                "main", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0,
                                bytecode.GetAddressOf(), errors.GetAddressOf());
    if (FAILED(result))
    {
        OutputDebugStringA(errors ? static_cast<const char*>(errors->GetBufferPointer()) : "Hit flash shader compilation failed\n");
        return;
    }
    D3D11_BUFFER_DESC buffer{};
    buffer.ByteWidth = 16;
    buffer.Usage = D3D11_USAGE_DYNAMIC;
    buffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_BLEND_DESC blend{};
    blend.RenderTarget[0].BlendEnable = TRUE;
    blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (FAILED(device->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, highlightShader.GetAddressOf())) ||
        FAILED(device->CreateBuffer(&buffer, nullptr, highlightConstants.GetAddressOf())) ||
        FAILED(device->CreateBlendState(&blend, highlightBlend.GetAddressOf())))
    {
        highlightShader.Reset();
        highlightConstants.Reset();
        highlightBlend.Reset();
        OutputDebugStringA("Enemy highlights disabled: D3D resource creation failed\n");
        return;
    }
    renderContext = context;

    isFoe = reinterpret_cast<IsFoe>(mem::modexport<"Game.dll", "?IsFoe@Team@GAME@@QBE_NABV12@@Z">());
    getMainPlayer = reinterpret_cast<GetMainPlayer>(
        mem::modexport<"Game.dll", "?GetMainPlayer@GameEngine@GAME@@QBEPAVPlayer@2@XZ">());

    struct Hook { void* target; void* detour; void** original; };
    const Hook hooks[] = {
        {reinterpret_cast<void*>(mem::modexport<"Game.dll", "??1Character@GAME@@UAE@XZ">()),
         reinterpret_cast<void*>(&OnDestroyCharacter), reinterpret_cast<void**>(&originalDestroyCharacter)},
        {reinterpret_cast<void*>(mem::modexport<"Game.dll", "?ApplyHighlightRecursive@Character@GAME@@UAEXPAVEntity@2@ABVColor@2@M@Z">()),
         reinterpret_cast<void*>(&OnApplyHighlight), reinterpret_cast<void**>(&originalApplyHighlight)},
        {reinterpret_cast<void*>(mem::modexport<"Engine.dll", "?ApplyHighlightRecursive@Actor@GAME@@UAEXPAVEntity@2@ABVColor@2@M@Z">()),
         reinterpret_cast<void*>(&OnActorHighlight), reinterpret_cast<void**>(&originalActorHighlight)},
        {reinterpret_cast<void*>(mem::modexport<"Engine.dll", "?AddRenderable@GraphicsSceneRenderer@GAME@@QAEXPAVGraphicsRenderable@2@PAVRegion@2@_N@Z">()),
         reinterpret_cast<void*>(&OnAddRenderable), reinterpret_cast<void**>(&originalAddRenderable)},
        {reinterpret_cast<void*>(mem::modexport<"Engine.dll", "?RenderPass@GraphicsMeshInstance@GAME@@UBEXABURenderablePass@2@ABVName@2@AAVGraphicsCanvas@2@ABVGraphicsSceneRenderer@2@@Z">()),
         reinterpret_cast<void*>(&OnRenderPass), reinterpret_cast<void**>(&originalRenderPass)},
        {(*reinterpret_cast<void***>(context))[12],
         reinterpret_cast<void*>(&OnDrawIndexed), reinterpret_cast<void**>(&originalDrawIndexed)},
        {reinterpret_cast<void*>(mem::modexport<"Game.dll", "?TakeAttack@Character@GAME@@AAE_NAAVParametersCombat@2@@Z">()),
         reinterpret_cast<void*>(&OnTakeAttack), reinterpret_cast<void**>(&originalTakeAttack)}
    };
    const auto createName = reinterpret_cast<GAME::Name(__cdecl*)(const char*)>(
        mem::modexport<"Engine.dll", "?Create@Name@GAME@@SA?AV12@PBD@Z">());
    if (!isFoe || !getMainPlayer || !createName || std::any_of(std::begin(hooks), std::end(hooks), [](const Hook& hook) { return !hook.target; }))
    {
        OutputDebugStringA("Hit flashes disabled: required game export missing\n");
        return;
    }
    highlightStyle = createName("highlightStyle");

    size_t created = 0;
    for (const auto& hook : hooks)
    {
        if (MH_CreateHook(hook.target, hook.detour, hook.original) != MH_OK) break;
        ++created;
    }
    if (created == std::size(hooks))
    {
        size_t enabled = 0;
        for (const auto& hook : hooks)
        {
            if (MH_EnableHook(hook.target) != MH_OK) break;
            ++enabled;
        }
        if (enabled == std::size(hooks))
        {
            OutputDebugStringA("Enemy hit flashes installed\n");
            return;
        }
    }
    for (size_t i = 0; i < created; ++i)
    {
        MH_DisableHook(hooks[i].target);
        MH_RemoveHook(hooks[i].target);
    }
    OutputDebugStringA("Hit flashes disabled: hook installation failed\n");
}
