cmake_minimum_required(VERSION 3.7)
project(hash_0e)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/index.cpp
        json.hpp
        main.cpp src/index.h src/AudioInfo.cpp src/AudioInfo.h
        src/utils.h src/utils.cpp src/IndexManager.cpp src/IndexManager.h src/InvertedIndex.cpp
        src/InvertedIndex.h src/NodeInfo.cpp src/NodeInfo.h src/ProgramList.cpp src/ProgramList.h
        template/phonome.cpp template/phonome.h template/SimpleDTW.cpp template/SimpleDTW.h template/IndexTemplate.cpp template/IndexTemplate.h template/IndexString.cpp template/IndexString.h template/Cmp.h template/InvertedIndexMergerThread.h template/InvertedIndexMergerThreadFre.h template/InvertedIndexMergerThreadSig.h template/InvertedIndexMergerThreadSim.h)

add_executable(StreamAudioIndex ${SOURCE_FILES})
find_package(Threads REQUIRED)
target_link_libraries(StreamAudioIndex Threads::Threads)