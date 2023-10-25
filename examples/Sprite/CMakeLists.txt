file(GLOB_RECURSE SRC
    "*.cpp"
)

if (WINDOWS)
    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
else()
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
endif()

include_directories(.)

add_executable(Sprite ${SRC})

target_link_libraries(Sprite jGL)

set_target_properties(Sprite PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Examples/Sprite")

file(COPY resource DESTINATION "${CMAKE_BINARY_DIR}/Examples/Sprite/")