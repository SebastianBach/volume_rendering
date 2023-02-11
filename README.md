# OpenGL/GLSL Volume Rendering

![Volumetric Rendering](screenshot.jpg)

Volume rendering experiments in OpenGL/GLSL for Windows.


Branch is **WIP**.

# Dependencies

Project uses *conan* to install:
* GLM: https://github.com/g-truc/glm
* GLAD: https://github.com/Dav1dde/glad
* GoogleTest: https://github.com/google/googletest


# Build

To test and build, run *conan* and *cmake*:

```
mkdir build
conan install . -if build -s build_type=Release -s compiler.runtime=MD
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target volume_lib
cmake --build . --config Release --target unit_tests
ctest -C Release  -VV
cmake --build . --config Release --target INSTALL
```

The executable with shaders can be found in ```build/product```.

# Usage

## Keys

* ```Esc```: close application
* ```Space```: pause/start animation
* ```Backspace``` or ```D```: remove object
* ```A```: add object
* ```N```: turn noise on/off
* ```0``` to ```9```: different rendering/shading modes
