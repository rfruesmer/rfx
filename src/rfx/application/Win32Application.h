#pragma once

#include "rfx/application/Application.h"

namespace rfx
{

class Win32Application : public Application
{
public:
    explicit Win32Application(
        const std::filesystem::path& configurationPath, 
        handle_t instanceHandle);

    void run() override;

protected:
    void createWindow() override;
    void initInputDeviceFactory() override;
    virtual void shutdown() {}

private:
    void updateFrameStats() const;

    HINSTANCE instanceHandle = nullptr;
};

} // namespace rfx
