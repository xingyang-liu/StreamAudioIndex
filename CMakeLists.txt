cmake_minimum_required(VERSION 3.5)
project(StreamAudioIndex)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/include/python3.5/)
find_package(Threads REQUIRED)

set(SOURCE_FILES
        BasicStructure/AudioInfo.cpp
        BasicStructure/AudioInfo.h
        BasicStructure/AudioInfo.i
        BasicStructure/AudioInfo_wrap.cxx
        BasicStructure/IdfCode.cpp
        BasicStructure/IdfCode.h
        BasicStructure/NodeInfo.cpp
        BasicStructure/NodeInfo.h
        BasicStructure/ProgramList.cpp
        BasicStructure/ProgramList.h
        cpp-btree-1.0.1/btree.h
#        cpp-btree-1.0.1/btree_bench.cc
        cpp-btree-1.0.1/btree_container.h
        cpp-btree-1.0.1/btree_map.h
        cpp-btree-1.0.1/btree_set.h
#        cpp-btree-1.0.1/btree_test.cc
        cpp-btree-1.0.1/btree_test.h
#        cpp-btree-1.0.1/btree_test_flags.cc
        cpp-btree-1.0.1/safe_btree.h
        cpp-btree-1.0.1/safe_btree_map.h
        cpp-btree-1.0.1/safe_btree_set.h
#        cpp-btree-1.0.1/safe_btree_test.cc
        HybridIndex/HybridIndexManager.cpp
        HybridIndex/HybridIndexManager.h
        I0Sort/I0SortThreadFre.h
        I0Sort/I0SortThreadSig.h
        I0Sort/I0SortThreadSim.h
        I0Sort/InvertedIndex0SortThread.h
        Merge/InvertedIndexMergerThread.h
        Merge/InvertedIndexMergerThreadFre.h
        Merge/InvertedIndexMergerThreadSig.h
        Merge/InvertedIndexMergerThreadSim.h
        PhonomeIndex/PhoIndexManager.cpp
        PhonomeIndex/PhoIndexManager.h
        PhonomeIndex/phonome.cpp
        PhonomeIndex/phonome.h
        TemplateIndex/Cmp.h
        TemplateIndex/IndexString.cpp
        TemplateIndex/IndexString.h
        TemplateIndex/IndexTemplate.cpp
        TemplateIndex/IndexTemplate.h
        TemplateIndex/Sig.cpp
        TemplateIndex/Sig.h
        TextIndex/IndexManager.cpp
        TextIndex/IndexManager.h
        TextIndex/InvertedIndex.cpp
        TextIndex/InvertedIndex.h
        utils/test.cpp
        utils/test.h
        utils/utils.cpp
        utils/utils.h
        json.hpp
        main.cpp)

add_executable(StreamAudioIndex ${SOURCE_FILES})
target_link_libraries(StreamAudioIndex python3.5m pthread)
