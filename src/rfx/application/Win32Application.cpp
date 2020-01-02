#include "rfx/pch.h"
#include "rfx/application/Win32Application.h"
#include "rfx/application/Win32Window.h"
#include "rfx/graphics/direct3d/D3D12GraphicsFactory.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Win32Application::Win32Application(HINSTANCE instanceHandle)
	: instanceHandle(instanceHandle)
{
	if (instanceHandle == nullptr)
	{
		throw invalid_argument("Invalid instance handle");
	}
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Application::createWindow()
{
	if (window != nullptr)
	{
		throw exception("Illegal state: window already created");
	}

	string windowTitle = configuration["application"]["windowTitle"].asString();
	Json::Value resolution = configuration["graphics"]["resolution"];

	window = make_unique<Win32Window>(instanceHandle);
	window->create(windowTitle, resolution["x"].asInt(), resolution["y"].asInt());
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Application::initGraphics()
{
	const string renderer = configuration["graphics"]["renderer"].asString();
	if (renderer == "d3d12")
	{
		graphicsFactory = make_unique<D3D12GraphicsFactory>(window);
	}
	else
	{
		const string message = "Renderer not implemented: " + renderer;
		throw exception(message.c_str());
	}

	graphicsFactory->initialize();

	Application::initGraphics();
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Application::runMessageLoop()
{
	timer.start();
	
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.tick();

			if (!paused)
			{
				updateFrameStats();
			}
			else
			{
				Sleep(100);
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Application::updateFrameStats() const
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;
	timeElapsed += timer.getElapsedTime();

	if (timeElapsed >= 1.0f)
	{
		const float fps = static_cast<float>(frameCount);
		const float fms = 1000.0f / fps;

		const string windowText = window->getTitle() +
			" [FPS: " + to_string(static_cast<int>(fps))  +
			" / FMS: " + to_string(fms) + "]";

		SetWindowTextA(static_cast<HWND>(window->getHandle()), windowText.c_str());
		
		frameCount = 0;
		timeElapsed = 0.0f;
	}
}

// ---------------------------------------------------------------------------------------------------------------------
