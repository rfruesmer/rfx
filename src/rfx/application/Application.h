#pragma once

#include "rfx/graphics/window/Window.h"
#include "rfx/graphics/GraphicsContext.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/input/InputDeviceFactory.h"
#include "rfx/core/Timer.h"

namespace rfx
{

class Application : public WindowListener
{
public:
    explicit Application(const std::filesystem::path& configurationPath);
    virtual ~Application() = default;

    virtual void initialize();

    void onActivated(Window* window) override;
    void onDeactivated(Window* window) override;
    void onMinimized(Window* window) override;
    void onMaximized(Window* window) override;
    void onResizing(Window* window) override;
    void onResized(Window* window, int clientWidth, int clientHeight) override;

    virtual void run() = 0;

protected:
    virtual void loadConfiguration();
    void initLogger();
    virtual void createWindow() = 0;
    virtual void initGraphics();
    virtual void initInputDeviceFactory() = 0;
    virtual void initInput();

    virtual void update();
    virtual void draw() {}

    std::filesystem::path configurationPath;
    std::string name;
    Timer timer;
    bool paused = false;
    Json::Value configuration;
    std::shared_ptr<Window> window;
    bool windowResized = false;
    std::unique_ptr<GraphicsContext> graphicsContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::unique_ptr<InputDeviceFactory> inputDeviceFactory;
    std::unique_ptr<Keyboard> keyboard;
};

} // namespace rfx

