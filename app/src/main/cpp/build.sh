#!/bin/bash
cd build
if [[ “$@“ =~ "-c" ]];then
    echo "----------------------------cmake clean----------------------------"
    rm -rf ./*
    exit
fi



export ANDROID_NDK_HOME=/home/weitaiping/study/android-ndk-r21e
export PATH=$ANDROID_NDK_HOME:$PATH
 
if [[ “$@“ =~ "-r" ]];then
    echo "----------------------------cmake release----------------------------"
    cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DANDROID_NDK=$ANDROID_NDK_HOME \
        -DANDROID_ABI=arm64-v8a \
        -DANDROID_PLATFORM=android-21 \
        -DANDROID_STL=c++_static \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_VERBOSE_MAKEFILE=ON \
        ..
else      
    echo "----------------------------cmake debug----------------------------"
    cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DANDROID_NDK=$ANDROID_NDK_HOME \
        -DANDROID_ABI=arm64-v8a \
        -DANDROID_PLATFORM=android-21 \
        -DANDROID_STL=c++_static \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_VERBOSE_MAKEFILE=OFF \
        ..
fi
	  
make 
 
rm -rf CMakeCache.txt
rm -rf CMakeFiles
rm -rf cmake_install.cmake
rm -rf Makefile
rm -rf CTestTestfile.cmake