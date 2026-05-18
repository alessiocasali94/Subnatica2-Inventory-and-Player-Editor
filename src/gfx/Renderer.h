#pragma once

#include <d3d11.h>
#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

namespace gfx {

class Renderer {
public:
    bool Init(HWND hwnd, int width, int height);
    void Resize(int width, int height);
    // clearColor: RGBA with alpha=1 for a solid backbuffer (alpha=0 shows black on non-layered HWND).
    void BeginFrame(const float clearColor[4]);
    void EndFrame();
    void Shutdown();

    ID3D11Device* Device() const { return device_.Get(); }
    ID3D11DeviceContext* Context() const { return context_.Get(); }

private:
    void CreateRenderTarget();

    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv_;
    int width_ = 0;
    int height_ = 0;
};

} // namespace gfx
