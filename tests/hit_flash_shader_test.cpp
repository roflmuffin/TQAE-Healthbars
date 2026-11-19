#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "../src/hit_flash_shader.h"

using Microsoft::WRL::ComPtr;

void Check(bool passed, const char* message)
{
    if (!passed) { std::fprintf(stderr, "%s\n", message); std::exit(1); }
}

int main()
{
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    Check(SUCCEEDED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &device, nullptr, &context)), "Create software D3D11 device");

    const char vertexSource[] = R"(
        cbuffer Test : register(b0) { float4 direction; };
        struct Output { float4 position : SV_Position; float2 uv : TEXCOORD0; float3 view : TEXCOORD1; };
        Output main(uint id : SV_VertexID) {
            Output o;
            o.position = float4(id == 2 ? 3 : -1, id == 1 ? 3 : -1, 0, 1);
            o.uv = float2(0.5, 0.5);
            o.view = direction.xyz;
            return o;
        }
    )";
    ComPtr<ID3DBlob> vsCode, psCode, errors;
    Check(SUCCEEDED(D3DCompile(vertexSource, sizeof(vertexSource) - 1, nullptr, nullptr, nullptr,
        "main", "vs_4_0", 0, 0, &vsCode, nullptr)), "Compile test vertex shader");
    const HRESULT compiled = D3DCompile(kHitFlashShader, sizeof(kHitFlashShader) - 1, nullptr, nullptr, nullptr,
        "main", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &psCode, &errors);
    if (errors) std::fprintf(stderr, "%s", static_cast<const char*>(errors->GetBufferPointer()));
    Check(SUCCEEDED(compiled), "Compile production highlight shader");
    ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11PixelShader> ps;
    Check(SUCCEEDED(device->CreateVertexShader(vsCode->GetBufferPointer(), vsCode->GetBufferSize(), nullptr, &vs)), "Create VS");
    Check(SUCCEEDED(device->CreatePixelShader(psCode->GetBufferPointer(), psCode->GetBufferSize(), nullptr, &ps)), "Create PS");

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> target, readback, texture;
    Check(SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &target)), "Create target");
    desc.BindFlags = 0;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    Check(SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &readback)), "Create readback");
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    Check(SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &texture)), "Create texture");
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11ShaderResourceView> srv;
    Check(SUCCEEDED(device->CreateRenderTargetView(target.Get(), nullptr, &rtv)), "Create RTV");
    Check(SUCCEEDED(device->CreateShaderResourceView(texture.Get(), nullptr, &srv)), "Create SRV");
    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    ComPtr<ID3D11SamplerState> sampler;
    Check(SUCCEEDED(device->CreateSamplerState(&samplerDesc, &sampler)), "Create sampler");
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = 16;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    ComPtr<ID3D11Buffer> effect, direction;
    Check(SUCCEEDED(device->CreateBuffer(&bufferDesc, nullptr, &effect)), "Create effect constants");
    Check(SUCCEEDED(device->CreateBuffer(&bufferDesc, nullptr, &direction)), "Create direction constants");
    D3D11_BLEND_DESC blendDesc{};
    auto& blend = blendDesc.RenderTarget[0];
    blend.BlendEnable = TRUE;
    blend.SrcBlend = blend.SrcBlendAlpha = D3D11_BLEND_ONE;
    blend.DestBlend = blend.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend.BlendOp = blend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    ComPtr<ID3D11BlendState> blendState;
    Check(SUCCEEDED(device->CreateBlendState(&blendDesc, &blendState)), "Create blend state");
    D3D11_RASTERIZER_DESC rasterDesc{};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    ComPtr<ID3D11RasterizerState> raster;
    Check(SUCCEEDED(device->CreateRasterizerState(&rasterDesc, &raster)), "Create raster state");

    context->VSSetShader(vs.Get(), nullptr, 0);
    context->PSSetShader(ps.Get(), nullptr, 0);
    context->PSSetConstantBuffers(13, 1, effect.GetAddressOf());
    context->VSSetConstantBuffers(0, 1, direction.GetAddressOf());
    context->PSSetShaderResources(0, 1, srv.GetAddressOf());
    context->PSSetSamplers(0, 1, sampler.GetAddressOf());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
    context->OMSetBlendState(blendState.Get(), nullptr, 0xffffffff);
    context->RSSetState(raster.Get());
    D3D11_VIEWPORT viewport{0, 0, 1, 1, 0, 1};
    context->RSSetViewports(1, &viewport);

    struct Case {
        const char* name;
        float flash, rim, facing, coverage;
        float expected[3];
        float width = 0.6f;
        float opacity = 1.0f;
    };
    const Case cases[] = {
        {"Full white centre", 1, 0, 1, 1, {1, 1, 1}},
        {"Flash wins over red hover", 1, 1, 0, 1, {1, 1, 1}},
        {"Half flash blends with scene", 0.5f, 0, 1, 1, {0.6f, 0.65f, 0.7f}},
        {"Red rim at silhouette", 0, 1, 0, 1, {1, 0.08f, 0.03f}},
        {"Solid rim extends inward", 0, 1, 0.2f, 1, {1, 0.08f, 0.03f}},
        {"Wider rim has soft inner edge", 0, 1, 0.4f, 1, {0.6f, 0.19f, 0.215f}},
        {"Narrow width excludes inner surface", 0, 1, 0.4f, 1, {0.2f, 0.3f, 0.4f}, 0.3f},
        {"Half rim opacity", 0, 1, 0, 1, {0.6f, 0.19f, 0.215f}, 0.6f, 0.5f},
        {"Zero rim opacity", 0, 1, 0, 1, {0.2f, 0.3f, 0.4f}, 0.6f, 0},
        {"Rim opacity does not weaken hits", 1, 1, 0, 1, {1, 1, 1}, 0.6f, 0},
        {"Hover leaves centre unchanged", 0, 1, 1, 1, {0.2f, 0.3f, 0.4f}},
        {"No lingering flash", 0, 0, 0, 1, {0.2f, 0.3f, 0.4f}},
        {"Texture cutouts preserved", 1, 1, 0, 0, {0.2f, 0.3f, 0.4f}}
    };
    for (const auto& test : cases)
    {
        const float background[4] = {0.2f, 0.3f, 0.4f, 1};
        const float constants[4] = {test.flash, test.rim, test.width, test.opacity};
        const float view[4] = {std::sqrt(1 - test.facing * test.facing), 0, test.facing, 0};
        const float texel[4] = {0, 0, 0, test.coverage};
        context->UpdateSubresource(effect.Get(), 0, nullptr, constants, 0, 0);
        context->UpdateSubresource(direction.Get(), 0, nullptr, view, 0, 0);
        context->UpdateSubresource(texture.Get(), 0, nullptr, texel, sizeof(texel), 0);
        context->ClearRenderTargetView(rtv.Get(), background);
        context->Draw(3, 0);
        context->CopyResource(readback.Get(), target.Get());
        D3D11_MAPPED_SUBRESOURCE mapped{};
        Check(SUCCEEDED(context->Map(readback.Get(), 0, D3D11_MAP_READ, 0, &mapped)), "Read pixels");
        const auto* pixel = static_cast<const float*>(mapped.pData);
        for (int i = 0; i < 3; ++i) Check(std::abs(pixel[i] - test.expected[i]) < 0.002f, test.name);
        context->Unmap(readback.Get(), 0);
        std::printf("PASS: %s\n", test.name);
    }
}
