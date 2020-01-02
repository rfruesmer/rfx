#include "rfx/pch.h"
#include "rfx/graphics/direct3d/D3D12GraphicsFactory.h"
#include "rfx/graphics/direct3d/D3D12GraphicsDevice.h"
#include "rfx/graphics/direct3d/D3D12Exception.h"
#include "rfx/core/StringUtil.h"

using namespace rfx;
using namespace std;
using namespace Microsoft::WRL;

// ---------------------------------------------------------------------------------------------------------------------

D3D12GraphicsFactory::D3D12GraphicsFactory(const shared_ptr<Window>& window)
    : GraphicsFactory(window) {}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsFactory::initialize()
{
#ifdef _DEBUG
    enableDebugging();
#endif

    createFactory();
    logAdapters();
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsFactory::enableDebugging()
{
    ComPtr<ID3D12Debug> debugController;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
    debugController->EnableDebugLayer();
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsFactory::createFactory()
{
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsFactory::logAdapters() const
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    vector<IDXGIAdapter*> adapterList;

    while (dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        RFX_LOG_DEBUG << "Adapter: " << StringUtil::convertWideToAnsiString(desc.Description);

        ReleaseCom(adapter);

        ++i;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

unique_ptr<GraphicsDevice> D3D12GraphicsFactory::createDevice()
{
    unique_ptr<GraphicsDevice> graphicsDevice = make_unique<D3D12GraphicsDevice>(dxgiFactory, window);
    graphicsDevice->initialize();
    
    return graphicsDevice;
}

// ---------------------------------------------------------------------------------------------------------------------
