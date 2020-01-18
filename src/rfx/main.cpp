// main.cpp : Defines the entry point for the application.
//

#include "rfx/pch.h"

#ifdef _WINDOWS
#include "rfx/application/Win32Application.h"
#else
static_assert(false, "Not implemented");
#endif // _WINDOWS

#include "test/TriangleTest.h"
#include "test/TexturedQuadTest.h"
#include "test/CubeTest.h"
#include "test/TexturedCubesTest.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

#ifdef _WINDOWS

#if 1
int APIENTRY wWinMain(HINSTANCE instanceHandle,
    HINSTANCE prevInstance,
    LPWSTR commandLine,
    int showParam) {
#else
int main()
{
    HINSTANCE instanceHandle = GetModuleHandle(nullptr);
#endif 

    try
    {
        RFX_LOG_INFO << "Starting rfx application ...";

        //TriangleTest theApp(instanceHandle);
        //CubeTest theApp(instanceHandle);
        //TexturedQuadTest theApp(instanceHandle);
        TexturedCubesTest theApp(instanceHandle);
        theApp.initialize();
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

// ---------------------------------------------------------------------------------------------------------------------
