# OpenGL/GLSL Volume Rendering

![Volumetric Rendering](screenshot.jpg)

Ray-casting volume rendering experiments in OpenGL/GLSL for Windows.


# Dependencies

Project uses *conan* to install:
* GLM: https://github.com/g-truc/glm
* GLAD: https://github.com/Dav1dde/glad
* GoogleTest: https://github.com/google/googletest


# Build

To test and build, run *conan* and *cmake*:

```
mkdir build
cd build
conan install .. -s build_type=Release -s compiler.runtime=MD
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
ctest -C Release  -VV
cmake --install .
```

The executable with shaders can be found in ```build/product```.

# Usage

Hotkeys:

* ```Esc```: close application
* ```Space Bar```: pause/start animation
* ```Backspace``` or ```D```: remove object
* ```A```: add object
* ```N```: toggle procedural noise deformation on/off
* ```0``` to ```9```: different rendering/shading modes

The rendering modes are:

* 1: object normals
* 2: Lambertian reflectance
* 3: Phong reflection
* 4: Fresnel effect
* 5: Hard shadows
* 6: Volumetric light effect
* 7: object colors
* 8: reserved
* 9: "blood" effect combining the above effects
* 0: default rendering combining the above effects
