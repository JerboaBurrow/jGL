## jGL 

### cross-platform graphics, abtracting OpenGL and Vulkan for 2D graphics/gamedev

This library aims to provide a simple api for graphics applications in C++, targeting multiple platforms: linux, windows, macOS, and Android 

### Cross platform status
linux/macOS/windows: [![jGL release](https://github.com/JerboaBurrow/jGL/actions/workflows/release.yml/badge.svg)](https://github.com/JerboaBurrow/jGL/actions/workflows/release.yml)

The [release action](https://github.com/JerboaBurrow/jGL/actions/workflows/release.yml) builds the lib's for each target platform, including verifying it works as intended in a user project.

- Linux x86_64
    - native build
    - tested in ubuntu 22.04 LTS
       
- Windows
  - cross-compiled from linux, via mingw-w64-x86_64
  - tested with wine32/64

- macOS
  - native build
  - tested with MacBook Pro (a1990 emc 3215), macOS 14.0
 
- Android
  - ndk cross-compile from linux
  - currently untested
