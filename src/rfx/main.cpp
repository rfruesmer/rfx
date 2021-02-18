#include "rfx/pch.h"
#include "rfx/application/Application.h"
#include "rfx/common/Logger.h"

#include "ColoredQuad.h"
#include "TexturedQuad.h"


using namespace rfx;
using namespace rfx::test;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
//        auto theApp = make_shared<ColoredQuad>();
        auto theApp = make_shared<TexturedQuad>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------