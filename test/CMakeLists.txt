SET (EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin/Test")
SET (LIBRARY_OUTPUT_PATH "${MAINFOLDER}/bin/Test")

FILE (GLOB_RECURSE test_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
SET (test_LIBS boost_unit_test_framework boost_random gexf xml2)
SET (test_BIN ${PROJECT_NAME}-tests)

ADD_EXECUTABLE(${test_BIN} ${test_SRCS})
TARGET_LINK_LIBRARIES(${test_BIN} ${test_LIBS})

if(START_TESTS)
    ADD_CUSTOM_TARGET(check ALL "${MAINFOLDER}/bin/Test/${test_BIN}" DEPENDS ${test_BIN} COMMENT "Executing unit tests..." VERBATIM SOURCES ${test_SRCS})
    ADD_CUSTOM_TARGET(test "${MAINFOLDER}/bin/Test/${test_BIN}" DEPENDS ${test_BIN} COMMENT "Executing unit tests..." VERBATIM SOURCES ${test_SRCS})
endif (START_TESTS)

