cmake_minimum_required(VERSION 3.12...3.26)

project(cheap-8 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set search path before finding the package
set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third_party/SDL3-3.2.16")

# Find SDL2
find_package(SDL3 REQUIRED)

add_executable(cheap-8 
    main.cpp
    cheap8.h cheap8.cpp 
    renderer.h renderer.cpp
)

set(CMAKE_FIND_DEBUG_MODE ON)
# Link SDL2 libraries
target_link_libraries(${PROJECT_NAME} 
    ${SDL3_LIBRARIES}
    # ${SDL3_IMAGE_LIBRARIES}
)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SDL3_INCLUDE_DIRS}
    # ${SDL3_IMAGE_INCLUDE_DIRS}
)

# Set runtime directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Copy DLLs to output directory
file(COPY ${SDL3_LIBRARY} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${PROJECT_SOURCE_DIR}/roms DESTINATION ${CMAKE_BINARY_DIR})