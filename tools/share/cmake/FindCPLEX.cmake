SET(CPLEX_ROOT_DIR "" CACHE PATH "CPLEX root directory")

FIND_PATH(CPLEX_INCLUDE_DIR
  ilcplex/cplex.h
  PATHS "/opt/ibm/ILOG/CPLEX_Studio_Academic124/cplex/include"
  PATHS "/opt/ibm/ILOG/CPLEX_Studio_Academic123/cplex/include"
  HINTS ${CPLEX_ROOT_DIR}/cplex/include
)
IF(UNIX)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so;.a")
ENDIF()
IF(${ARCH_SYSTEM} MATCHES "64" AND NOT FORCE_32)
FIND_LIBRARY(CPLEX_LIBRARY
  cplex
  PATHS "/opt/ibm/ILOG/CPLEX_Studio_Academic123/cplex/lib/x86-64_sles10_4.1/static_pic/"
  PATHS "/opt/ibm/ILOG/CPLEX_Studio_Academic124/cplex/lib/x86-64_sles10_4.1/static_pic/"
  HINTS ${CPLEX_ROOT_DIR}/lib/x86-64_sles10_4.1/static_pic/
)
ELSE() 
FIND_LIBRARY(CPLEX_LIBRARY
  cplex
  PATHS "/opt/ibm/ILOG/CPLEX_Studio_Academic123/cplex/lib/x86_sles10_4.1/static_pic/"
  PATHS "/opt/ibm/ILOG/CPLEX_Studio_Academic124/cplex/lib/x86_sles10_4.1/static_pic/"
  HINTS ${CPLEX_ROOT_DIR}/lib/x86_sles10_4.1/static_pic/
)
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CPLEX DEFAULT_MSG CPLEX_LIBRARY CPLEX_INCLUDE_DIR)

IF(CPLEX_FOUND)
  SET(CPLEX_INCLUDE_DIRS ${CPLEX_INCLUDE_DIR})
  SET(CPLEX_LIBRARIES ${CPLEX_LIBRARY};)
  IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(CPLEX_LIBRARIES "${CPLEX_LIBRARIES};m;pthread")
  ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
ENDIF(CPLEX_FOUND)

MARK_AS_ADVANCED(CPLEX_LIBRARY CPLEX_INCLUDE_DIR)
