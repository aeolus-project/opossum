
SET (EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin/${CMAKE_BUILD_TYPE}")
SET (LIBRARY_OUTPUT_PATH "${MAINFOLDER}/bin/${CMAKE_BUILD_TYPE}")

#FORCE CXX COMPILATION FOR ALL SOURCES FILES
FILE (GLOB_RECURSE test_SRCS *.cpp *.cxx *.cc *.C *.c)
FOREACH (SRC_FILE ${test_SRCS})
	set_source_files_properties(${SRC_FILE} PROPERTIES LANGUAGE CXX)
ENDFOREACH (SRC_FILE ${test_SRCS})

#ADD boost_unit_test_framework because it is not set on ${Boost_LIBRARIES}
SET (test_LIBS boost_unit_test_framework ${Boost_LIBRARIES} ${XML2_LIBRARY_DIRS} gexf ilocplex cplex concert m pthread) #boost_random gexf xml2
#SET (test_LIBS boost_unit_test_framework boost_random gexf xml2)
SET (test_BIN ${PROJECT_NAME}-tests)

ADD_EXECUTABLE(${test_BIN} ${test_SRCS})
TARGET_LINK_LIBRARIES(${test_BIN} ${test_LIBS})

if(START_TESTS)
    ADD_CUSTOM_TARGET(check ALL "${MAINFOLDER}/bin/Test/${test_BIN}" DEPENDS ${test_BIN} COMMENT "Executing unit tests..." VERBATIM SOURCES ${test_SRCS})
    ADD_CUSTOM_TARGET(test "${MAINFOLDER}/bin/Test/${test_BIN}" DEPENDS ${test_BIN} COMMENT "Executing unit tests..." VERBATIM SOURCES ${test_SRCS})
endif (START_TESTS)

