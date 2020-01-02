#pragma once

#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class D3D12GraphicsDevice : public GraphicsDevice
{
public:
    D3D12GraphicsDevice(Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory,
        const std::shared_ptr<Window>& window);

    void initialize() override;

private:
    void createDevice();
    void createFence();
    void queryDescriptorSizes();
    void queryMsaaQuality();
    void createCommandObjects();
    void createSwapChain();

    std::shared_ptr<Window> window;
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

    UINT rtvDescriptorSize = 0;
    UINT dsvDescriptorSize = 0;
    UINT cbvSrvUavDescriptorSize = 0;

    bool msaaEnabled = false;
    UINT msaaQuality = 0;

    static const int SWAPCHAIN_BUFFERCOUNT = 2;
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
};

} // namespace rfx
