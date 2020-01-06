#pragma once


namespace rfx
{

class Window;

enum class WindowState : uint8_t
{
    MINIMIZED,
    MAXIMIZED,
    RESTORED
};

class WindowListener
{
public:
    virtual ~WindowListener() = default;

    virtual void onActivated(Window* window) = 0;
    virtual void onDeactivated(Window* window) = 0;

    virtual void onMinimized(Window* window) = 0;
    virtual void onMaximized(Window* window) = 0;
    virtual void onResizing(Window* window) = 0;
    virtual void onResized(Window* window, int clientWidth, int clientHeight) = 0;
};

class Window
{
public:
    virtual ~Window() = default;

    handle_t getHandle() const;
    const std::string& getTitle() const;
    int getWidth() const;
    int getHeight() const;
    float getAspectRatio() const;

    void addListener(WindowListener* listener);

    virtual void create(const std::string& title, int clientWidth, int clientHeight) = 0;

protected:
    void onActivated();
    void onDeactivated();
    void onMinimized();
    void onMaximized();
    void onResizing();
    void onResized();

    handle_t handle = nullptr;
    std::string title;
    int clientWidth = 0;
    int clientHeight = 0;
    bool minimized = false;
    bool maximized = false;
    bool resizing = false;

private:
    std::vector<WindowListener*> listeners;
};
    
} // namespace rfx
