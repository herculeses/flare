


file(GLOB_RECURSE FLARE_BASE_SRC "${PROJECT_SOURCE_DIR}/flare/base/*.cc")
file(GLOB_RECURSE FLARE_BASE_C_SRC "${PROJECT_SOURCE_DIR}/flare/base/*.c")
file(GLOB_RECURSE FLARE_CONTAINER_SRC "${PROJECT_SOURCE_DIR}/flare/container/*.cc")
file(GLOB_RECURSE FLARE_HASH_SRC "${PROJECT_SOURCE_DIR}/flare/hash/*.cc")
file(GLOB_RECURSE FLARE_MEMORY_SRC "${PROJECT_SOURCE_DIR}/flare/memory/*.cc")
file(GLOB_RECURSE FLARE_IO_SRC "${PROJECT_SOURCE_DIR}/flare/io/*.cc")
file(GLOB_RECURSE FLARE_STRINGS_SRC "${PROJECT_SOURCE_DIR}/flare/strings/*.cc")
file(GLOB_RECURSE FLARE_DEBUG_SRC "${PROJECT_SOURCE_DIR}/flare/debugging/*.cc")
file(GLOB_RECURSE FLARE_LOG_SRC "${PROJECT_SOURCE_DIR}/flare/log/*.cc")
file(GLOB_RECURSE FLARE_BOOTSTRAP_SRC "${PROJECT_SOURCE_DIR}/flare/bootstrap/*.cc")
file(GLOB_RECURSE FLARE_TIMES_SRC "${PROJECT_SOURCE_DIR}/flare/times/*.cc")
file(GLOB_RECURSE FLARE_FILES_SRC "${PROJECT_SOURCE_DIR}/flare/files/*.cc")
file(GLOB_RECURSE FLARE_THREAD_SRC "${PROJECT_SOURCE_DIR}/flare/thread/*.cc")
file(GLOB_RECURSE FLARE_FUTURE_SRC "${PROJECT_SOURCE_DIR}/flare/future/*.cc")
file(GLOB_RECURSE FLARE_DAG_SRC "${PROJECT_SOURCE_DIR}/flare/dag/*.cc")
file(GLOB_RECURSE FLARE_METRICS_SRC "${PROJECT_SOURCE_DIR}/flare/metrics/*.cc")
file(GLOB_RECURSE FLARE_FIBER_SRC "${PROJECT_SOURCE_DIR}/flare/fiber/*.cc")
file(GLOB_RECURSE FLARE_J2B_SRC "${PROJECT_SOURCE_DIR}/flare/json2pb/*.cc")

set(FLARE_SRC
        ${FLARE_BASE_C_SRC}
        ${FLARE_BASE_SRC}
        ${FLARE_CONTAINER_SRC}
        ${FLARE_HASH_SRC}
        ${FLARE_MEMORY_SRC}
        ${FLARE_IO_SRC}
        ${FLARE_STRINGS_SRC}
        ${FLARE_DEBUG_SRC}
        ${FLARE_LOG_SRC}
        ${FLARE_BOOTSTRAP_SRC}
        ${FLARE_HASH_SRC}
        ${FLARE_TIMES_SRC}
        ${FLARE_FILES_SRC}
        ${FLARE_THREAD_SRC}
        ${FLARE_FUTURE_SRC}
        ${FLARE_DAG_SRC}
        ${FLARE_METRICS_SRC}
        ${FLARE_FIBER_SRC}
        ${FLARE_J2B_SRC}
        )

carbin_cc_library(
        NAMESPACE flare
        NAME flare
        SOURCES ${FLARE_SRC}
        PUBLIC_LINKED_TARGETS ${CARBIN_DYLINK} ${DYNAMIC_LIB}
        PRIVATE_COMPILE_OPTIONS ${CARBIN_DEFAULT_COPTS}
        PUBLIC
        SHARED
)


install(DIRECTORY ${PROJECT_SOURCE_DIR}/flare
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.list"
        )

if (ENABLE_TESTING)

    carbin_cc_library(
            NAMESPACE flare
            NAME flare-debug-test
            SOURCES ${FLARE_SRC}
            PUBLIC_LINKED_TARGETS ${CARBIN_DYLINK} ${DYNAMIC_LIB}
            PRIVATE_COMPILE_OPTIONS ${CARBIN_TEST_COPTS}
            SHARED
    )

endif (ENABLE_TESTING)