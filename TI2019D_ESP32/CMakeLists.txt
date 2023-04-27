# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS $ENV{RMAKER_PATH}/components/esp-insights/components $ENV{RMAKER_PATH}/components $ENV{RMAKER_PATH}/examples/common)#
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(TI2019D_ESP32)
