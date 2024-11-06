cmake_minimum_required(VERSION 3.1)
project(i2s VERSION 1.0.0 LANGUAGES CXX C)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
add_compile_options(-Wall -Wextra -fPIC)

# 设置输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 源文件列表
set(I2S_SOURCES
    src/i2s_api.cpp
    src/NormalI2S.cpp
    src/HighSpeedI2S.cpp
    src/I2SFactory.cpp
    src/TinyalsaWrapper.cpp
)

# 头文件目录
set(I2S_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 创建动态库目标
add_library(i2s SHARED ${I2S_SOURCES})

# 设置库的属性
set_target_properties(i2s PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "include/i2s_api.h"
)

# 设置包含目录
target_include_directories(i2s
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyalsa/include
)

# 链接动态加载库
target_link_libraries(i2s
    PRIVATE
        dl
)

# 安装规则
include(GNUInstallDirs)
install(TARGETS i2s
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 可选：构建示例程序
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif() 