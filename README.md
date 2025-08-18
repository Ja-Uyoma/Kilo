# Kilo

This is a C++ port of
[Salvatore Sanfilippo's (antirez) kilo](http://antirez.com/news/108).

## Description

This project adapts the minimalistic kilo text editor into C++, leverating
modern tooling, configuration, and build systems like CMake and Conan for easier
maintainability and extensibility.

## Getting Started

### Prerequisites

1. C++20 (or newer) compatible compiler
2. CMake (v3.15+ recommended)
3. Conan (v2.0+ for dependency management)

### Set-up Conan

Before building, make sure you have a Conan profile.

```sh
# Detect and create a default profile (overwrites if it already exists)
conan profile detect --force
```

This generates a profile (usually saved as `~/.conan2/profiles/default`) with
settings for your current compiler and system.

To inspect or tweak it:

```sh
conan profile path default
cat $(conan profile path default)
```

Since our project is using C++20, we need to make sure that this is the default
version of C++ that Conan uses as well. For your own convenience, the following
is the profile that I used to build the project:

```ini
[settings]
arch=x86_64
build_type=Release
compiler=gcc
compiler.cppstd=20
compiler.libcxx=libstdc++11
compiler.version=13
os=Linux
```

## Build and install

```sh
# Clone the repository
git clone https://github.com/Ja-Uyoma/Kilo.git
cd Kilo

# Install dependencies and build the project with Conan
conan build conanfile.py --build=missing --profile=default
```

## Usage

Run the editor with:

```sh
build/src/Release/kilo <filename>
```

## License

This project is licensed under the MIT License. See LICENSE for more
information.

## Acknowledgements

- The original `kilo` editor by Salvatore Sanfilippo (antirez)
- Inspired by its simplicity and elegance
