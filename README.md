# Running

The program does not take any commandline arguments and must be run from the root project directroy. 
For example, when building on Linux to the out directory the run command is `./out/A5`.

# User Interface

## Camera Controls
- Horizontal Mouse Motion: move the view direction left/right
- Vertical Mouse Motion: tilt the view direction up/down
- q: move the camera down
- e: move the camera up
- a: move the camera left
- d: move the camera right
- w: move the camera forward
- s: move the camera backward

## Picking
- left-click: select an object

## Guide Controls
- g: open/close the clue guide
- left-arrow: flip to the previous clue
- right-arrow: flip to the next clue

## Flashlight Controls
- f: toggle flashlight
- arrow-keys: change the flashlight direction

## Binocular Controls
- b: toggle binoculars
- h: decrease the focus distance by 1m
- j: decrease the focus distance by 0.1m
- k: increase the focus distance by 0.1m
- l: increase the focus distance by 1m

## Time Controls
- u: slow down time by a factor of 2
- i: go back in time 1 hour
- o: go forward in time 1 hour
- p: speed up time by a factor of 2

## Feature Toggles
- 1: toggle reflections
- 2: toggle shadow
- 3: toggle bumpmapping


# Building

This project is built with cmake. It depends on SDL2, glm, and glew.

## Windows

- Download and extract the latest versions of SLD2, glm, and glew
- For each of the libraries, add a cmake conifg file setting the paths to the libraries and header files (SDL2_INCLUDE_DIRS, SDL2_LIBRARIES, etc.)
- Open the project folder in Visual Studio
- Add ad or modify the variables in CMakeSettings.json (SDL2_DIR, GLM_DIR, and GLEW_DIR) to point to where the libraries are installed
- Before running, ensure SDL2.dll is on the system path

## Linux (Ubuntu)

Install the dependencies:
- `sudo apt install libsdl2.0-dev`
- `sudo apt install libglm-dev`
- `sudo apt install libglew-dev`

For the glm headers to be recognized you need to copy FindGLM.cmake from https://github.com/Groovounet/glm-deprecated/blob/master/util/FindGLM.cmake to ~/.local/lib/cmake/FindGLM.cmake

Configure cmake and build
- `cmake -DCMAKE_BUILD_TYPE="Debug" -Bout`
- `make -C out`

# Assets

I created all the assets that I used myself.

## Textures

I generated the font texture using the Codehead’s Bitmap Font Generator tool found at http://www.codehead.co.uk/cbfg/. 
I generated the other textures using a combination of GIMP and Clip Studio Paint.

## Sounds

I created the fail and pass sounds in LMMS and recorded the collision sound with Audacity.

# Models

I generated the .obj and .mtl files using Blender. For the models that did not come from .obj files (the tombstone and tree) I generated the vertex buffer data in code.
