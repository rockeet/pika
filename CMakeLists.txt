cmake_minimum_required(VERSION 3.16)
project(pika)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DARWIN_UNLIMITED_SELECT -D_GNU_SORUCE -DFOLLY_NO_CONFIG=1")

#SET(CMAKE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../toplingdb/sideplugin/topling-core/boost-include)
#SET(Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../toplingdb/sideplugin/topling-core/boost-include)
#SET(DOUBLE_CONVERSION_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third/double-conversion)
#SET(DOUBLE_CONVERSION_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/third/double-conversion)
#add_subdirectory(third/folly)

include_directories(third/pink/pink/include)
include_directories(third/pink/pink/src)
include_directories(third/pink/pink)
include_directories(third/pink)
include_directories(third/blackwidow/include)
include_directories(third/blackwidow/src)
include_directories(third/blackwidow)
include_directories(third/slash)
include_directories(third/folly)
include_directories(src)
include_directories(include)
include_directories(.)
include_directories(../toplingdb)
include_directories(../toplingdb/include)
include_directories(../toplingdb/sideplugin/rockside/src)
include_directories(../toplingdb/sideplugin/topling-core/src)
include_directories(../toplingdb/sideplugin/topling-core/boost-include)
include_directories(../toplingdb/sideplugin/topling-core/3rdparty/zstd)
include_directories(../toplingdb/sideplugin/topling-rocks/src)
include_directories(/hddpool/pxe/node-shared/vcpkg/installed/x64-linux/include)

file(GLOB SOURCES0 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        third/blackwidow/include/*.h
        third/blackwidow/src/*.cc
        third/pink/pink/include/*.h
        third/pink/pink/src/*.cc
        third/slash/slash/include/*.h
        third/slash/slash/src/*.cc
        src/*.cc
        include/*.h
)
#list(JOIN SOURCES "\n" TEXT)
#message(STATUS ${TEXT})
add_executable(pika ${SOURCES0})
