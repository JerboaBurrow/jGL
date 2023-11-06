cmake_minimum_required(VERSION 3.1)
project(Jerboa)

list(APPEND CMAKE_CXX_SOURCE_FILE_EXTENSIONS shader)

option(TEST_SUITE "test" OFF)
option(RELEASE On)

set(CMAKE_CXX_STANDARD 17)
if (RELEASE)
    set(CMAKE_BUILD_TYPE Release)
else()
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (BENCHMARK)
    add_compile_definitions(BUILD_TYPE="RelWithDebInfo")
elseif (RELEASE)
    add_compile_definitions(BUILD_TYPE="Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fno-signed-zeros")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall")
else ()
    add_compile_definitions(BUILD_TYPE="Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g -Wall")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_BUILD_RPATH ".")

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})
include_directories(include include/vendored/VulkanSDK/Include)

add_subdirectory(include/vendored/freetype)
include_directories(include/vendored/freetype/include)

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
add_subdirectory(include/vendored/glm)

add_subdirectory(include/vendored/stduuid)

add_subdirectory(include/vendored/glfw)
include_directories(include/vendored/glfw/include)

find_package(OpenGL REQUIRED)
# let sources know glew is static
add_compile_definitions(GLEW_STATIC)
set(GLEW_SOURCE include/vendored/glew/src/glew.c)
set(GLEW_INCLUDE include/vendored/glew/include/)

add_library(glew STATIC ${GLEW_SOURCE} ${GLEW_INCLUDE})

target_link_libraries(glew
    ${OPENGL_LIBRARY}
)

include_directories(include include/vendored ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE})

file(GLOB_RECURSE jGL_SRC
    "src/jGL/*.cpp"
    "src/*.cpp"
    "src/Util/*.cpp"
)

if (WINDOWS)
    add_compile_definitions(WINDOWS)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    link_directories(include/vendored/VulkanSDK/Windows/Lib)
elseif(OSX)
    add_compile_definitions(MACOS)
    link_directories(include/vendored/VulkanSDK/MacOS/Lib)
else()
    link_directories(include/vendored/VulkanSDK/Linux/Lib)
endif()

if (VALIDATION)
    add_compile_definitions(VALIDATION)
    if (VERBOSE)
        add_compile_definitions(VERBOSE)
    endif()
endif()

add_library(jGL STATIC ${jGL_SRC})

target_compile_definitions(jGL PUBLIC GLSL_VERSION="330")
target_link_libraries(jGL stduuid glew freetype glm ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${Vulkan_LIBRARIES} glfw shaderc_combined)

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
        "src/id.cpp"    
    )
    include_directories(include "tests/")

    # GL Enabled
    add_executable(glTests 
        ${TEST_SRC}
        "src/log.cpp"
        "src/jGL/Display/display.cpp"
        "src/jGL/shader.cpp"
        "src/id.cpp"
    )

    target_compile_definitions(glTests PUBLIC GLSL_VERSION="330")

    target_link_libraries(glTests stduuid glew freetype glm ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} glfw)
    
    include(CTest)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/)
    include(Catch)
    catch_discover_tests(glTests)

    # vulkan tests

    add_compile_definitions(EXCEPT_ON_VALIDATION)

    file(GLOB_RECURSE TEST_SRC 
        "tests/vulkanTests/*.cpp"
        "src/jGL/Vulkan/*.cpp"
    )
    include_directories(include "tests/" ${Vulkan_INCLUDE_DIR})

    add_executable(vkTests
        ${TEST_SRC}
        "src/jGL/Display/display.cpp"
        "src/log.cpp"
        "src/jGL/shader.cpp"
        "src/jGL/warning.cpp"
        "src/jGL/spriteRenderer.cpp"
        "src/id.cpp"
    )
    

    target_link_libraries(vkTests stduuid freetype glm ${Vulkan_LIBRARIES} glfw shaderc_combined)
    target_compile_definitions(vkTests PUBLIC GLSL_VERSION="330")
    include(CTest)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/)
    include(Catch)
    catch_discover_tests(vkTests)

    # non gl units

    file(GLOB TEST_SRC "tests/unitTests/*.cpp")
    include_directories(include "tests/")

    add_executable(unitTests 
        ${TEST_SRC}
        "src/Util/util.cpp"
    )

    target_compile_definitions(unitTests PUBLIC GLSL_VERSION="330")
    
    include(CTest)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/)
    include(Catch)
    catch_discover_tests(unitTests)
  
ENDIF(TEST_SUITE)