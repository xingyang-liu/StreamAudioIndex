cmake_minimum_required(VERSION 3.7)
project(hash_0e)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/index.cpp
        json.hpp
        main.cpp src/index.h BasicStructure/AudioInfo.cpp BasicStructure/AudioInfo.h
        src/utils.h src/utils.cpp TextIndex/IndexManager.cpp TextIndex/IndexManager.h TextIndex/InvertedIndex.cpp
        TextIndex/InvertedIndex.h BasicStructure/NodeInfo.cpp BasicStructure/NodeInfo.h BasicStructure/ProgramList.cpp
        BasicStructure/ProgramList.h TemplateIndex/Sig.h TemplateIndex/Sig.cpp
        PhonomeIndex/Phoneme.cpp PhonomeIndex/Phoneme.h src/SimpleDTW.cpp src/SimpleDTW.h TemplateIndex/IndexTemplate.h
        TemplateIndex/Cmp.h Merge/InvertedIndexMergerThread.h Merge/InvertedIndexMergerThreadFre.h Merge/InvertedIndexMergerThreadSig.h
        Merge/InvertedIndexMergerThreadSim.h PhonomeIndex/PhonomeIndex.cpp PhonomeIndex/PhonomeIndex.h
        PhonomeIndex/PhoIndexManager.cpp PhonomeIndex/PhoIndexManager.h Sort/InvertedIndex0SortThread.h Sort/I0SortThreadFre.h
        Sort/I0SortThreadSig.h Sort/I0SortThreadSim.h TemplateIndex/ManagerTemplate.h src/HybridIndexManager.cpp src/HybridIndexManager.h)

add_executable(StreamAudioIndex ${SOURCE_FILES})
find_package(Threads REQUIRED)
target_link_libraries(StreamAudioIndex Threads::Threads)