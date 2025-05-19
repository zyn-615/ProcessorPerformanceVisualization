cmake_minimum_required(VERSION 3.16)
project(ProcessorPerformanceVisualization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's moc, uic, rcc tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages, and specify needed components (e.g. Core, Gui, Widgets)
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Debug: Print Qt6 information
if(Qt6_FOUND)
    message(STATUS "Successfully found Qt6 version ${Qt6_VERSION}")
    # The INTERFACE_INCLUDE_DIRECTORIES for Qt6::Core etc. are what get propagated.
    # We can also check the CMAKE_PREFIX_PATH that might be used.
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
    message(STATUS "Qt6_DIR: ${Qt6_DIR}") # Should point to the Qt6Config.cmake directory

    # For modern CMake, include directories are usually handled by target_link_libraries
    # with imported targets like Qt6::Core. Let's check if these targets are found.
    if(TARGET Qt6::Core)
        message(STATUS "Target Qt6::Core found.")
    else()
        message(WARNING "Target Qt6::Core NOT found.")
    endif()
    if(TARGET Qt6::Widgets)
        message(STATUS "Target Qt6::Widgets found.")
    else()
        message(WARNING "Target Qt6::Widgets NOT found.")
    endif()
else()
    message(FATAL_ERROR "Qt6 was NOT found by find_package despite REQUIRED. This should not happen if previous configuration was successful.")
endif()

# Add your source files
add_executable(ProcessorPerformanceVisualization # Should match the project name or be your desired executable name
    main/main.cpp
    # If you add more .cpp files, list them here
)

# Link Qt6 libraries
target_link_libraries(ProcessorPerformanceVisualization PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
) 