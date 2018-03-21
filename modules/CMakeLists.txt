project(modules)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# default build type is "Release"
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type is \"Release\"")
endif ()
# where to install executables, libraries and headers (relative path to ./build or absolute)
# default install prefix is "/usr/local"
if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local CACHE FILEPATH "Default install prefix is \"/usr/local\"")
endif()
# where to install module libraries (relative path to INSTALL_PREFIX or absolute)
if (NOT VTAPI_MODULES_DIR)
  set(VTAPI_MODULES_DIR lib CACHE FILEPATH "Default module libraries path is \"${CMAKE_INSTALL_PREFIX}/lib\"")
endif ()

#pkg-config
set(PKG_CONFIG "pkg-config" CACHE FILEPATH "Path of pkg-config executable")
execute_process(COMMAND ${PKG_CONFIG} RESULT_VARIABLE PKG_CONFIG_ERR OUTPUT_QUIET ERROR_QUIET)
if (NOT (PKG_CONFIG_ERR EQUAL 1))
  message(FATAL_ERROR "Executable \"pkg_config\" could not be found. Please, install it.")
endif ()

# take supplied OpenCV or find default one
if (OPENCV_PC_PATH)
  if(EXISTS ${OPENCV_PC_PATH})
    execute_process(COMMAND ${PKG_CONFIG} ${OPENCV_PC_PATH} --modversion
      OUTPUT_VARIABLE OPENCV_VERSION # TODO: minimal version check
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND ${PKG_CONFIG} ${OPENCV_PC_PATH} --variable=includedir_new
      OUTPUT_VARIABLE OPENCV_INCLUDE_PATH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND ${PKG_CONFIG} ${OPENCV_PC_PATH} --libs
      OUTPUT_VARIABLE OPENCV_LIBS
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND ${PKG_CONFIG} ${OPENCV_PC_PATH} --variable=libdir
      OUTPUT_VARIABLE OPENCV_LIBDIR
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REPLACE " " ";" OPENCV_LIBS ${OPENCV_LIBS})
  else ()
    message(SEND_ERROR "OpenCV file for pkg-config could not be found at location \"${OPENCV_PC_PATH}\". Please, check the path and try it again.")
  endif ()
else ()
  # TODO Remake to find_package(OpenCV REQUIRED) ONLY + version checks
  find_package(OpenCV REQUIRED PATHS ${OPENCV_PATH})
  if (OpenCV_VERSION VERSION_LESS 2.4.2)
    message(SEND_ERROR "OpenCV version >= 2.4.2 is required.")
  endif ()
  if (OpenCV_FOUND)
    set(OPENCV_INCLUDE_PATH ${OpenCV_INCLUDE_DIRS})
    set(OPENCV_LIBS ${OpenCV_LIBS})
  endif ()
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  find_library(vtapi vtapi ${CMAKE_SOURCE_DIR}/../vtapi/install/lib)
  set(VTAPI_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../vtapi/install/include)
else ()
  find_library(vtapi vtapi)
  set(VTAPI_INCLUDE_PATH )
endif ()



add_subdirectory(src)
