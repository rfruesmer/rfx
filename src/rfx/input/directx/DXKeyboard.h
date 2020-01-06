#pragma once

#include "rfx/input/Keyboard.h"

namespace rfx
{

class DXKeyboard : public Keyboard
{
public:
    explicit DXKeyboard(const Microsoft::WRL::ComPtr<IDirectInput8>& directInput, HWND windowHandle);
    ~DXKeyboard();

    void acquire() override;
    void unacquire() override;
    void update() override;
    bool isKeyDown(Key key) override;

    void dispose() override;

private:
    Microsoft::WRL::ComPtr<IDirectInputDevice8> dxInputDevice;
    unsigned char state[256] = {};
};

}
