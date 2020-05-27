# Install script for directory: /home/krypton/Downloads/PA4-MeshEdit/CGL/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/krypton/Downloads/PA4-MeshEdit/build/CGL/src/libCGL.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/CGL" TYPE FILE FILES
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/CGL.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/vector2D.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/vector3D.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/vector4D.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/matrix3x3.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/matrix4x4.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/quaternion.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/complex.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/color.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/osdtext.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/viewer.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/base64.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/tinyxml2.h"
    "/home/krypton/Downloads/PA4-MeshEdit/CGL/src/renderer.h"
    )
endif()

