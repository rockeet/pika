cmake_minimum_required(VERSION 3.16)
project(pika)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1y -D_DARWIN_UNLIMITED_SELECT -D_GNU_SORUCE")

include_directories(third/pink/pink/include)
include_directories(third/pink/pink/src)
include_directories(third/pink/pink)
include_directories(third/pink)
include_directories(third/blackwidow/include)
include_directories(third/blackwidow/src)
include_directories(third/blackwidow)
include_directories(third/slash)
include_directories(src)
include_directories(.)
include_directories(../topling-core/src)
include_directories(../topling-core/boost-include)
include_directories(../rocksdb)
include_directories(../rocksdb/include)

file(GLOB SOURCES0 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        third/blackwidow/include/*.h
        third/blackwidow/src/*.cc
        third/pink/pink/include/*.h
        third/pink/pink/src/*.cc
        third/slash/slash/include/*.h
        third/slash/slash/src/*.cc
        src/*.cc
        src/table/*.cc
        include/*.h
)
#list(JOIN SOURCES "\n" TEXT)
#message(STATUS ${TEXT})
add_executable(pika ${SOURCES0})
