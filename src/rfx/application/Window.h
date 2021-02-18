#pragma once

namespace rfx {

class Window;

class WindowListener
{
public:
    virtual ~WindowListener() = default;

    virtual void onResized(const Window& window, int width, int height) = 0;
};

class Window
{
public:
    void create(const std::string& title, int width, int height);

    [[nodiscard]] GLFWwindow* getGlfwWindow() const;
    [[nodiscard]] void* getHandle() const;

    void addListener(const std::shared_ptr<WindowListener>& listener);

private:
    static void onResize(GLFWwindow* window, int width, int height);
    void onResize(int width, int height);

    GLFWwindow* window = nullptr;
    std::vector<std::shared_ptr<WindowListener>> listeners; // TODO: use weak_ptr
};

} // namespace rfx