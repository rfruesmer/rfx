#include "rfx/pch.h"
#include "rfx/application/Application.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

void Application::initialize()
{
    loadConfiguration();
    initLogger();
    createWindow();
    initGraphics();
    initInputDeviceFactory();
    initInput();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::loadConfiguration()
{
    const filesystem::path configFilePath = filesystem::current_path() / 
        filesystem::path("assets/application-config.json");
    ifstream configFile(configFilePath.string(), ifstream::binary);

    configFile >> configuration;

    name = configuration["application"]["name"].asString();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initLogger()
{
    const LogLevel configuredLogLevel = Logger::toLogLevel(configuration["application"]["logLevel"].asString());
    Logger::setLogLevel(configuredLogLevel);
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initGraphics()
{
    graphicsContext = make_unique<GraphicsContext>();
    graphicsContext->initialize(name, window, {});

    VkPhysicalDeviceFeatures features = {};
    features.geometryShader = VK_TRUE;
    features.samplerAnisotropy = VK_TRUE;

    graphicsDevice = graphicsContext->createGraphicsDevice(features, {},
        { VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT }, window);

    onResized(window.get(), window->getWidth(), window->getHeight());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initInput()
{
    keyboard = inputDeviceFactory->createKeyboard(window->getHandle());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::update()
{
    keyboard->update();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onActivated(Window* window)
{
    if (keyboard) {
        keyboard->acquire();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onDeactivated(Window* window)
{
    if (keyboard) {
        keyboard->unacquire();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onMinimized(Window* window)
{
    paused = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onMaximized(Window* window)
{
    onResized(window, window->getWidth(), window->getHeight());
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onResizing(Window* window)
{
    paused = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::onResized(Window* window, int clientWidth, int clientHeight)
{
    paused = false;
    windowResized = true;
}

// ---------------------------------------------------------------------------------------------------------------------
