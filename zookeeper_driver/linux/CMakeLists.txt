cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME Enviroment)
set(SERVER_NAME zookeeper_test)

project(MIGRATION CXX C)
ENABLE_LANGUAGE(C)

#set compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#flag
#-pg: produce run debug information
SET(CMAKE_C_FLAG "${CMAKE_C_FLAG} -Wall ")
SET(CMAKE_CXX_FLAG "${CMAKE_CXX_FLAG} -Wall ")

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -o0 -Wall  -g -ggdb -std=c99 ")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -o0  -Wall -g -ggdb ")

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -o3 -Wall ")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -o3 -Wall ")

#includ path
set(LOCAL_INCLUDE_DIR /usr/local/include)
set(OPT_INCLUDE_DIR /opt/local/include)

#64-byte machine
set(LIB64_DIR /lib64)
set(USR_LIB64_DIR  /usr/lib64)
set(LOCAL_LIB64_DIR /usr/local/lib64)

set(DRIVER_DIR ${PROJECT_SOURCE_DIR}/../driver/)

#include dir
include_directories(${LOCAL_INCLUDE_DIR})
include_directories(${OPT_INCLUDE_DIR})
include_directories(${LIB64_DIR})
include_directories(${USR_LIB64_DIR})
include_directories(${LOCAL_LIB64_DIR})

include_directories(${DRIVER_DIR}/include)

# find library
#FIND_LIBRARY(FILESYSTEM NAMES boost_filesystem PATHS /home/wuxiang/lib/)
#FIND_LIBRARY(BOOST_THREAD NAMES boost_thread  PATHS /home/wuxiang/lib/)
#FIND_LIBRARY(MYSQLCLIENT_LIB NAMES mysqlclient  PATHS /home/wuxiang/lib/)
FIND_LIBRARY(ZOOKEEPERLIB NAMES zookeeper_mt  PATHS ${PROJECT_SOURCE_DIR}/../lib/)

#source file 
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/../ OWN_FILE)
#FILE(GLOB_RECURSE DRIVER_SRC  ${DRIVER_DIR}/src/*.c)

###################################################
##create excutable file
##################################################

add_executable(  ${SERVER_NAME}  ${OWN_FILE}
              )

target_link_libraries(   ${SERVER_NAME}
					     pthread
						 ${ZOOKEEPERLIB}
					 )
