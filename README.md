# OpenGL/GLSL Volume Rendering

![Volumetric Rendering](screenshot.jpg)

Volume rendering experiments in OpenGL/GLSL for Windows.


Branch is **WIP**.

# Dependencies

Project uses *conan* to install:
* GLM: https://github.com/g-truc/glm
* GLAD: https://github.com/Dav1dde/glad


# Build

Build with *conan* and *cmake*:

```
mkdir build
conan install . -if build --build=missing
cmake -Bbuild
cmake --build build --config Release --target INSTALL
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
