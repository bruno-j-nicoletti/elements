cmake_minimum_required(VERSION 3.16.0)
project(DynamicLists LANGUAGES C CXX VERSION "1.0.0")

if (NOT ELEMENTS_ROOT)
   message(FATAL_ERROR "ELEMENTS_ROOT is not set")
endif()

# Make sure ELEMENTS_ROOT is an absolute path to add to the CMake module path
get_filename_component(ELEMENTS_ROOT "${ELEMENTS_ROOT}" ABSOLUTE)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${ELEMENTS_ROOT}/cmake")

# If we are building outside the project, you need to set ELEMENTS_ROOT:
if (NOT ELEMENTS_BUILD_EXAMPLES)
   include(ElementsConfigCommon)
   set(ELEMENTS_BUILD_EXAMPLES OFF)
   add_subdirectory(${ELEMENTS_ROOT} elements)
endif()

set(ELEMENTS_APP_PROJECT "StatusBars")
set(ELEMENTS_APP_TITLE "Status Bars")
set(ELEMENTS_APP_COPYRIGHT "Copyright (c) 2024 Flole")
set(ELEMENTS_APP_ID "com.cycfi.status_bars")
set(ELEMENTS_APP_VERSION "1.0")

set(ELEMENTS_APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# For your custom application icon on macOS or Windows see cmake/AppIcon.cmake module
include(AppIcon)
include(ElementsConfigApp)
