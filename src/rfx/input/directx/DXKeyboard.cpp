#include "rfx/pch.h"
#include "rfx/input/directx/DXKeyboard.h"

using namespace rfx;
using namespace std;
using namespace Microsoft::WRL;

// ---------------------------------------------------------------------------------------------------------------------

DXKeyboard::DXKeyboard(const ComPtr<IDirectInput8>& directInput, HWND windowHandle)
{
    HRESULT hr = directInput->CreateDevice(GUID_SysKeyboard, &dxInputDevice, nullptr);
    RFX_CHECK_STATE(SUCCEEDED(hr) && dxInputDevice != nullptr,
        "Failed to create keyboard instance");

    hr = dxInputDevice->SetDataFormat(&c_dfDIKeyboard);
    RFX_CHECK_STATE(SUCCEEDED(hr), "Failed to set keyboard format");

    hr = dxInputDevice->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    RFX_CHECK_STATE(SUCCEEDED(hr), "Failed to set cooperative level for keyboard");
}

// ---------------------------------------------------------------------------------------------------------------------

DXKeyboard::~DXKeyboard()
{
    DXKeyboard::dispose();
}

// ---------------------------------------------------------------------------------------------------------------------

void DXKeyboard::acquire()
{
    const HRESULT hr = dxInputDevice->Acquire();
    if (FAILED(hr)) {
        RFX_LOG_ERROR << "Failed to acquire keyboard";
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DXKeyboard::unacquire()
{
    const HRESULT hr = dxInputDevice->Unacquire();
    if (FAILED(hr)) {
        RFX_LOG_ERROR << "Failed to unacquire keyboard";
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DXKeyboard::update()
{
    HRESULT hr = dxInputDevice->GetDeviceState(sizeof(state), static_cast<void*>(&state));
    if (FAILED(hr)) {
        if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
            dxInputDevice->Acquire();
        }
        else {
            RFX_LOG_ERROR << "Failed to get keyboard state";
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool DXKeyboard::isKeyDown(Key key)
{
    static unordered_map<Key, unsigned char> keyMapping = {
        {KEY_W, DIK_W},
        {KEY_A, DIK_A},
        {KEY_S, DIK_S},
        {KEY_D, DIK_D},
        {KEY_LEFT, DIK_LEFT},
        {KEY_RIGHT, DIK_RIGHT},
        {KEY_UP, DIK_UP},
        {KEY_DOWN, DIK_DOWN}
    };

    const auto it = keyMapping.find(key);
    if (it != keyMapping.end()) {
        return state[it->second];
    }
    else {
        RFX_LOG_ERROR << "Undefined key: " << key;
    }

    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

void DXKeyboard::dispose()
{
    if (dxInputDevice != nullptr) {
        unacquire();
        dxInputDevice.Reset();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
