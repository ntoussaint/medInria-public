#######################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#######################################################################

cmake_minimum_required(VERSION 2.8.1)

project(itkDataDiffusionGradientListPlugin)

## #################################################################
## Setup version numbering
## #################################################################

set(${PROJECT_NAME}_VERSION ${medInria_VERSION})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

## #################################################################
## Resolve dependencies
## #################################################################

find_package(dtk REQUIRED)
include(${dtk_USE_FILE})
include(dtkPlugin)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})


## #################################################################
## Input
## #################################################################

set(${PROJECT_NAME}_HEADERS_MOC
  itkDataDiffusionGradientListPlugin.h
  itkDataDiffusionGradientList.h)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HEADERS_MOC})

set(${PROJECT_NAME}_SOURCES
  itkDataDiffusionGradientListPlugin.cpp
  itkDataDiffusionGradientList.cpp)

## #################################################################
## Build rules
## #################################################################

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)
if (NOT MSVC)
  # Shouldn't the QT_USE_FILE set this appropriately?
  add_definitions(-DQT_NO_DEBUG)
endif()

qt4_wrap_cpp(${PROJECT_NAME}_SOURCES_MOC ${${PROJECT_NAME}_HEADERS_MOC})

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_HEADERS}
  ${${PROJECT_NAME}_SOURCES_MOC}
  ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES}
  dtkItkLogForwarder
  dtkCore dtkLog)

# Group common files together in Visual Studio.
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_SOURCES_MOC})

## #################################################################
## Install rules
## #################################################################

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib)
