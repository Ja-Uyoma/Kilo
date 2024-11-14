from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake

class KiloRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("ms-gsl/4.0.0")
        self.requires("fmt/11.0.1")

    def build_requirements(self):
        self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
