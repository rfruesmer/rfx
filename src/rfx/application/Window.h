#pragma once


namespace rfx
{
class Window
{
public:
	virtual ~Window() = default;

	handle_t getHandle() const;
	const std::string& getTitle() const;
	int getWidth() const;
	int getHeight() const;
	
	virtual void create(const std::string& title, int clientWidth, int clientHeight) = 0;

protected:
	handle_t handle = nullptr;
	std::string title;
	int width = 0;
	int height = 0;
};
	
} // namespace rfx
