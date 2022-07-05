# Building

This test project is built with cmake. It depends on SDL2, glm, and glew.

## Windows

- Download and extract the latest versions of SLD2, glm, and glew
- For each of the library, add a cmake conifg file setting the paths to the libraries and header files (SDL2_INCLUDE_DIRS, SDL2_LIBRARIES, etc.)
- Open folder in Visual Studio
- Add modify variables in CMakeSettings.json (SDL2_DIR, GLM_DIR, ...)
- Before running, ensure SDL2 dll is on the system path

## Linux

`sudo apt install libsdl2.0-dev`

`sudo apt install libglm-dev`

`sudo apt install libglew-dev`

For the glm headers to be recognized you need to copy FindGLM.cmake from https://github.com/Groovounet/glm-deprecated/blob/master/util/FindGLM.cmake to ~/.local/lib/cmake/FindGLM.cmake

`cmake -DCMAKE_BUILD_TYPE="Debug" -Bout`

`cd out`

`make`

# Assets

## Fonts

I generated the font texture using the Codehead’s Bitmap Font Generator tool found at http://www.codehead.co.uk/cbfg/