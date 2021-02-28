#include "rfx/pch.h"
#include "rfx/application/Window.h"
#include <rfx/common/Algorithm.h>


using namespace rfx;
using namespace std;
namespace ranges = ranges;

// ---------------------------------------------------------------------------------------------------------------------

void Window::create(const string& title, int width, int height)
{
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        RFX_THROW("Failed to create window");
    }

    glfwGetFramebufferSize(window, &width_, &height_);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, onKeyEvent);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetCursorEnterCallback(window, onCursorEntered);
    glfwSetCursorPosCallback(window, onCursorPos);
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onResize(GLFWwindow* window, int width, int height)
{
    auto rfxWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    rfxWindow->onResize(width, height);
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onResize(int width, int height)
{
    width_ = width;
    height_ = height;

    ranges::for_each(listeners,
        [this, width, height](const weak_ptr<WindowListener>& weakListener) {
            if (auto listener = weakListener.lock()) {
                listener->onResized(*this, width, height);
            }
        });
}

// ---------------------------------------------------------------------------------------------------------------------

GLFWwindow* Window::getGlfwWindow() const
{
    return window;
}

// ---------------------------------------------------------------------------------------------------------------------

void* Window::getHandle() const
{
    return glfwGetWin32Window(window);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Window::getClientWidth() const
{
    return width_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Window::getClientHeight() const
{
    return height_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::addListener(const shared_ptr<WindowListener>& listener)
{
    if (!contains(listeners, listener)) {
        listeners.push_back(listener);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto rfxWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    rfxWindow->onKeyEvent(key, scancode, action, mods);
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onKeyEvent(int key, int scancode, int action, int mods)
{
    ranges::for_each(listeners,
        [this, key, scancode, action, mods](const weak_ptr<WindowListener>& weakListener) {
            if (auto listener = weakListener.lock()) {
                listener->onKeyEvent(*this, key, scancode, action, mods);
            }
        });
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onCursorPos(GLFWwindow* window, double x, double y)
{
    auto rfxWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    rfxWindow->onCursorPos(static_cast<float>(x), static_cast<float>(y));
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onCursorPos(float x, float y)
{
    ranges::for_each(listeners,
        [this, x, y](const weak_ptr<WindowListener>& weakListener) {
            if (auto listener = weakListener.lock()) {
                listener->onCursorPos(*this, x, y);
            }
        });
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onCursorEntered(GLFWwindow* window, int entered)
{
    auto rfxWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    rfxWindow->onCursorEntered(entered);
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::onCursorEntered(bool entered)
{
    ranges::for_each(listeners,
        [this, entered](const weak_ptr<WindowListener>& weakListener) {
            if (auto listener = weakListener.lock()) {
                listener->onCursorEntered(*this, entered);
            }
        });
}

// ---------------------------------------------------------------------------------------------------------------------

