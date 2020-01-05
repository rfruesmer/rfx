#pragma once

#include "rfx/application/Window.h"


namespace rfx
{

class Win32Window : public Window
{
public:
    explicit Win32Window(HINSTANCE instanceHandle);

    void create(const std::string& title, int clientWidth, int clientHeight) override;

    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
private:
    void registerWindowClass();
    void createWindow(const std::string& title, int clientWidth, int clientHeight);
    static WindowState toState(WPARAM wParam);

    HINSTANCE instanceHandle = nullptr;
};

} // namespace rfx
