cmake_minimum_required(VERSION 3.22)

set(OUTPUT_NAME user)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fno-signed-zeros")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall")
        
set(SRC
    "main.cpp"
	"rand.cpp"
)

# launch as windows, not console app - so cmd does not open as well
add_link_options(-mwindows)

include_directories(include)
include_directories(.)

find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

add_executable(${OUTPUT_NAME} ${SRC})

target_link_libraries(
	${OUTPUT_NAME}
	${CMAKE_SOURCE_DIR}/libjGL.a
	${X11_LIBRARIES}
	${OPENGL_LIBRARIES} 
    ${Vulkan_LIBRARIES}
	"winmm"
)

target_include_directories(
	${OUTPUT_NAME}
	PUBLIC
	${Vulkan_INCLUDE_DIR} 
)

target_compile_definitions(${OUTPUT_NAME} PUBLIC GLSL_VERSION="330")
target_compile_definitions(${OUTPUT_NAME} PUBLIC MAX_SPRITE_BATCH_BOUND_TEXTURES=4)
