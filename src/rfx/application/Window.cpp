#include "rfx/pch.h"
#include "rfx/application/Window.h"
#include <rfx/common/Algorithm.h>


using namespace rfx;
using namespace std;
namespace ranges = ranges;

// ---------------------------------------------------------------------------------------------------------------------

static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Window::create(const string& title, int width, int height)
{
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        RFX_THROW("Failed to create window");
    }
    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, onKeyEvent);
    glfwSetFramebufferSizeCallback(window, onResize);
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

void Window::addListener(const shared_ptr<WindowListener>& listener)
{
    if (!contains(listeners, listener)) {
        listeners.push_back(listener);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

