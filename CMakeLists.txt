cmake_minimum_required(VERSION 2.8.3)
project(gpu_voxels)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED)

catkin_package(
   INCLUDE_DIRS packages/gpu_voxels/src
#  LIBRARIES gpu_voxels
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

SET(ICMAKER_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/icmaker)
INCLUDE(${ICMAKER_DIRECTORY}/CMakeLists.txt)

ADD_SUBDIRECTORY(packages)
ADD_SUBDIRECTORY(doc)

IF (EXISTS "script" AND IS_DIRECTORY "script")
  ADD_SUBDIRECTORY(script)
ENDIF ()
