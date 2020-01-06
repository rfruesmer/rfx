#include "rfx/pch.h"
#include "rfx/input/directx/DXInputDeviceFactory.h"
#include "rfx/input/directx/DXKeyboard.h"

using namespace rfx;
using namespace std;
using namespace Microsoft::WRL;

// ---------------------------------------------------------------------------------------------------------------------

DXInputDeviceFactory::DXInputDeviceFactory(HINSTANCE instanceHandle)
{
    const HRESULT hr = DirectInput8Create(instanceHandle, DIRECTINPUT_VERSION, 
        IID_IDirectInput8, static_cast<void**>(&directInput), nullptr);
    RFX_CHECK_STATE(SUCCEEDED(hr) && directInput != nullptr, 
        "Failed to create IDirectInput8 instance");
}

// ---------------------------------------------------------------------------------------------------------------------

unique_ptr<Keyboard> DXInputDeviceFactory::createKeyboard(handle_t windowHandle)
{
    return make_unique<DXKeyboard>(directInput, static_cast<HWND>(windowHandle));
}

// ---------------------------------------------------------------------------------------------------------------------
