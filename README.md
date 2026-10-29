# Kilo

Kilo is a C++ port of [antirez's kilo](http://antirez.com/news/108), a small terminal text editor.

## Requirements

- A C++23-capable compiler
- [CMake](https://cmake.org/) 3.20 or newer
- [Conan](https://conan.io/) 2
- Ninja when using the development CMake presets

The project dependencies are declared in `conanfile.py`. They include
Microsoft GSL and fmt; GoogleTest is installed for the test suite.

## Install dependencies

Install the dependencies before configuring CMake. From the repository root,
run:

```sh
conan profile detect
conan install . --build=missing
```

Conan generates the CMake toolchain and dependency files under `conan/`, as
referenced by the CMake presets. `--build=missing` asks Conan to build any
dependency binaries that are not already available for your profile.

## Build with a CMake preset

There are shared CI presets in `CMakePresets.json` and local development
presets in `CMakeUserPresets.json`. The user-preset file is ignored by Git, so
it may not be present in a fresh checkout. When available, the `dev` preset is
the convenient Linux development workflow:

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

This preset uses Ninja Multi-Config, builds Debug into `build/dev`, and enables
`clang-tidy`, `cppcheck`, and the documentation configuration. Install Ninja,
`ccache`, `clang-tidy`, and `cppcheck`; Python is also required for the
documentation configuration. CMake downloads m.css when configuring.

The shared presets in `CMakePresets.json` provide platform CI builds. On Linux,
`ci-ubuntu` configures a Release build in `build/`; it also enables
`clang-tidy` and `cppcheck`, which must be installed and available on `PATH`:

```sh
cmake --preset ci-ubuntu
cmake --build build --parallel
ctest --test-dir build --output-on-failure --no-tests=error
```

The `ci-macos` and `ci-windows` presets use Xcode and Visual Studio 2022,
respectively. These multi-configuration generators require specifying the
configuration when building and testing:

```sh
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure --no-tests=error
```

See which presets are available in your checkout with:

```sh
cmake --list-presets
```

## Build without a CMake preset

After installing dependencies, configure CMake directly. This example uses a
Debug build and keeps generated build files in `build/manual`:

```sh
cmake -S . -B build/manual \
  -DCMAKE_TOOLCHAIN_FILE="$PWD/conan/conan_toolchain.cmake" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON
cmake --build build/manual --parallel
ctest --test-dir build/manual --output-on-failure --no-tests=error
```

If you change the Conan profile or dependency requirements, run `conan install
. --build=missing` again before reconfiguring CMake.

## Run

On Linux, the executable is `build/manual/src/kilo` for the non-preset example,
or `build/src/kilo` for the `ci-ubuntu` preset:

```sh
build/manual/src/kilo [path/to/file]
```
