#pragma once

#include "rfx/graphics/GraphicsFactory.h"

namespace rfx
{
    
class D3D12GraphicsFactory : public GraphicsFactory
{
public:
    explicit D3D12GraphicsFactory(const std::shared_ptr<Window>& window);
    
    void initialize() override;

    std::unique_ptr<GraphicsDevice> createDevice() override;

private:
    void enableDebugging();
    void createFactory();
    void logAdapters() const;

    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
};
    
} // namespace rfx
