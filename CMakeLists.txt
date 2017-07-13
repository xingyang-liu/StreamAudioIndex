cmake_minimum_required(VERSION 3.7)
project(sample_connector)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        demoC++/index.cpp
        json.hpp
        main.cpp demoC++/index.h demoC++/AudioInfo.cpp demoC++/AudioInfo.h demoC++/Fre.cpp demoC++/Fre.h demoC++/Sig.cpp demoC++/Sig.h demoC++/TermFreq.cpp demoC++/TermFreq.h demoC++/I0.cpp demoC++/I0.h demoC++/Ii.cpp demoC++/Ii.h demoC++/IndexAll.cpp demoC++/IndexAll.h demoC++/utils.h demoC++/utils.cpp)

add_executable(sample_connector ${SOURCE_FILES})