#pragma once


namespace rfx
{

class InputDevice
{
public:
    InputDevice() = default;
    InputDevice(const InputDevice&) = delete;
    InputDevice(InputDevice&&) = delete;
    InputDevice& operator =(const InputDevice&) = delete;
    InputDevice& operator =(InputDevice&&) = delete;

    virtual ~InputDevice() = default;

    virtual void acquire() = 0;
    virtual void unacquire() = 0;
    virtual void update() = 0;
    virtual void dispose() = 0;
};

}