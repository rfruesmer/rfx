// rfx.cpp : Defines the entry point for the application.
//

#include "rfx/pch.h"

#ifdef _WINDOWS
#include "rfx/application/Win32Application.h"
#else
static_assert(false, "Not implemented");
#endif // _WINDOWS

using namespace rfx;
using namespace std;


#ifdef _WINDOWS

int APIENTRY wWinMain(HINSTANCE instanceHandle,
    HINSTANCE prevInstance,
    LPWSTR commandLine,
    int showParam)
{
    try
    {
        RFX_LOG_INFO << "Starting rfx application ...";

        Logger::setLogLevel(LogLevel::TRACE);

        Win32Application theApp(instanceHandle);
        theApp.run();

        return EXIT_SUCCESS;
    }
    catch (const exception & ex)
    {
        RFX_LOG_FATAL << ex.what();
    }
    catch (...)
    {
        RFX_LOG_FATAL << "Unknown exception";
    }

    return EXIT_FAILURE;
}

#else // _WINDOWS
static_assert(false, "Not implemented");
#endif // _WINDOWS




