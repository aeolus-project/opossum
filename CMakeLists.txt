# <Copyright Holder>. Copyright (C) <Copyright Year(s)>. <License>.

#export ILOG_LICENSE_FILE=access.ilm

################ Project Properties ####################
CMAKE_MINIMUM_REQUIRED (VERSION 2.6.2)
PROJECT (opossum)
SET (APPLICATION_NAME "OPOSSUM")
SET (APPLICATION_CODENAME "${PROJECT_NAME}")
SET (APPLICATION_COPYRIGHT_YEARS "2012")
SET (APPLICATION_VERSION_MAJOR 0)
SET (APPLICATION_VERSION_MINOR 1)
SET (APPLICATION_VERSION_PATCH 0)
SET (APPLICATION_VERSION_TYPE SNAPSHOT)
SET (APPLICATION_VERSION_STRING "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}-${APPLICATION_VERSION_TYPE}")
SET (APPLICATION_VENDOR_ID "momo_nono")
SET (APPLICATION_VENDOR_NAME "momo_nono")
SET (APPLICATION_VENDOR_URL "www.i3s.unice.fr")
SET (APPLICATION_ID "${APPLICATION_VENDOR_ID}.${PROJECT_NAME}")

################ FORCE COMPILATION 32 Bits ####################
SET (FORCE_32 TRUE)

################ DETECT SYSTEM AND ARCHITECTURE ####################
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET (OS_SYSTEM "linux")
	IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	    SET (ARCH_SYSTEM "64")
	ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8)
	    SET (ARCH_SYSTEM "32")
	ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

elseif (WIN32)
	SET (OS_SYSTEM "windows")
	if (CMAKE_CL_64) 
	    SET (ARCH_SYSTEM "64")
	else() 
	    SET (ARCH_SYSTEM "32")
	endif(CMAKE_CL_64)
elseif (APPLE)
	SET (OS_SYSTEM "osx")
	SET (ARCH_SYSTEM "64")

endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

message(STATUS "OS_TYPE: ${OS_SYSTEM}" )
message(STATUS "ARCH_TYPE: ${ARCH_SYSTEM} bits" )
message(STATUS "FORCE_32: ${FORCE_32}" )

################ FORCE SET TYPE OF BUILD ####################
# DEBUG or RELEASE
#SET (CMAKE_BUILD_TYPE "DEBUG")
#build Debug only when explicitally set
if(CMAKE_BUILD_TYPE MATCHES DEBUG OR CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "debug")
	message(STATUS "Building Debug")
	SET (CMAKE_BUILD_TYPE "Debug")
else()
	message(STATUS "Building Release")
	SET (CMAKE_BUILD_TYPE "Release")
endif()

################ # DEBUG or RELEASE ####################
SET (CMAKE_VERBOSE_MAKEFILE 1) # Use 1 for debugging, 0 for release
 
################ Project Output Paths ####################
SET (MAINFOLDER ${PROJECT_SOURCE_DIR})

################ Project Search Paths ####################
LIST (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}")
LIST (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}/tools")
LIST (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}/thirdparty")
SET (CMAKE_MODULE_PATH "${MAINFOLDER}/tools/share/cmake")

################ BOOST Lib Check ####################
FIND_PACKAGE (Boost REQUIRED)
if(Boost_FOUND)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
	#message(STATUS "BOOST Lib found" )
else()
	#message(STATUS "BOOST Lib not found so put this library in the folder: ${CMAKE_LINKER_PATH}" )
endif(Boost_FOUND)

################ XML2 Lib Check ####################
FIND_PACKAGE(XML2 REQUIRED)
INCLUDE_DIRECTORIES(${XML2_INCLUDE_DIRS})
LINK_DIRECTORIES(${XML2_LIBRARY_DIRS})


################ INCLUDE others Libs ####################
SET (CMAKE_INCLUDE_PATH "${MAINFOLDER}/include")
if(FORCE_32)
	SET (CMAKE_LINKER_PATH "${MAINFOLDER}/lib/${OS_SYSTEM}/lib32")
else()
	SET (CMAKE_LINKER_PATH "${MAINFOLDER}/lib/${OS_SYSTEM}/lib${ARCH_SYSTEM}")
endif(FORCE_32)

message(STATUS "INCLUDE_PATH: ${CMAKE_INCLUDE_PATH}" )
message(STATUS "LINKER_PATH: ${CMAKE_LINKER_PATH}" )

INCLUDE_DIRECTORIES(${CMAKE_INCLUDE_PATH})
LINK_DIRECTORIES(${CMAKE_LINKER_PATH})


#################### CPLEX #################### 
SET(CPLEX_ROOT_DIR "/opt/ibm/ILOG/CPLEX_Studio_Academic123")
message(STATUS "CPLEX_ROOT_DIR: ${CPLEX_ROOT_DIR}" )

INCLUDE_DIRECTORIES(${CPLEX_ROOT_DIR}/cplex/include ${CPLEX_ROOT_DIR}/concert/include)
LINK_DIRECTORIES(${CPLEX_ROOT_DIR}/cplex/lib/x86_sles10_4.1/static_pic ${CPLEX_ROOT_DIR}/concert/lib/x86_sles10_4.1/static_pic)

#RESET DEFAULT FLAGS 
SET(CMAKE_CXX_FLAGS "")

if(FORCE_32)
	SET(CMAKE_CXX_COMPILE_OBJECT
	    "<CMAKE_CXX_COMPILER>  <DEFINES> <FLAGS> -fmessage-length=0 -Wall -c -O0 -O2 -m32 -fPIC -fexceptions -DNDEBUG -DIL_STD <SOURCE> -o <OBJECT>")

	SET(CMAKE_CXX_LINK_EXECUTABLE
	    "<CMAKE_CXX_COMPILER>  <FLAGS> -fmessage-length=0 -O0 -Wall -O2 -m32 -fPIC -fexceptions -DNDEBUG -DIL_STD <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
else()

	SET(CMAKE_CXX_COMPILE_OBJECT
	    "<CMAKE_CXX_COMPILER>  <DEFINES> <FLAGS> -fmessage-length=0 -Wall -c -O0 -O2 -fPIC -fexceptions -DNDEBUG -DIL_STD <SOURCE> -o <OBJECT>")

	SET(CMAKE_CXX_LINK_EXECUTABLE
	    "<CMAKE_CXX_COMPILER>  <FLAGS> -fmessage-length=0 -Wall -O0 -O2 -fPIC -fexceptions -DNDEBUG -DIL_STD <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
endif(FORCE_32)
	

#################### Configure Files #################### 
FILE (GLOB_RECURSE CONFIGINPUTS1 include/*.in.h.cmake)
FILE (GLOB_RECURSE CONFIGINPUTS2 include/*.h.in.cmake)
FILE (GLOB_RECURSE CONFIGINPUTS3 src/*.in.h.cmake)
FILE (GLOB_RECURSE CONFIGINPUTS4 src/*.h.in.cmake)
LIST (APPEND CONFIGINPUTS "${CONFIGINPUTS1}")
LIST (APPEND CONFIGINPUTS "${CONFIGINPUTS2}")
LIST (APPEND CONFIGINPUTS "${CONFIGINPUTS3}")
LIST (APPEND CONFIGINPUTS "${CONFIGINPUTS4}")
LIST (APPEND CONFIGINPUTS "${CONFIGINPUTS5}")
LIST (APPEND CONFIGINPUTS "${CONFIGINPUTS6}")
FOREACH (CONFIGINPUT ${CONFIGINPUTS})
    STRING (REPLACE ".in.h.cmake" ".h" CONFIGOUTPUT1 "${CONFIGINPUT}")
    STRING (REPLACE ".h.in.cmake" ".h" CONFIGOUTPUT2 "${CONFIGOUTPUT1}")
    CONFIGURE_FILE ("${CONFIGINPUT}" "${CONFIGOUTPUT2}")
ENDFOREACH (CONFIGINPUT ${CONFIGINPUTS})

#################### Add Build Targets #################### 
ADD_SUBDIRECTORY(src)
#Build the test drivers if necessary
if(BUILD_TESTS)
    FILE (GLOB_RECURSE CONFIGINPUTS5 test/*.in.h.cmake)
    FILE (GLOB_RECURSE CONFIGINPUTS6 test/*.h.in.cmake)
    ADD_SUBDIRECTORY(test)
    message(STATUS "Building Test")
endif (BUILD_TESTS)

#################### Add Install Targets #################### 
IF (EXISTS "${MAINFOLDER}/include/${PROJECT_NAME}" AND IS_DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}")
    INSTALL(DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}" DESTINATION "include")
ENDIF (EXISTS "${MAINFOLDER}/include/${PROJECT_NAME}" AND IS_DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}")

#################### Add Documentation Targets #################### 
INCLUDE (DocumentationTargets)

