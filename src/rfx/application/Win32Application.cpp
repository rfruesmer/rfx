#include "rfx/pch.h"
#include "rfx/application/Win32Application.h"
#include "rfx/application/Win32Window.h"
#include "rfx/input/directx/DXInputDeviceFactory.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Win32Application::Win32Application(
    std::filesystem::path configurationPath,
    handle_t instanceHandle)
        : Application(configurationPath),
          instanceHandle(static_cast<HINSTANCE>(instanceHandle))
{
    RFX_CHECK_ARGUMENT(instanceHandle != nullptr, "Invalid instance handle");
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Application::createWindow()
{
    RFX_CHECK_STATE(window == nullptr, "window already created");

    string windowTitle = configuration["application"]["name"].asString();
    Json::Value resolution = configuration["graphics"]["resolution"];

    window = make_unique<Win32Window>(instanceHandle);
    window->create(windowTitle, resolution["x"].asInt(), resolution["y"].asInt());
    window->addListener(this);
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Application::initInputDeviceFactory()
{
    inputDeviceFactory = make_unique<DXInputDeviceFactory>(instanceHandle);
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Application::run()
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
                update();
                draw();
            }
            else
            {
                Sleep(100);
            }
        }
    }

    shutdown();
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
