#pragma once

#include "rfx/application/Window.h"
#include "rfx/graphics/GraphicsFactory.h"
#include "rfx/core/Timer.h"

namespace rfx
{

class Application
{
public:
	virtual ~Application() = default;

	void run();

protected:
	virtual void loadConfiguration();
	virtual void createWindow() = 0;
	virtual void initGraphics();
	virtual void runMessageLoop() = 0;

	bool paused = false;
	Json::Value configuration;
	Timer timer;
	std::shared_ptr<Window> window;
	std::unique_ptr<GraphicsFactory> graphicsFactory;
	std::unique_ptr<GraphicsDevice> graphicsDevice;
};

} // namespace rfx
