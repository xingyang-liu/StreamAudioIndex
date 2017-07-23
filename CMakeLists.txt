cmake_minimum_required(VERSION 3.7)
project(hash_0e)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/index.cpp
        json.hpp
        main.cpp src/index.h src/AudioInfo.cpp src/AudioInfo.h src/Ii.cpp src/Ii.h src/IndexAll.cpp src/IndexAll.h
        src/utils.h src/utils.cpp src/ListInfo.cpp src/ListInfo.h
        template/phonome.cpp template/phonome.h template/SimpleDTW.cpp template/SimpleDTW.h)

add_executable(StreamAudioIndex ${SOURCE_FILES})
find_package(Threads REQUIRED)
target_link_libraries(StreamAudioIndex Threads::Threads)