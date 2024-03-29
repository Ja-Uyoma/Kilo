from conan import ConanFile
from conan.tools.cmake import cmake_layout

class KiloRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("catch2/3.5.2")
        self.requires("ms-gsl/4.0.0")

    def layout(self):
        cmake_layout(self)
