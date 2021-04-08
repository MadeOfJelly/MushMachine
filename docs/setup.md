# Setting up MushMachine

## Game development

First add MM as a git submodule if in git repo:
```bash
git submodule add <url>
git submodule update --init --recursive
```
else just git clone --recursive

add to cmake as subdirectory
```cmake
include("${CMAKE_CURRENT_LIST_DIR}/external/MushMachine/mm_options_and_defines.cmake")
add_subdirectory(MushMachine)
```

The Engine is intended to be linked statically, but you can explore dynamic-linking of course.

## Engine development
* clone repo
* install dependencies

### GNU/LiNUX
* install dependencies
  - libsdl2

* navigate to (out of tree) build directory (prefer just "build" in repo-dir)
* `cmake ..`

### OSX
never tested before. be the first one :) .

### Windows
* install dependencies
  - using install vcpkg
    - install SDL2 (non static)
  - or by hand
    - download libSDL2 and put it somewere, and set the "SDL2" environment variable to that location (set SDL2="path/to/folder")

#### Visual Studio (GUI)
* open cmake project with visual studio (vs2019; vs2017 may work)
  - if using vcpkg and vs2017: set vcpkg cmake toolchain (vs2019 does this automaticly)

#### Visual Studio (MSVC dev cmd)
* open dev cmd (search start menu)
* navigate to (out of tree) build directory (prefer just "build" in repo-dir)
  - if using vcpkg
    - `cmake .. -DCMAKE_TOOLCHAIN_FILE="<path-to-vcpkg-install>\scripts\buildsystems\vcpkg.cmake" -G Ninja`
  - else
    - `cmake .. -G Ninja`

#### CLion
TODO
(look at cmake in this doc)

## Additional Notes

### CMake
* to specify build type on console use `-DCMAKE_BUILD_TYPE=<build-type>` eg. `Release`. the engine cmake defaults to `Debug` if not set.
* the engine has cmake-level defines that can be set (via cmd eg. `-DMM_HEADLESS=ON`):
  - `MM_HEADLESS` (disables sdl2 and opengl usage)
  - `MM_NETWORKING` (depricated)
  - `MM_OPENGL_3` (disabled if `MM_HEADLESS`)
  - `MM_OPENGL_3_GLES` (disabled if `MM_HEADLESS`, `MM_OPENGL_3` supplement, makes the openglrenderer use gles3 instead of gl3)

### Emscripten
* version 2.0.1 works
* version 1.39.18 does not (webgl errors)

