# GENERATED! DO NOT MANUALLY EDIT THIS FILE.
#
# (1) Edit abel/copts/copts.py.
# (2) Run `python <path_to_abel>/copts/generate_copts.py`.

list(APPEND CARBIN_GCC_FLAGS
        "-Wall"
        "-Wextra"
        "-Wno-cast-qual"
        "-Wconversion-null"
        "-Wformat-security"
        "-Woverlength-strings"
        "-Wpointer-arith"
        "-Wno-undef"
        "-Wunused-local-typedefs"
        "-Wunused-result"
        "-Wvarargs"
        "-Wno-attributes"
        "-Wno-implicit-fallthrough"
        "-Wno-unused-parameter"
        "-Wno-unused-function"
        "-Wwrite-strings"
        "-Wno-class-memaccess"
        "-Wno-sign-compare"
        "-DNOMINMAX"
        "-D__CONST__="
        )

list(APPEND CARBIN_GCC_TEST_FLAGS
        "-Wno-conversion-null"
        "-Wno-deprecated-declarations"
        "-Wno-missing-declarations"
        "-Wno-sign-compare"
        "-Wno-undef"
        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-private-field"
        "-DUNIT_TEST"
        "-Dprivate=public"
        "-Dprotected=public"
        "-Wno-invalid-offsetof"
        "-Wno-unused-parameter"
        "-fno-omit-frame-pointer"
        "-g"
        "-O0"
        )

list(APPEND CARBIN_LLVM_FLAGS
        "-Wall"
        "-Wextra"
        "-Wno-cast-qual"
        "-Wno-conversion"
        "-Wno-sign-compare"
        "-Wfloat-overflow-conversion"
        "-Wfloat-zero-conversion"
        "-Wfor-loop-analysis"
        "-Wformat-security"
        "-Wgnu-redeclared-enum"
        "-Winfinite-recursion"
        "-Wliteral-conversion"
        "-Wmissing-declarations"
        "-Woverlength-strings"
        "-Wpointer-arith"
        "-Wself-assign"
        "-Wno-shadow"
        "-Wstring-conversion"
        "-Wtautological-overlap-compare"
        "-Wno-undef"
        "-Wuninitialized"
        "-Wunreachable-code"
        "-Wunused-comparison"
        "-Wunused-local-typedefs"
        "-Wunused-result"
        "-Wno-vla"
        "-D__CONST__="
        "-Wwrite-strings"
        "-Wno-float-conversion"
        "-Wno-implicit-float-conversion"
        "-Wno-implicit-int-float-conversion"
        "-Wno-implicit-int-conversion"
        "-Wno-shorten-64-to-32"
        "-Wno-sign-conversion"
        "-Wno-unused-parameter"
        "-Wno-unused-function"
        "-DNOMINMAX"
        )

list(APPEND CARBIN_LLVM_TEST_FLAGS
        "-Wno-c99-extensions"
        "-Wno-deprecated-declarations"
        "-Wno-missing-noreturn"
        "-Wno-missing-prototypes"
        "-Wno-missing-variable-declarations"
        "-Wno-null-conversion"
        "-Wno-shadow"
        "-Wno-undef"
        "-Wno-shift-sign-overflow"
        "-Wno-sign-compare"
        "-Wno-unused-function"
        "-Wno-unused-member-function"
        "-Wno-unused-parameter"
        "-Wno-unused-private-field"
        "-Wno-unused-template"
        "-Wno-used-but-marked-unused"
        "-Wno-zero-as-null-pointer-constant"
        "-Wno-gnu-zero-variadic-macro-arguments"
        "-DUNIT_TEST"
        "-Dprivate=public"
        "-Dprotected=public"
        "-g"
        "-O0"
        "-Wno-invalid-offsetof"
        "-Wno-unused-parameter"
        "-fno-omit-frame-pointer"
        )

list(APPEND CARBIN_RANDOM_HWAES_ARM32_FLAGS
        "-mfpu=neon"
        )

list(APPEND CARBIN_RANDOM_HWAES_ARM64_FLAGS
        "-march=armv8-a+crypto"
        )

list(APPEND CARBIN_RANDOM_HWAES_MSVC_X64_FLAGS
        )

list(APPEND CARBIN_RANDOM_HWAES_X64_FLAGS
        "-maes"
        "-msse4.1"
        )
