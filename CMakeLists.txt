cmake_minimum_required(VERSION 3.7)
project(hash_0e)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/index.cpp
        json.hpp
        main.cpp src/index.h src/AudioInfo.cpp src/AudioInfo.h src/IndexManager.cpp src/IndexManager.h src/utils.h src/utils.cpp src/ProgramList.cpp src/ProgramList.h src/NodeInfo.cpp src/NodeInfo.h src/InvertedIndex.cpp src/InvertedIndex.h)

add_executable(hash_0e ${SOURCE_FILES})
find_package(Threads REQUIRED)
target_link_libraries(hash_0e Threads::Threads)