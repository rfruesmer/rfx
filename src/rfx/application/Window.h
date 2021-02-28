#pragma once

namespace rfx {

class Window;

class WindowListener
{
public:
    virtual ~WindowListener() = default;

    virtual void onKeyEvent(const Window& window, int key, int scancode, int action, int mods) {};
    virtual void onResized(const Window& window, int width, int height) {};
    virtual void onCursorEntered(const Window& window, bool entered) {};
    virtual void onCursorPos(const Window& window, float x, float y) {};
};

class Window
{
public:
    void create(const std::string& title, int width, int height);

    [[nodiscard]] GLFWwindow* getGlfwWindow() const;
    [[nodiscard]] void* getHandle() const;
    [[nodiscard]] uint32_t getClientWidth() const;
    [[nodiscard]] uint32_t getClientHeight() const;

    void addListener(const std::shared_ptr<WindowListener>& listener);

private:
    static void onResize(GLFWwindow* window, int width, int height);
    static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void onCursorPos(GLFWwindow* window, double x, double y);
    static void onCursorEntered(GLFWwindow* window, int entered);

    void onKeyEvent(int key, int scancode, int action, int mods);
    void onResize(int width, int height);
    void onCursorPos(float x, float y);
    void onCursorEntered(bool entered);

    GLFWwindow* window = nullptr;
    int width_ = 0;
    int height_ = 0;
    std::vector<std::shared_ptr<WindowListener>> listeners; // TODO: use weak_ptr
};

} // namespace rfx