set(OUTPUT_NAME Particles)

file(GLOB_RECURSE SRC
    "*.cpp"
)

if (WINDOWS)
    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
elseif(OSX)
else()
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
endif()

include_directories(.)

add_executable(${OUTPUT_NAME} ${SRC})

target_link_libraries(${OUTPUT_NAME} jGL)

set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Examples/${OUTPUT_NAME}")

file(COPY resource DESTINATION "${CMAKE_BINARY_DIR}/Examples/${OUTPUT_NAME}/")