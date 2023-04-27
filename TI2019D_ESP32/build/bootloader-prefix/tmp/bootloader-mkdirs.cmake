# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Hardware/Espressif/frameworks/esp-idf/components/bootloader/subproject"
  "D:/MyWorks/Programs/Github/TI2019D/TI2019D/build/bootloader"
  "D:/MyWorks/Programs/Github/TI2019D/TI2019D/build/bootloader-prefix"
  "D:/MyWorks/Programs/Github/TI2019D/TI2019D/build/bootloader-prefix/tmp"
  "D:/MyWorks/Programs/Github/TI2019D/TI2019D/build/bootloader-prefix/src/bootloader-stamp"
  "D:/MyWorks/Programs/Github/TI2019D/TI2019D/build/bootloader-prefix/src"
  "D:/MyWorks/Programs/Github/TI2019D/TI2019D/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/MyWorks/Programs/Github/TI2019D/TI2019D/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/MyWorks/Programs/Github/TI2019D/TI2019D/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
