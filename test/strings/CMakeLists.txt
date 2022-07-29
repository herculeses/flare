

# Copyright (c) 2021, gottingen group.
# All rights reserved.
# Created by liyinbin lijippy@163.com

file(GLOB SRC "*.cc")

foreach (fl ${SRC})

    string(REGEX REPLACE ".+/(.+)\\.cc$" "\\1" TEST_NAME ${fl})
    get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    string(REPLACE " " "_" DIR_NAME ${DIR_NAME})

    set(EXE_NAME ${DIR_NAME}_${TEST_NAME})
    carbin_cc_test(
            NAME ${EXE_NAME}
            SOURCES ${fl}
            PUBLIC_LINKED_TARGETS ${GTEST_MAIN_LIB}  ${GTM_LIB} ${CARBIN_DYLINK} ${DYNAMIC_LIB} flare::testing flare::flare-debug-test
            PRIVATE_COMPILE_OPTIONS ${CARBIN_TEST_COPTS}
            VERBOSE
    )
endforeach (fl ${SRC})