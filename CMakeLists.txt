cmake_minimum_required(VERSION 3.17)
project(HashTablesOpenAddressing)

set(CMAKE_CXX_STANDARD 17)

# Standalone app
add_executable(HashTablesOpenAddressing
    main.cpp
    HashTableDictionary.cpp HashTableDictionary.hpp
    InvertedListDictionary.cpp InvertedListDictionary.hpp
    SmallIntMixedOperations.cpp SmallIntMixedOperations.hpp
    Operations.hpp
)

# Harness for LRU experiments
add_executable(HashTableHarness
    LRUHarness.cpp
    HashTableDictionary.cpp HashTableDictionary.hpp
    InvertedListDictionary.cpp InvertedListDictionary.hpp
    SmallIntMixedOperations.cpp SmallIntMixedOperations.hpp
    Operations.hpp
)
