#include "rfx/pch.h"
#include "rfx/application/Win32Window.h"
#include "rfx/core/StringUtil.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

static const wstring WINDOW_CLASS = L"rfx";

static Win32Window* mainWindow = nullptr;

// ---------------------------------------------------------------------------------------------------------------------

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        const auto createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        mainWindow = reinterpret_cast<Win32Window*>(createStruct->lpCreateParams);
    }

    if (mainWindow)
    {
        return mainWindow->WndProc(hwnd, msg, wParam, lParam);
    }
        
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------------------------------------------------

LRESULT Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYUP:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        break;

    case WM_ENTERSIZEMOVE:
        onResizing();
        break;

    case WM_EXITSIZEMOVE:
        onResized();
        break;

    case WM_SIZE:
        clientWidth = LOWORD(lParam);
        clientHeight = HIWORD(lParam);
        if (wParam == SIZE_MINIMIZED)
        {
            onMinimized();
        }
        else if (wParam == SIZE_MAXIMIZED)
        {
            onMaximized();
        }
        else if (!resizing)
        {
            onResized();
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

Win32Window::Win32Window(HINSTANCE instanceHandle)
    : instanceHandle(instanceHandle)
{
    if (instanceHandle == nullptr)
    {
        throw invalid_argument("Invalid instance handle");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Window::create(const std::string& title, int clientWidth, int clientHeight)
{
    registerWindowClass();
    createWindow(title, clientWidth, clientHeight);
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Window::registerWindowClass()
{
    const HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(instanceHandle);

    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RFX));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = WINDOW_CLASS.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    
    const ATOM classId = RegisterClassExW(&wcex);
    if (classId == 0)
    {
        throw exception("Failed to register window class");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Win32Window::createWindow(const std::string& title, int clientWidth, int clientHeight)
{
    const HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(instanceHandle);
    const DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    
    RECT windowRect = { 0, 0, clientWidth, clientHeight };
    if (!AdjustWindowRectEx(&windowRect, dwStyle, FALSE, 0)) // TODO: make DPI-aware
    {
        throw exception("Invalid window size");
    }

    const HWND hwnd = CreateWindowA(StringUtil::convertWideToAnsiString(WINDOW_CLASS).c_str(), title.c_str(), dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, 
        nullptr, nullptr, hInstance, this);

    if (hwnd == nullptr)
    {
        throw exception("Failed to create native window");
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    RECT clientRect = { 0 };
    GetClientRect(hwnd, &clientRect);
    clientWidth = clientRect.right - clientRect.left;
    clientHeight = clientRect.bottom - clientRect.top;

    handle = static_cast<handle_t>(hwnd);
    this->title = title;
}

// ---------------------------------------------------------------------------------------------------------------------

WindowState Win32Window::toState(WPARAM wParam)
{
    switch (wParam)
    {
    case SIZE_MINIMIZED:
        return WindowState::MINIMIZED;
    case SIZE_MAXIMIZED:
        return WindowState::MAXIMIZED;
    default:
        return WindowState::RESTORED;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
