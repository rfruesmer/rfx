#include "rfx/pch.h"
#include "SceneTest.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<SceneTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::initGraphics()
{
    Application::initGraphics();

    createSyncObjects();
}

// ---------------------------------------------------------------------------------------------------------------------
