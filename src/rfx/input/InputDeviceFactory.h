#pragma once

#include "rfx/input/Keyboard.h"


namespace rfx
{
class InputDeviceFactory
{
public:
    InputDeviceFactory() = default;
    InputDeviceFactory(const InputDeviceFactory&) = delete;
    InputDeviceFactory(InputDeviceFactory&&) = delete;
    InputDeviceFactory& operator =(const InputDeviceFactory&) = delete;
    InputDeviceFactory& operator =(InputDeviceFactory&&) = delete;

    virtual ~InputDeviceFactory() = default;

    virtual std::unique_ptr<Keyboard> createKeyboard(handle_t windowHandle) = 0;
};
}
