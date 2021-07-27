[![](glTF.png)](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0)

# mixamo to glTF 2.0 merger

mixamo2gltf2 is a command line tool to merge two [mixamo](https://www.mixamo.com/) [glTF 2.0](https://www.khronos.org/gltf/) into one glTF file.  
The animations of the glTF have to be generated from the same mixamo character.  
  
Usage: `mixamo2gltf2.exe folder mainGltf animationGltF`  
  
`folder` Output folder.  
`mainGltf` Main glTF with meshes/skins plus optional animation data.  
`animationGltf` Animation glTF with animation data plus optional meshes/skin data.  

  
## How to generate glTF from mixamo

1. Export the animated mixamo character with the following settings:  
   - Format: `FBX Binary (.fbx)`  
   - Skin: `With Skin` for the main glTF. `Without Skin` for the animation glTF.  
   - Frames per Second: `30` for a good quality.  
   - Keyframe Reduction: `Uniform` otherwise artifacts can occur.  
2. Convert the FBX to glTF e.g. using [Blender](https://www.blender.org/)  
   - Try out [2gltf2](https://github.com/ux3d/2gltf2) for a quick conversion  


## Software Requirements

* C/C++ 17 compiler e.g. gcc or Visual C++
* [Eclipse IDE for C/C++ Developers](https://www.eclipse.org/downloads/packages/release/2021-03/r/eclipse-ide-cc-developers) or  
* [CMake](https://cmake.org/)  


## View, inspect, edit, optimze and compose glTF

To e.g. optimize the merged glTF, please try out [Gestaltor](https://gestaltor.io/).  

