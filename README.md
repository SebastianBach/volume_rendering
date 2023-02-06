# OpenGL GLSL Volume Rendering

Volume rendering experiments in GLSL


Branch is **WIP**.

# Dependencies

Uses *conan* to install:
* GLM: https://github.com/g-truc/glm
* GLAD: https://github.com/Dav1dde/glad


# Build

```
mkdir build
conan install . -if build --build=missing
cd build
cmake .. 
cmake --build .
```

# Usage

## Keys

* ```Esc```: close application
* ```Space```: pause/start animation
* ```Backspace``` or ```D```: remove object
* ```A```: add object
* ```N```: turn noise on/off
* ```0``` to ```9```: different rendering/shading modes
