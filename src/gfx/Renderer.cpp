#include "gfx/Renderer.h"

#include <dxgi.h>

namespace gfx {

bool Renderer::Init(HWND hwnd, int width, int height) {
    width_ = width;
    height_ = height;

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0;
    D3D_FEATURE_LEVEL level{};
    const D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0};

    if (FAILED(D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, levels, 1,
            D3D11_SDK_VERSION, &sd, swapChain_.GetAddressOf(), device_.GetAddressOf(),
            &level, context_.GetAddressOf())))
        return false;

    CreateRenderTarget();

    D3D11_BLEND_DESC blend{};
    blend.RenderTarget[0].BlendEnable = TRUE;
    blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    Microsoft::WRL::ComPtr<ID3D11BlendState> bs;
    device_->CreateBlendState(&blend, bs.GetAddressOf());
    float factor[4] = {0, 0, 0, 0};
    context_->OMSetBlendState(bs.Get(), factor, 0xffffffff);

    return true;
}

void Renderer::CreateRenderTarget() {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    device_->CreateRenderTargetView(backBuffer.Get(), nullptr, rtv_.ReleaseAndGetAddressOf());
}

void Renderer::Resize(int width, int height) {
    if (width <= 0 || height <= 0)
        return;
    width_ = width;
    height_ = height;
    rtv_.Reset();
    swapChain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTarget();
}

void Renderer::BeginFrame(const float clearColor[4]) {
    static const float kDefaultClear[4] = {0.02f, 0.05f, 0.12f, 1.f};
    const float* clear = clearColor ? clearColor : kDefaultClear;
    context_->OMSetRenderTargets(1, rtv_.GetAddressOf(), nullptr);
    context_->ClearRenderTargetView(rtv_.Get(), clear);
}

void Renderer::EndFrame() {
    swapChain_->Present(1, 0);
}

void Renderer::Shutdown() {
    rtv_.Reset();
    swapChain_.Reset();
    context_.Reset();
    device_.Reset();
}

} // namespace gfx
