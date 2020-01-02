#include "rfx/pch.h"
#include "rfx/graphics/direct3d/D3D12GraphicsDevice.h"
#include "rfx/graphics/direct3d/D3D12Exception.h"

using namespace rfx;
using namespace std;
using namespace Microsoft::WRL;

// ---------------------------------------------------------------------------------------------------------------------

D3D12GraphicsDevice::D3D12GraphicsDevice(ComPtr<IDXGIFactory4> dxgiFactory,	const shared_ptr<Window>& window)
	: window(window), dxgiFactory(dxgiFactory) {}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsDevice::initialize()
{
	createDevice();
	createFence();
	queryDescriptorSizes();
	queryMsaaQuality();
	createCommandObjects();
	createSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsDevice::createDevice()
{
	HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
	ThrowIfFailed(hr);
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsDevice::createFence()
{
	HRESULT hr = d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	ThrowIfFailed(hr);
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsDevice::queryDescriptorSizes()
{
	rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	cbvSrvUavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsDevice::queryMsaaQuality()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	const HRESULT hr = d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels, sizeof(msQualityLevels));
	ThrowIfFailed(hr);

	msaaQuality = msQualityLevels.NumQualityLevels;
	if (msaaQuality == 0)
	{
		throw exception("Unexpected MSAA quality level");
	}
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsDevice::createCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

	ThrowIfFailed(d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(commandAllocator.GetAddressOf())));

	ThrowIfFailed(d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(commandList.GetAddressOf())));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	commandList->Close();
}

// ---------------------------------------------------------------------------------------------------------------------

void D3D12GraphicsDevice::createSwapChain()
{
	// Release the previous swapchain we will be recreating.
	swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = window->getWidth();
	sd.BufferDesc.Height = window->getHeight();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = backBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = msaaEnabled ? 4 : 1;
	sd.SampleDesc.Quality = msaaEnabled ? (msaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SWAPCHAIN_BUFFERCOUNT;
	sd.OutputWindow = static_cast<HWND>(window->getHandle());
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(dxgiFactory->CreateSwapChain(
		commandQueue.Get(),
		&sd,
		swapChain.GetAddressOf()));
}

// ---------------------------------------------------------------------------------------------------------------------
