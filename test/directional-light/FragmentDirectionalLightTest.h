#pragma once

#include "test/TestApplication.h"


namespace rfx
{

class FragmentDirectionalLightTest final : public TestApplication
{
public:
    explicit FragmentDirectionalLightTest(handle_t instanceHandle);

protected:
    FragmentDirectionalLightTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void createEffectFactories() override;
};

}
