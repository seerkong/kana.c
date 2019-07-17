# project
cmake_minimum_required(VERSION 3.13.0)
project(kon VERSION 0.1.0 LANGUAGES C CXX ASM)

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
add_executable(koni "")
set_target_properties(koni PROPERTIES OUTPUT_NAME "koni")
set_target_properties(koni PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(koni kon)
target_include_directories(koni PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(koni PRIVATE
    DEBUG
    __tb_prefix__="koni"
)
set_property(TARGET koni PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(koni PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET koni PROPERTY CXX_STANDARD 14)
target_compile_options(koni PRIVATE
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
    target_compile_options(koni PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(koni PRIVATE -O0)
endif()
target_link_libraries(koni PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(koni PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(koni PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(koni PRIVATE
    src/koni/main.c
)

# target
add_executable(playdatatypes "")
set_target_properties(playdatatypes PROPERTIES OUTPUT_NAME "playdatatypes")
set_target_properties(playdatatypes PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(playdatatypes kon)
target_include_directories(playdatatypes PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(playdatatypes PRIVATE
    DEBUG
    __tb_prefix__="playdatatypes"
)
set_property(TARGET playdatatypes PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(playdatatypes PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET playdatatypes PROPERTY CXX_STANDARD 14)
target_compile_options(playdatatypes PRIVATE
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
    target_compile_options(playdatatypes PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(playdatatypes PRIVATE -O0)
endif()
target_link_libraries(playdatatypes PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(playdatatypes PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(playdatatypes PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(playdatatypes PRIVATE
    src/playground/datatypes.cpp
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
add_executable(playreader "")
set_target_properties(playreader PROPERTIES OUTPUT_NAME "playreader")
set_target_properties(playreader PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(playreader kon)
target_include_directories(playreader PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(playreader PRIVATE
    DEBUG
    __tb_prefix__="reader"
)
set_property(TARGET playreader PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(playreader PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET playreader PROPERTY CXX_STANDARD 14)
target_compile_options(playreader PRIVATE
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
    target_compile_options(playreader PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(playreader PRIVATE -O0)
endif()
target_link_libraries(playreader PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(playreader PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(playreader PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(playreader PRIVATE
    src/playground/kson/reader.c
)

# target
add_executable(eval_file "")
set_target_properties(eval_file PROPERTIES OUTPUT_NAME "eval_file")
set_target_properties(eval_file PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(eval_file kon)
target_include_directories(eval_file PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(eval_file PRIVATE
    DEBUG
    __tb_prefix__="eval_file"
)
set_property(TARGET eval_file PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(eval_file PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET eval_file PROPERTY CXX_STANDARD 14)
target_compile_options(eval_file PRIVATE
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
    target_compile_options(eval_file PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(eval_file PRIVATE -O0)
endif()
target_link_libraries(eval_file PRIVATE
    tbox
    pthread
    kon
)
target_link_directories(eval_file PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(eval_file PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(eval_file PRIVATE
    src/playground/eval_file.c
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
    src/kon/interpreter/keyword/prefix_or.c
    src/kon/kon.c
    src/kon/interpreter/keyword/prefix_let.c
    src/kon/interpreter/env.c
    src/kon/interpreter/primary.c
    src/kon/interpreter/keyword/prefix_cond.c
    src/kon/kson/state_stack.c
    src/kon/interpreter/keyword/prefix_func.c
    src/kon/interpreter/keyword/prefix_blk.c
    src/kon/kson/writer.c
    src/kon/interpreter/keyword/prefix_set.c
    src/kon/interpreter/keyword/prefix_do.c
    src/kon/interpreter/keyword/prefix_lambda.c
    src/kon/interpreter/keyword/prefix_if.c
    src/kon/kson/murmurhash.c
    src/kon/kson/node.c
    src/kon/utils/number_utils.c
    src/kon/kson/hashmap.c
    src/kon/kson/tokenizer.c
    src/kon/interpreter/keyword/prefix_and.c
    src/kon/kson/value_builder.c
    src/kon/interpreter/cps_interpreter.c
    src/kon/kson/reader.c
)

