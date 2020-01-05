#include "rfx/pch.h"
#include "rfx/application/Window.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

handle_t Window::getHandle() const
{
    return handle;
}

// ---------------------------------------------------------------------------------------------------------------------

const string& Window::getTitle() const
{
    return title;
}

// ---------------------------------------------------------------------------------------------------------------------

int Window::getWidth() const
{
    return clientWidth;
}

// ---------------------------------------------------------------------------------------------------------------------

int Window::getHeight() const
{
    return clientHeight;
}

// ---------------------------------------------------------------------------------------------------------------------

float Window::getAspectRatio() const
{
    return static_cast<float>(clientWidth)
         / static_cast<float>(clientHeight);
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::addListener(WindowListener* listener)
{
    if (find(listeners.begin(), listeners.end(), listener) == listeners.end())
    {
        listeners.push_back(listener);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onMinimized()
{
    resizing = false;
    maximized = false;
    minimized = true;

    for (WindowListener* listener : listeners)
    {
        listener->onMinimized(this);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onMaximized()
{
    resizing = false;
    maximized = true;
    minimized = false;

    for (WindowListener* listener : listeners)
    {
        listener->onMaximized(this);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onResizing()
{
    resizing = true;

    for (WindowListener* listener : listeners)
    {
        listener->onResizing(this);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onResized()
{
    resizing = false;

    for (WindowListener* listener : listeners)
    {
        listener->onResized(this, clientWidth, clientHeight);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
