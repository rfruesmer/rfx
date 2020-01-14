# rfx
Vulkan Playground

## Build Instructions (Windows)

### System requirements

- Windows 10
- Visual Studio 2019 Community Edition
- Windows 10 SDK

### External Libraries

Please use the downloadable installer for installation of:

- [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)

Please use [vcpkg](https://github.com/microsoft/vcpkg/releases) for installation of:

- [glm](https://github.com/g-truc/glm)
- [glslang](https://github.com/KhronosGroup/glslang)
- [JsonCpp](https://github.com/open-source-parsers/jsoncpp)
- [Open Asset Import Library (assimp)](https://github.com/assimp/assimp)
- [stb](https://github.com/nothings/stb)

For compressed, mipmapped and cubemap texture files the [.ktx](https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/) format is used.

For loading .ktx files, [libktx](https://github.com/KhronosGroup/KTX-Software) is used and the latest release must be manually [downloaded](https://github.com/KhronosGroup/KTX-Software/releases) and [build](https://github.com/KhronosGroup/KTX-Software/blob/master/BUILDING.md#windows). 

Afterwards, copy the following KTX header files and build artifacts into the following rfx workspace locations:

```
rfx
  /lib
    /ktx
      /include
        ktx.h
      /lib
        /x64
          /debug
            libktx.gl.lib (from debug/x64 build)
          /release
            libktx.gl.lib (from release/x64 build)
```