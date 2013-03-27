#
# Top level makefile for Reflect
#
# John Farrier, Hellebore Consulting LLC
#

#
# Cmake Configuration
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#
# Project Name
#

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)

#
# User Options
#

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# Compiler Settings
#

if(MSVC) 
  # Force to always compile with warning level 4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]") 
	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") 
  else() 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4") 
  endif() 

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Compiler Flags for Debug Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE) 

  add_definitions("/D_CRT_SECURE_NO_WARNINGS /wd4251 /nologo")
endif() 
 
if(BORLAND) 
  set(CMAKE_CXX_FLAGS "" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "Compiler Flags for Debug Builds" FORCE)  
  set(CMAKE_CXX_FLAGS_RELEASE  "" CACHE STRING "Compiler Flags for Release Builds" FORCE) 
endif() 

message (STATUS "SYSTEM: ${CMAKE_SYSTEM_NAME}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	EXEC_PROGRAM(xcrun ARGS -find clang++ OUTPUT_VARIABLE XCODE_CLANG_PATH)
	set (XCODE_CLANG_PATH "${XCODE_CLANG_PATH}" CACHE STRING "Xcode clang path")
	set (CMAKE_CXX_COMPILER "${XCODE_CLANG_PATH}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") 
	set (CMAKE_COMPILER_IS_GNUCXX 1)
endif()
 
if(CMAKE_COMPILER_IS_GNUCXX) 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11 -Wall" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic" CACHE STRING "Compiler Flags for Debug Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE) 
endif() 

#
# Build and Install Settings
#

set(CMAKE_DEBUG_POSTFIX          "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX        ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX     ""  CACHE STRING "add a postfix, usually empty on windows")

# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #

set(TARGET_LIBRARIES ${SYSLIBS})
include_directories(${HEADER_PATH})

# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #

# --------------------------------------------------------------------------- #
# GTest Unit Tests
# --------------------------------------------------------------------------- #

option(REFLECT_GTEST "Set to ON to enable Google tests Reflect." ON)

if(REFLECT_GTEST)
	set(PROJECT_TEST_NAME reflectTest)
	set(GTEST_INCLUDE_DIR gtest/include)
	set(GTEST_LIBRARY gtest)
	set(GTEST_MAIN_LIBRARY gtest_main)

	enable_testing(true)

	add_executable(${PROJECT_TEST_NAME} src/test.cpp)

	# VS2012 doesn't support true variadic templates
	if(MSVC) 
		add_definitions( /D _VARIADIC_MAX=10 )
	endif()

	ADD_SUBDIRECTORY(gtest)

	include_directories(${HEADER_PATH})
	include_directories(${GTEST_INCLUDE_DIR})
	target_link_libraries(${PROJECT_TEST_NAME} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
	add_test(${PROJECT_TEST_NAME} ${PROJECT_TEST_NAME} )
	add_custom_command(TARGET ${PROJECT_TEST_NAME} POST_BUILD COMMAND $<TARGET_FILE:${PROJECT_TEST_NAME}>)
endif()
