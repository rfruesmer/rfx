#pragma once

#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{
    
class GraphicsFactory
{
public:
    explicit GraphicsFactory(const std::shared_ptr<Window>& window);
    virtual ~GraphicsFactory() = default;

    virtual void initialize() = 0;
    virtual std::unique_ptr<GraphicsDevice> createDevice() = 0;


protected:
    std::shared_ptr<Window> window;
};

} // namespace rfx
