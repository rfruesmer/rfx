#include "rfx/pch.h"
#include "rfx/application/Application.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

void Application::run()
{
    loadConfiguration();
    createWindow();
    initGraphics();
    runMessageLoop();
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::loadConfiguration()
{
    filesystem::path configFilePath = filesystem::current_path() / 
        filesystem::path("assets/application-config.json");
    ifstream configFile(configFilePath.string(), ifstream::binary);

    configFile >> configuration;
}

// ---------------------------------------------------------------------------------------------------------------------

void Application::initGraphics()
{
    if (graphicsFactory == nullptr)
    {
        throw exception("Illegal state: graphics factory not initialized");
    }
    
    graphicsDevice = graphicsFactory->createDevice();
    graphicsDevice->initialize();
}

// ---------------------------------------------------------------------------------------------------------------------

