from conans import ConanFile, CMake


class VolumeRendering(ConanFile):
    name = "VolumeRendering"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    requires = (
        "glm/0.9.9.8",
        "glad/0.1.36"
    )
    generators = "cmake"
