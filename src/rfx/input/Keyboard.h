#pragma once

#include "InputDevice.h"


namespace rfx
{

class Keyboard : public InputDevice
{
public:
    enum Key
    {
        KEY_W,
        KEY_S,
        KEY_A,
        KEY_D,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_UP,
        KEY_DOWN
    };

    virtual bool isKeyDown(Key key) = 0;

protected:
    Keyboard() = default;
};

}
