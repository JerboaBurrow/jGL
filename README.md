## jGL 

### cross-platform graphics, abtracting OpenGL and Vulkan for 2D graphics/gamedev

This library aims to provide a simple API for 2D graphics applications in C++, targeting multiple platforms: linux, windows, macOS, and Android (and maybe IOS).

Versions 0.0.X will progressively define an API.

Currently OpenGL is the main vehicle for functionality, the Vulkan backend is a shell. Once the API is reasonably solid the Vulkan side will be brought up to date for 0.1.0.

IOS depends on how easy Vulkan is to integrate with an IOS app, we may opt for https://developer.apple.com/metal/cpp/ 

### Documentation

[Doxygen docs are hosted here, in step with main](https://jerboaburrow.github.io/jGL/)

### Cross platform status
linux/macOS/windows: [![jGL release](https://github.com/JerboaBurrow/jGL/actions/workflows/release.yml/badge.svg)](https://github.com/JerboaBurrow/jGL/actions/workflows/release.yml)

### Features

- [ ] Window managent and I/O
    - [x] Single window creation
    - [x] Mouse and keyboard events
    - [x] Frame locking
    - [ ] Snapping/resizing
    - [ ] Multi-windowing
    - [ ] Basic ui (dearimgui?)   
- [ ] Sprites
    - [x] Texture atlasing/texture region support.
    - [x] Texture management
    - [ ] Animations
- [ ] Particles
    - [x] Textured/untextured
    - [ ] Dynamics API  
- [ ] Text
    - [x] FreeType font rendering
    - [ ] Automatic alignment/wrapping     
- [ ] Shapes
    - [ ] Circles
    - [ ] Polygon
    - [ ] Lines 
