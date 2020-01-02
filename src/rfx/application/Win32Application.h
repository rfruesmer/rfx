#pragma once

#include "rfx/application/Application.h"

namespace rfx
{

class Win32Application : public Application
{
public:
	explicit Win32Application(HINSTANCE instanceHandle);

protected:
	void createWindow() override;
	void initGraphics() override;
	void runMessageLoop() override;

private:
	void updateFrameStats() const;

	HINSTANCE instanceHandle = nullptr;
};

} // namespace rfx
