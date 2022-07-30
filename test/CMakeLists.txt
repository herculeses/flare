
include(require_gtest)
include(require_gmock)
set(GTM_LIB
        ${GTEST_LIB}
        ${GMOCK_LIB}
        )

find_package(Gperftools)
include_directories(${GPERFTOOLS_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/test)


set(DYNAMIC_LIB ${DYNAMIC_LIB} ${GPERFTOOLS_LIBRARIES})

include(CompileProto)
set(IO_PROTO_FILES
        io/cord_buf.proto
        json/message.proto
        json/addressbook.proto
        json/addressbook1.proto
        json/addressbook_encode_decode.proto
        json/addressbook_map.proto)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/hdrs)
set(PROTOC_FLAGS ${PROTOC_FLAGS} -I${CMAKE_SOURCE_DIR}/test -I${CMAKE_SOURCE_DIR})
compile_proto(IO_PROTO_HDRS IO_PROTO_SRCS ${CMAKE_BINARY_DIR}/test
        ${CMAKE_BINARY_DIR}/test/hdrs
        ${CMAKE_SOURCE_DIR}/test
        "${IO_PROTO_FILES}")

file(GLOB_RECURSE TESTING_SOURCES "${PROJECT_SOURCE_DIR}/test/testing/*.cc")
carbin_cc_library(
        NAMESPACE flare
        NAME testing
        SOURCES ${TESTING_SOURCES} ${IO_PROTO_SRCS}
        PUBLIC_LINKED_TARGETS ${CARBIN_DYLINK} ${DYNAMIC_LIB} flare::flare-debug-test
        PRIVATE_COMPILE_OPTIONS ${CARBIN_TEST_COPTS}
        SHARED
)

file(COPY ${PROJECT_SOURCE_DIR}/test/json/jsonout DESTINATION ${PROJECT_BINARY_DIR}/bin)
carbin_cc_test(
        NAME cord_buf_test
        SOURCES io/cord_buf_test.cc
        PUBLIC_LINKED_TARGETS ${GTEST_MAIN_LIB} ${GTM_LIB} ${CARBIN_DYLINK} ${DYNAMIC_LIB} flare::flare-debug-test flare::testing
        PRIVATE_COMPILE_OPTIONS ${CARBIN_TEST_COPTS}
        VERBOSE
)

carbin_cc_test(
        NAME json_protobuf_test
        SOURCES json/json_protobuf_test.cc
        PUBLIC_LINKED_TARGETS ${GTEST_MAIN_LIB} ${GTM_LIB} ${CARBIN_DYLINK} ${DYNAMIC_LIB} flare::flare-debug-test flare::testing
        PRIVATE_COMPILE_OPTIONS ${CARBIN_TEST_COPTS}
        VERBOSE
)


add_subdirectory(metrics)
add_subdirectory(fiber)
add_subdirectory(base)
add_subdirectory(bootstrap)
add_subdirectory(container)
add_subdirectory(files)
add_subdirectory(future)
add_subdirectory(hash)
add_subdirectory(log)
add_subdirectory(memory)
add_subdirectory(strings)
add_subdirectory(io)
add_subdirectory(debugging)
add_subdirectory(times)
add_subdirectory(thread)
add_subdirectory(dag)

