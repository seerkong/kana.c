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
add_executable(testreader "")
set_target_properties(testreader PROPERTIES OUTPUT_NAME "testreader")
set_target_properties(testreader PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(testreader kon)
target_include_directories(testreader PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(testreader PRIVATE
    DEBUG
)
set_property(TARGET testreader PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(testreader PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET testreader PROPERTY CXX_STANDARD 14)
target_compile_options(testreader PRIVATE
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
    target_compile_options(testreader PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(testreader PRIVATE -O0)
endif()
target_link_libraries(testreader PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(testreader PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(testreader PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(testreader PRIVATE
    test/kson/testreader.c
)

# target
add_executable(playwriter "")
set_target_properties(playwriter PROPERTIES OUTPUT_NAME "playwriter")
set_target_properties(playwriter PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(playwriter kon)
target_include_directories(playwriter PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(playwriter PRIVATE
    DEBUG
    __tb_prefix__="playwriter"
)
set_property(TARGET playwriter PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(playwriter PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET playwriter PROPERTY CXX_STANDARD 14)
target_compile_options(playwriter PRIVATE
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
    target_compile_options(playwriter PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(playwriter PRIVATE -O0)
endif()
target_link_libraries(playwriter PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(playwriter PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(playwriter PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(playwriter PRIVATE
    src/playground/kson/writer.c
)

# target
add_executable(playtokenizer "")
set_target_properties(playtokenizer PROPERTIES OUTPUT_NAME "playtokenizer")
set_target_properties(playtokenizer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(playtokenizer kon)
target_include_directories(playtokenizer PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(playtokenizer PRIVATE
    DEBUG
    __tb_prefix__="tokenizer"
)
set_property(TARGET playtokenizer PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(playtokenizer PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET playtokenizer PROPERTY CXX_STANDARD 14)
target_compile_options(playtokenizer PRIVATE
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
    target_compile_options(playtokenizer PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(playtokenizer PRIVATE -O0)
endif()
target_link_libraries(playtokenizer PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(playtokenizer PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(playtokenizer PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(playtokenizer PRIVATE
    src/playground/kson/tokenizer.c
)

# target
add_executable(testeval "")
set_target_properties(testeval PROPERTIES OUTPUT_NAME "testeval")
set_target_properties(testeval PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(testeval kon)
target_include_directories(testeval PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(testeval PRIVATE
    DEBUG
)
set_property(TARGET testeval PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(testeval PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET testeval PROPERTY CXX_STANDARD 14)
target_compile_options(testeval PRIVATE
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
    target_compile_options(testeval PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(testeval PRIVATE -O0)
endif()
target_link_libraries(testeval PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(testeval PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(testeval PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(testeval PRIVATE
    test/klang/testeval.c
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
    __tb_debug__
    __tb_prefix__="kon"
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
    src/kon/interpreter/keyword/prefix_or.c
    src/kon/interpreter/keyword/prefix_break.c
    src/kon/interpreter/keyword/prefix_set.c
    src/kon/interpreter/env.c
    src/kon/container/hashtable/kx_hashtable.c
    src/kon/interpreter/primary.c
    src/kon/interpreter/keyword/prefix_cond.c
    src/kon/kson/state_stack.c
    src/kon/interpreter/keyword/prefix_func.c
    src/kon/interpreter/mod/kon_string.c
    src/kon/interpreter/mod/kon_vector.c
    src/kon/interpreter/mod/kon_module.c
    src/kon/interpreter/mod/kon_list.c
    src/kon/interpreter/keyword/prefix_let.c
    src/kon/interpreter/mod/kon_cell.c
    src/kon/kson/tokenizer.c
    src/kon/interpreter/keyword/prefix_if.c
    src/kon/utils/number_utils.c
    src/kon/kson/writer.c
    src/kon/interpreter/keyword/prefix_def_dispatcher.c
    src/kon/string/kx_stringview.c
    src/kon/kson/reader.c
    src/kon/interpreter/keyword/prefix_and.c
    src/kon/interpreter/keyword/prefix_sh.c
    src/kon/container/hashtable/murmurhash.c
    src/kon/interpreter/keyword/prefix_for.c
)

