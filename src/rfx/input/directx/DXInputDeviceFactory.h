#pragma once

#include "rfx/input/InputDeviceFactory.h"


namespace rfx
{

class DXInputDeviceFactory : public InputDeviceFactory
{
public:
    DXInputDeviceFactory(HINSTANCE instanceHandle);

    std::unique_ptr<Keyboard> createKeyboard(handle_t windowHandle) override;

private:
    Microsoft::WRL::ComPtr<IDirectInput8> directInput;
};

}
