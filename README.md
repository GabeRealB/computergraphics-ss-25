# Computer Graphics

## Developing

The project comes preconfigured with the following libraries:

- GLFW
- GLM
- ImGui
- stb_image

To implement your own functionality you may add files to the `src` directory, and modify the `src/App.hpp` file.

To ease the implementation, we provide the following wrappers:

- `src/Image.hpp`: Image loading and creation.

## Resources

All resources, like shaders, imaged and models must be placed in the `resources` directory, to be loadable from the application.

## Dependencies

- C++ 17 compiler (Visual Studio/Clang/GCC)
- CMake
  
### Linux Dependencies

Linux operating also require the following packages.

#### Ubuntu/Debian

- libwayland-dev
- libxkbcommon-dev
- xorg-dev

```bash
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev
```

#### Fedora/Red Hat

- wayland-devel
- libxkbcommon-devel
- libXcursor-devel
- libXi-devel
- libXinerama-devel
- libXrandr-devel

```bash
sudo dnf install wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel
```

## Build

1. Create a `build` directory, where the application will be built.

```sh
mkdir build
cd build
```

2. Configure the project using `CMake`.

```sh
cmake ..
```

3. Build the project.

```sh
cmake --build .
```

You may be required to reexecute step `2` each time you add another file to the `src` directory.