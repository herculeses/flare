
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
set(TEST_PROTO_FILES
        cord_buf.proto)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/hdrs)
set(PROTOC_FLAGS ${PROTOC_FLAGS} -I${CMAKE_SOURCE_DIR}/test -I${CMAKE_SOURCE_DIR})
compile_proto(PROTO_HDRS PROTO_SRCS ${CMAKE_BINARY_DIR}/test
        ${CMAKE_BINARY_DIR}/test/hdrs
        ${CMAKE_SOURCE_DIR}/test
        "${TEST_PROTO_FILES}")
add_library(TEST_PROTO_LIB OBJECT ${PROTO_SRCS} ${PROTO_HDRS})

include(require_gtest)
include(require_gmock)

set(IO_TEST_SRC
        ${PROJECT_SOURCE_DIR}/test/io/cord_buf_test.cc
        )


# fiber_* functions are used in logging.cc, and they need to be marked as
# weak symbols explicitly in Darwin system.
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(DYNAMIC_LIB ${DYNAMIC_LIB}
            "-Wl,-U,_fiber_getspecific"
            "-Wl,-U,_fiber_setspecific"
            "-Wl,-U,_fiber_key_create")
endif ()

file(GLOB_RECURSE TESTING_SOURCES "${PROJECT_SOURCE_DIR}/test/testing/*.cc")
carbin_cc_library(
        NAMESPACE flare
        NAME testing
        SOURCES ${TESTING_SOURCES}
        PUBLIC_LINKED_TARGETS ${CARBIN_DYLINK} ${DYNAMIC_LIB} flare::flare-debug-test
        PRIVATE_COMPILE_OPTIONS ${CARBIN_TEST_COPTS}
        SHARED
)
# test_io
carbin_cc_test(
        NAME test_io
        SOURCES io/cord_buf_test.cc ${CMAKE_CURRENT_BINARY_DIR}/cord_buf.pb.cc
        PUBLIC_LINKED_TARGETS ${GTEST_MAIN_LIB}  ${GTM_LIB} ${CARBIN_DYLINK} ${DYNAMIC_LIB} flare::flare-debug-test flare::testing
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

