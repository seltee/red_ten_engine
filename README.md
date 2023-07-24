# Red Ten Engine
### Is
Personal crossplatform 2D/3D game engine I'm creating for development of my indie projects. Currently in alpha state, use on your own risk, existing components are enough to make games

### Features
* Modern deffered rendering allowing hundreds of light sources to be rendered without fps drops. Shadows supported
* Layers system allowing to separate actors, effects, sprites and UI by using different layers and managing them separately. Additional libraries like UI may exist on their own layer without collisions with your 3d objects/sprites and other UI implementations
* Free for commercial and none commercial usage (MIT license)
* Own built-in physics

# Compilation
1. Clone the repo, install make if you don't have it, install clang++
2. Install following libraries: 

**Windows**

* SDL 2 - https://github.com/libsdl-org/SDL/releases/tag/release-2.24.0
* SDL 2 ttf - https://github.com/libsdl-org/SDL_ttf/releases
* OpenAL - https://www.openal.org/downloads/

**MacOS X**

* brew install sdl2 sdl2_ttf openal_soft

3. In the makefile check pathes to this libraries
4. For Windows put nessesary DLL files into bin directory if you would like to run examples
5. Run "make" in root directory

# Usage
### Windows
Link rtengine.lib with your program and put rtengine.dll, SDL2_ttf.dll, SDL2.dll, zlib1.dll, libfreetype-6.dll into your bin folder. You can follow building process of examples
Note, that opanAL executable libraries is usuall presented on the systems, but might be missing. Download installer on the official website https://www.openal.org/downloads/
Without OpenAL audio devices won't be detected
### MacOS
Link rtengine.so with your program, all libraries should already be installed in the system. Use brew if they are not (sdl2 sdl2_ttf openal_soft).

### How to learn
The easiest and currently the only way is to go through the examples. They have comments to explain how to do most of the things the engine capable of and to understand it's architecture

### How engine is being developed
I have a huge backlog of features I want to implement. I select them mostly by my current needs of the projects I'm working on.

