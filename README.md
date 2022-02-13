# rfx
Vulkan Playground

## Build Instructions

### System requirements

You will need a compiler that supports C++ 20.

### External Libraries

Please use the downloadable installer for installation of:

- [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)

Please use [vcpkg](https://github.com/microsoft/vcpkg/releases) for installation of:

- [GLFW](https://www.glfw.org/)
- [glm](https://github.com/g-truc/glm)
- [glslang](https://github.com/KhronosGroup/glslang)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [nlohmann-json](https://github.com/nlohmann/json)
- [stb](https://github.com/nothings/stb)
- [TinyGLTF](https://github.com/syoyo/tinygltf)
- [fmt](https://github.com/fmtlib/fmt)
- [gli](https://github.com/g-truc/gli)

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
        ktx.lib (or libktx.gl.lib) 
```