from conan import ConanFile
from conan.tools.cmake import CMake

class KiloRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps", "VirtualRunEnv"

    def layout(self):
        self.folders.generators = "conan"

    def requirements(self):
        self.requires("ms-gsl/4.0.0")
        self.requires("fmt/11.0.1")

    def build_requirements(self):
        self.test_requires("gtest/1.14.0")
