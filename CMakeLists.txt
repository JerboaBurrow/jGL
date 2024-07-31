cmake_minimum_required(VERSION 3.1)
project(Jerboa)

list(APPEND CMAKE_CXX_SOURCE_FILE_EXTENSIONS shader)

option(TEST_SUITE "test" OFF)
option(RELEASE On)

set(CMAKE_CXX_STANDARD 17)
if (RELEASE)
    if (ANDROID)
        message("ANDROID MinSizeRel!")
        # GOOGLE!! (Release is not a thing pos ndk 23 apparently..)
        add_compile_definitions(BUILD_TYPE="MinSizeRel")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signed-zeros")
    else()
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fno-signed-zeros")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall")
    endif()
else()
    if (ANDROID)
        # GOOGLE!!
        add_compile_definitions(BUILD_TYPE="RelWithDebInfo")
    else()
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g -Wall")
    endif()
endif()

if (SANITISE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(CMAKE_BUILD_RPATH ".")

find_package(Vulkan REQUIRED)

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
add_subdirectory(include/vendored/freetype)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")

if (ANDROID)
    find_library(GLES-lib GLESv3 REQUIRED)
else()
    find_package(OpenGL REQUIRED)
endif()

add_subdirectory(include/vendored/glm)
add_subdirectory(include/vendored/stduuid)

if (NOT ANDROID)

    add_subdirectory(include/vendored/glfw)
    # let sources know glew is static
    add_compile_definitions(GLEW_STATIC)
    set(GLEW_SOURCE include/vendored/glew/src/glew.c)
    set(GLEW_INCLUDE include/vendored/glew/include)

    add_library(glew STATIC ${GLEW_SOURCE} ${GLEW_INCLUDE})
    target_include_directories(glew PUBLIC ${GLEW_INCLUDE} ${OPENGL_INCLUDE_DIRS})

    target_link_libraries(glew
        ${OPENGL_LIBRARY}
    )

endif()

if (ANDROID)
    file(GLOB_RECURSE jGL_SRC
        "src/jGL/OpenGL/*.cpp"
        "src/font.cpp"
        "src/glyph.cpp"
        "src/shader.cpp"
        "src/shapeRenderer.cpp"
        "src/spriteRenderer.cpp"
        "src/warning.cpp"
        "src/jGL/common.cpp"
        "src/id.cpp"
    )
else()
    file(GLOB_RECURSE jGL_SRC
        "src/jGL/*.cpp"
        "src/*.cpp"
    )
endif()

if (VALIDATION)
    add_compile_definitions(VALIDATION)
    if (VERBOSE)
        add_compile_definitions(VERBOSE)
    endif()
endif()

if (NO_WARN)
    add_compile_definitions(NO_WARN)
endif()

add_library(jGL STATIC ${jGL_SRC})

target_include_directories(jGL PUBLIC 
    include 
    include/vendored 
    ${OPENGL_INCLUDE_DIRS} 
    ${Vulkan_INCLUDE_DIR} 
    ${GLEW_INCLUDE}
    include/vendored/glew/include/GL
    include/vendored/freetype/include 
    include/vendored/VulkanSDK/Include
    include/vendored/VulkanSDK/Include/shaderc 
    include/vendored/VulkanSDK/Include/vulkan
    include/vendored/glfw/include
    include/jLog
    include/jThread
)

if (WINDOWS)
    add_compile_definitions(WINDOWS)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    target_link_directories(jGL PUBLIC include/vendored/VulkanSDK/Windows/Lib)
elseif(OSX)
    add_compile_definitions(MACOS)
    target_link_directories(jGL PUBLIC include/vendored/VulkanSDK/MacOS/Lib)
elseif(ANDROID)
    
else()
    target_link_directories(jGL PUBLIC include/vendored/VulkanSDK/Linux/Lib)
endif()

target_compile_definitions(jGL PUBLIC MAX_SPRITE_BATCH_BOUND_TEXTURES=4)

if (ANDROID)
    target_compile_definitions(jGL PUBLIC GLSL_VERSION="300 es")
    target_link_libraries(jGL PUBLIC ${GLES-lib} ${Vulkan_LIBRARIES} shaderc_combined stduuid freetype glm)
elseif(WINDOWS)
    target_compile_definitions(jGL PUBLIC GLSL_VERSION="330")
    target_link_libraries(jGL PUBLIC stduuid glew freetype glm ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${Vulkan_LIBRARIES} glfw shaderc_combined "winmm")
else()
    target_compile_definitions(jGL PUBLIC GLSL_VERSION="330")
    target_link_libraries(jGL PUBLIC stduuid glew freetype glm ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${Vulkan_LIBRARIES} glfw shaderc_combined)
endif()

If (EXAMPLES)
    add_subdirectory(examples)
endif()

IF (TEST_SUITE)

    file(COPY "tests/resource" DESTINATION "${CMAKE_BINARY_DIR}/")

    IF (WINDOWS)
        ## ask nicely for big objs (gnu assembler flag)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
    endif()

    file(GLOB_RECURSE TEST_SRC 
        "tests/glTests/*.cpp"
        "src/jGL/OpenGL/*.cpp"
        "src/jGL/shader.cpp"
        "src/jGL/warning.cpp"
        "src/jGL/spriteRenderer.cpp"
        "src/jGL/shapeRenderer.cpp"
        "src/id.cpp"    
        "src/jGL/Display/*.cpp"
        "src/jGL/common.cpp"
    )
    include_directories(include "tests/")

    # GL Enabled
    add_executable(glTests 
        ${TEST_SRC}
        "src/jGL/shader.cpp"
        "src/id.cpp"
        "src/jGL/font.cpp"
        "src/jGL/glyph.cpp"
    )

    target_include_directories(glTests PUBLIC 
        include 
        include/vendored 
        ${OPENGL_INCLUDE_DIRS} 
        ${GLEW_INCLUDE}
        include/vendored/glew/include/GL
        include/vendored/freetype/include 
        include/vendored/glfw/include
        include/vendored/VulkanSDK/Include
        include/jLog
        include/jThread
    )

    target_compile_definitions(glTests PUBLIC GLSL_VERSION="330")
    target_compile_definitions(glTests PUBLIC MAX_SPRITE_BATCH_BOUND_TEXTURES=4)

    if (WINDOWS)
        target_link_libraries(glTests stduuid glew freetype glm ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} glfw "winmm")
    else()
        target_link_libraries(glTests stduuid glew freetype glm ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} glfw)
    endif()
    
    include(CTest)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/)
    include(Catch)
    catch_discover_tests(glTests)

    # vulkan tests

    add_compile_definitions(EXCEPT_ON_VALIDATION)

    file(GLOB_RECURSE TEST_SRC 
        "tests/vulkanTests/*.cpp"
        "src/jGL/Vulkan/*.cpp"
        "src/jGL/Display/*.cpp"
        "src/jGL/common.cpp"
    )

    add_executable(vkTests
        ${TEST_SRC}
        "src/jGL/shader.cpp"
        "src/jGL/warning.cpp"
        "src/jGL/spriteRenderer.cpp"
        "src/jGL/shapeRenderer.cpp"
        "src/id.cpp"
        "src/jGL/font.cpp"
        "src/jGL/glyph.cpp"
    )

    target_include_directories(vkTests PUBLIC 
        include 
        include/vendored 
        ${Vulkan_INCLUDE_DIR} 
        tests
        include/vendored/freetype/include 
        include/vendored/VulkanSDK/Include
        include/jLog
        include/jThread
    )

    if (WINDOWS)
        add_compile_definitions(WINDOWS)
        set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
        target_link_directories(vkTests PUBLIC include/vendored/VulkanSDK/Windows/Lib)
    elseif(OSX)
        add_compile_definitions(MACOS)
        target_link_directories(vkTests PUBLIC include/vendored/VulkanSDK/MacOS/Lib)
    elseif(ANDROID)
        
    else()
        target_link_directories(vkTests PUBLIC include/vendored/VulkanSDK/Linux/Lib)
    endif()
    
    if (WINDOWS)
        target_link_libraries(vkTests stduuid freetype glm ${Vulkan_LIBRARIES} glfw shaderc_combined "winmm")
    else()
        target_link_libraries(vkTests stduuid freetype glm ${Vulkan_LIBRARIES} glfw shaderc_combined)
    endif()

    target_compile_definitions(vkTests PUBLIC GLSL_VERSION="330")
    target_compile_definitions(vkTests PUBLIC MAX_SPRITE_BATCH_BOUND_TEXTURES=4)
    include(CTest)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/)
    include(Catch)
    catch_discover_tests(vkTests)
  
ENDIF(TEST_SUITE)