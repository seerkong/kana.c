# project
cmake_minimum_required(VERSION 3.13.0)
project(kon VERSION 0.1.0 LANGUAGES C CXX ASM)

# target
add_executable(kli "")
set_target_properties(kli PROPERTIES OUTPUT_NAME "kli")
set_target_properties(kli PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(kli kon)
target_include_directories(kli PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(kli PRIVATE
    DEBUG
)
set_property(TARGET kli PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(kli PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET kli PROPERTY CXX_STANDARD 14)
target_compile_options(kli PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-function>
)
if(MSVC)
    target_compile_options(kli PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(kli PRIVATE -O0)
endif()
target_link_libraries(kli PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(kli PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(kli PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(kli PRIVATE
    src/kli/main.c
    src/kli/commander.c
)

# target
add_executable(test_eval "")
set_target_properties(test_eval PROPERTIES OUTPUT_NAME "test_eval")
set_target_properties(test_eval PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(test_eval kon)
target_include_directories(test_eval PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(test_eval PRIVATE
    DEBUG
)
set_property(TARGET test_eval PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(test_eval PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET test_eval PROPERTY CXX_STANDARD 14)
target_compile_options(test_eval PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-function>
)
if(MSVC)
    target_compile_options(test_eval PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(test_eval PRIVATE -O0)
endif()
target_link_libraries(test_eval PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(test_eval PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(test_eval PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(test_eval PRIVATE
    test/klang/test_eval.c
)

# target
add_executable(test_tokenizer "")
set_target_properties(test_tokenizer PROPERTIES OUTPUT_NAME "test_tokenizer")
set_target_properties(test_tokenizer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(test_tokenizer kon)
target_include_directories(test_tokenizer PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(test_tokenizer PRIVATE
    DEBUG
)
set_property(TARGET test_tokenizer PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(test_tokenizer PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET test_tokenizer PROPERTY CXX_STANDARD 14)
target_compile_options(test_tokenizer PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-function>
)
if(MSVC)
    target_compile_options(test_tokenizer PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(test_tokenizer PRIVATE -O0)
endif()
target_link_libraries(test_tokenizer PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(test_tokenizer PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(test_tokenizer PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(test_tokenizer PRIVATE
    test/kson/test_tokenizer.c
)

# target
add_executable(test_reader "")
set_target_properties(test_reader PROPERTIES OUTPUT_NAME "test_reader")
set_target_properties(test_reader PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(test_reader kon)
target_include_directories(test_reader PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(test_reader PRIVATE
    DEBUG
)
set_property(TARGET test_reader PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(test_reader PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET test_reader PROPERTY CXX_STANDARD 14)
target_compile_options(test_reader PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-function>
)
if(MSVC)
    target_compile_options(test_reader PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(test_reader PRIVATE -O0)
endif()
target_link_libraries(test_reader PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(test_reader PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(test_reader PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(test_reader PRIVATE
    test/kson/test_reader.c
)

# target
add_executable(test_table "")
set_target_properties(test_table PROPERTIES OUTPUT_NAME "test_table")
set_target_properties(test_table PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(test_table kon)
target_include_directories(test_table PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(test_table PRIVATE
    DEBUG
)
set_property(TARGET test_table PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(test_table PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET test_table PROPERTY CXX_STANDARD 14)
target_compile_options(test_table PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-function>
)
if(MSVC)
    target_compile_options(test_table PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(test_table PRIVATE -O0)
endif()
target_link_libraries(test_table PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(test_table PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(test_table PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(test_table PRIVATE
    test/container/test_table.c
    test/container/test_hashtable.c
)

# target
add_library(kon STATIC "")
set_target_properties(kon PROPERTIES OUTPUT_NAME "kon")
set_target_properties(kon PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
target_include_directories(kon PRIVATE
    /usr/include
    /usr/local/include
    src
)
target_include_directories(kon INTERFACE
    src
)
target_compile_definitions(kon PRIVATE
    DEBUG
    __debug__
    __kon_prefix__="kon"
)
set_property(TARGET kon PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(kon PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET kon PROPERTY CXX_STANDARD 14)
target_compile_options(kon PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-function>
)
if(MSVC)
    target_compile_options(kon PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(kon PRIVATE -O0)
endif()
target_link_libraries(kon PRIVATE
    tbox
    pthread
)
target_link_directories(kon PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(kon PRIVATE
)
target_sources(kon PRIVATE
    src/kon/interpreter/keyword/prefix_eval.c
    src/kon/interpreter/keyword/prefix_callcc.c
    src/kon/container/vector/kx_vector.c
    src/kon/kon.c
    src/kon/interpreter/keyword/prefix_do.c
    src/kon/kson/node.c
    src/kon/string/kx_stringbuffer.c
    src/kon/interpreter/keyword/prefix_blk.c
    src/kon/interpreter/mod/kon_table.c
    src/kon/interpreter/keyword/prefix_def_builder.c
    src/kon/interpreter/mod/kon_number.c
    src/kon/kson/value_builder.c
    src/kon/interpreter/keyword/prefix_lambda.c
    src/kon/interpreter/keyword/prefix_continue.c
    src/kon/interpreter/cps_interpreter.c
    src/kon/interpreter/keyword/prefix_apply.c
    src/kon/interpreter/keyword/prefix_or.c
    src/kon/interpreter/keyword/prefix_break.c
    src/kon/interpreter/keyword/prefix_let.c
    src/kon/interpreter/env.c
    src/kon/kson/writer.c
    src/kon/interpreter/primary.c
    src/kon/interpreter/keyword/prefix_cond.c
    src/kon/kson/state_stack.c
    src/kon/interpreter/keyword/prefix_func.c
    src/kon/interpreter/mod/kon_string.c
    src/kon/interpreter/mod/kon_vector.c
    src/kon/interpreter/mod/kon_module.c
    src/kon/interpreter/mod/kon_list.c
    src/kon/interpreter/keyword/prefix_set.c
    src/kon/interpreter/mod/kon_cell.c
    src/kon/container/hashtable/kx_hashtable.c
    src/kon/interpreter/keyword/prefix_if.c
    src/kon/kson/reader.c
    src/kon/utils/number_utils.c
    src/kon/interpreter/keyword/prefix_def_dispatcher.c
    src/kon/kson/tokenizer.c
    src/kon/string/kx_stringview.c
    src/kon/interpreter/keyword/prefix_and.c
    src/kon/interpreter/keyword/prefix_sh.c
    src/kon/container/hashtable/murmurhash.c
    src/kon/interpreter/keyword/prefix_for.c
)

