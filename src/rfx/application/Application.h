#pragma once

#include "rfx/application/Window.h"
#include "rfx/graphics/GraphicsContext.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/core/Timer.h"

namespace rfx
{

class Application : public WindowListener
{
public:
    virtual ~Application() = default;

    virtual void initialize();

    void onMinimized(Window* window) override;
    void onMaximized(Window* window) override;
    void onResizing(Window* window) override;
    void onResized(Window* window, int clientWidth, int clientHeight) override;

    virtual void run() = 0;

protected:
    virtual void loadConfiguration();
    virtual void createWindow() = 0;
    virtual void initGraphics();

    virtual void update() {}
    virtual void draw() {}

    std::string name;
    Timer timer;
    bool paused = false;
    Json::Value configuration;
    std::shared_ptr<Window> window;
    std::unique_ptr<GraphicsContext> graphicsContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
};

} // namespace rfx

