cmake_minimum_required( VERSION 2.6 )
set (PROJECT_NAME CommSerial)
set (COM_LIB_NAME ComSer)
set (TEST_NAME ComSerTest)

project(${PROJECT_NAME})

option(release "Compile the release version" OFF)

if (release)
  set (CMAKE_BUILD_TYPE Release)
else()
  set (CMAKE_BUILD_TYPE Debug)
endif()

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/include/)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/)

include_directories(SYSTEM ${HEADER_DIR})

set(CS_HEADERS
	${HEADER_DIR}/Parser.h
	${HEADER_DIR}/Serial.h
	${HEADER_DIR}/SerialConnection.h
	${HEADER_DIR}/ConditionVariable.h
	${HEADER_DIR}/Crc.h
	${HEADER_DIR}/ObjStream.h
	${HEADER_DIR}/Base32.h
	${HEADER_DIR}/Pack.h
	${HEADER_DIR}/PackManager.h
	${HEADER_DIR}/stdafx.h)
	
set(CS_SOURCES
	${SOURCE_DIR}/Parser.cpp
	${SOURCE_DIR}/WinSerial.cpp
	${SOURCE_DIR}/LinSerial.cpp
	${SOURCE_DIR}/SerialConnection.cpp
	${SOURCE_DIR}/Crc.cpp
	${SOURCE_DIR}/Base32.cpp
	${SOURCE_DIR}/Pack.cpp
	${SOURCE_DIR}/PackManager.cpp
	${SOURCE_DIR}/ObjStream.cpp)
	
set(CS_GLOB
	${CS_HEADERS}
	${CS_SOURCES})

find_package(Threads)

add_library(${COM_LIB_NAME} STATIC ${CS_GLOB})

set(TEST_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/Test/TestPack.h)

set(TEST_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Test/Source.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Test/TestPack.cpp)

add_executable(${TEST_NAME} ${TEST_HEADERS} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME} ${COM_LIB_NAME})
target_link_libraries(${TEST_NAME} ${COM_LIB_NAME} ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET ${COM_LIB_NAME} ${TEST_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${COM_LIB_NAME} ${TEST_NAME} PROPERTY CXX_STANDARD_REQUIRED 11)
