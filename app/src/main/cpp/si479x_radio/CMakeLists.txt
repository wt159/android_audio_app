cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(si479x_radio LANGUAGES C CXX)

#SET(CMAKE_SYSTEM_NAME Linux)
#SET(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
#SET(CMAKE_CXX_COMPILER arm-linux-gnueabihf-c++)

INCLUDE_DIRECTORIES(include/config include/config/part_number include/config/share include/si479x include/si469x include)

ADD_SUBDIRECTORY(tuner)
ADD_SUBDIRECTORY(radio)
ADD_SUBDIRECTORY(flash_tool)

#TARGET_LINK_LIBRARIES(si479x_radio si479x rds si4769x dcapp)

