# *Pistacchio*

A C++ framework for making real-time applications.

## Dependencies

- GLM
- SDL2
- **(optional)** FreeType
- **(optional)** OpenGL

## Building

Only Clang is currently supported.

```pwsh
mkdir build
cmake -B build
      -S .
      -DCMAKE_TOOLCHAIN_FILE=<vcpkg-path>/scripts/buildsystems/vcpkg.cmake
      -DPISTACCHIO_ENABLE_FREETYPE=ON    # Enable FreeType support
      -DPISTACCHIO_ENABLE_OPENGL=ON      # Enable OpenGL support
      -DPISTACCHIO_BUILD_EXAMPLES=OFF    # Build examples
cmake --build build
```

## License

See [LICENSE](/LICENSE). For files under the `thirdparty/` directories check
with the original license holders.

- [Dear ImGui](https://github.com/ocornut/imgui)
- [glad](https://github.com/Dav1dde/glad)
- [stb](https://github.com/nothings/stb)
