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
    tbox
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
    tbox
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
add_executable(test_list "")
set_target_properties(test_list PROPERTIES OUTPUT_NAME "test_list")
set_target_properties(test_list PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(test_list kon)
target_include_directories(test_list PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(test_list PRIVATE
    DEBUG
)
set_property(TARGET test_list PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(test_list PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET test_list PROPERTY CXX_STANDARD 14)
target_compile_options(test_list PRIVATE
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
    target_compile_options(test_list PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(test_list PRIVATE -O0)
endif()
target_link_libraries(test_list PRIVATE
    tbox
    pthread
    tbox
    kon
)
target_link_directories(test_list PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(test_list PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(test_list PRIVATE
    test/container/test_list.c
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
    tbox
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
add_executable(test_string "")
set_target_properties(test_string PROPERTIES OUTPUT_NAME "test_string")
set_target_properties(test_string PROPERTIES RUNTIME_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
add_dependencies(test_string kon)
target_include_directories(test_string PRIVATE
    /usr/include
    /usr/local/include
)
target_compile_definitions(test_string PRIVATE
    DEBUG
)
set_property(TARGET test_string PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(test_string PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET test_string PROPERTY CXX_STANDARD 14)
target_compile_options(test_string PRIVATE
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
    target_compile_options(test_string PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(test_string PRIVATE -O0)
endif()
target_link_libraries(test_string PRIVATE
    tbox
    pthread
    tbox
    kon
)
target_link_directories(test_string PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(test_string PRIVATE
    -L/usr/local/lib
    -lpthread
)
target_sources(test_string PRIVATE
    test/string/test_string.c
)

# target
add_library(tbox STATIC "")
set_target_properties(tbox PROPERTIES OUTPUT_NAME "tbox")
set_target_properties(tbox PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "build/macosx/x86_64/release")
target_include_directories(tbox PRIVATE
    /usr/include
    /usr/local/include
    src
    build/macosx/x86_64/release
)
target_include_directories(tbox INTERFACE
    src
    build/macosx/x86_64/release
)
target_compile_definitions(tbox PRIVATE
    DEBUG
    __tb_prefix__="tbox"
)
set_property(TARGET tbox PROPERTY C_STANDARD 99)
if(MSVC)
    target_compile_options(tbox PRIVATE $<$<COMPILE_LANGUAGE:C>:-TP>)
endif()
set_property(TARGET tbox PROPERTY CXX_STANDARD 14)
target_compile_options(tbox PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-declarations>
    $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=expansion-to-defined>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=expansion-to-defined>
)
if(MSVC)
    target_compile_options(tbox PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(tbox PRIVATE -O0)
endif()
target_link_libraries(tbox PRIVATE
    pthread
    pthread
)
target_link_directories(tbox PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(tbox PRIVATE
)
target_sources(tbox PRIVATE
    src/tbox/string/static_string.c
    src/tbox/platform/sched.c
    src/tbox/stream/stream.c
    src/tbox/container/array_iterator.c
    src/tbox/utils/base32.c
    src/tbox/algorithm/heap_sort.c
    src/tbox/libc/stdio/sprintf.c
    src/tbox/libc/string/strcat.c
    src/tbox/platform/semaphore.c
    src/tbox/memory/impl/memory.c
    src/tbox/network/impl/network.c
    src/tbox/hash/fnv32.c
    src/tbox/container/element/uint16.c
    src/tbox/libc/stdio/printf.c
    src/tbox/platform/addrinfo.c
    src/tbox/algorithm/quick_sort.c
    src/tbox/libc/stdio/puts.c
    src/tbox/platform/memory.c
    src/tbox/memory/impl/static_fixed_pool.c
    src/tbox/container/queue.c
    src/tbox/utils/dump.c
    src/tbox/container/bloom_filter.c
    src/tbox/utils/lock_profiler.c
    src/tbox/libc/stdio/wputs.c
    src/tbox/utils/url.c
    src/tbox/algorithm/remove_if.c
    src/tbox/memory/default_allocator.c
    src/tbox/math/int32.c
    src/tbox/algorithm/remove.c
    src/tbox/libc/stdlib/random.c
    src/tbox/platform/impl/sockdata.c
    src/tbox/platform/cache_time.c
    src/tbox/platform/impl/dns.c
    src/tbox/libc/stdlib/stdlib.c
    src/tbox/platform/dynamic.c
    src/tbox/platform/syserror.c
    src/tbox/platform/environment.c
    src/tbox/libc/string/strncat.c
    src/tbox/platform/thread_pool.c
    src/tbox/platform/impl/platform.c
    src/tbox/platform/thread_local.c
    src/tbox/network/impl/http/status.c
    src/tbox/container/iterator.c
    src/tbox/math/fixed16.c
    src/tbox/libc/string/strncpy.c
    src/tbox/memory/impl/native_large_allocator.c
    src/tbox/libc/impl/libc.c
    src/tbox/platform/timer.c
    src/tbox/platform/ltimer.c
    src/tbox/libc/string/strlen.c
    src/tbox/network/hwaddr.c
    src/tbox/platform/backtrace.c
    src/tbox/platform/event.c
    src/tbox/hash/bkdr.c
    src/tbox/platform/thread.c
    src/tbox/libc/string/strdup.c
    src/tbox/platform/time.c
    src/tbox/memory/impl/static_large_allocator.c
    src/tbox/platform/page.c
    src/tbox/platform/mutex.c
    src/tbox/platform/poller.c
    src/tbox/platform/socket.c
    src/tbox/algorithm/walk.c
    src/tbox/platform/atomic64.c
    src/tbox/string/string.c
    src/tbox/platform/processor.c
    src/tbox/memory/fixed_pool.c
    src/tbox/platform/hostname.c
    src/tbox/algorithm/sort.c
    src/tbox/network/url.c
    src/tbox/stream/static_stream.c
    src/tbox/hash/adler32.c
    src/tbox/platform/process.c
    src/tbox/network/http.c
    src/tbox/libc/string/wcslen.c
    src/tbox/platform/directory.c
    src/tbox/libc/string/strcpy.c
    src/tbox/algorithm/remove_first_if.c
    src/tbox/container/hash_map.c
    src/tbox/libm/ilog2i.c
    src/tbox/libm/idivi8.c
    src/tbox/stream/impl/filter/cache.c
    src/tbox/memory/static_buffer.c
    src/tbox/network/impl/http/option.c
    src/tbox/container/priority_queue.c
    src/tbox/libc/string/stricmp.c
    src/tbox/container/circle_queue.c
    src/tbox/algorithm/rwalk.c
    src/tbox/container/hash_set.c
    src/tbox/container/element/hash.c
    src/tbox/platform/file.c
    src/tbox/memory/allocator.c
    src/tbox/libc/misc/time/gmtime.c
    src/tbox/stream/impl/stream/filter.c
    src/tbox/container/element/uint32.c
    src/tbox/stream/transfer.c
    src/tbox/container/element/size.c
    src/tbox/libc/string/stristr.c
    src/tbox/algorithm/binary_find.c
    src/tbox/container/stack.c
    src/tbox/tbox.c
    src/tbox/container/element/str.c
    src/tbox/container/element/mem.c
    src/tbox/container/element/ptr.c
    src/tbox/libc/string/memcmp.c
    src/tbox/utils/bits.c
    src/tbox/libc/string/wcsichr.c
    src/tbox/container/single_list.c
    src/tbox/utils/trace.c
    src/tbox/libc/string/strichr.c
    src/tbox/libc/string/wcsrstr.c
    src/tbox/container/element/true.c
    src/tbox/libc/string/strnstr.c
    src/tbox/container/single_list_entry.c
    src/tbox/libc/string/strnrstr.c
    src/tbox/container/list.c
    src/tbox/memory/buffer.c
    src/tbox/network/dns/cache.c
    src/tbox/libc/string/wcsnrchr.c
    src/tbox/algorithm/remove_first.c
    src/tbox/utils/used.c
    src/tbox/libm/impl/libm.c
    src/tbox/utils/base64.c
    src/tbox/stream/filter.c
    src/tbox/libc/stdlib/mbstowcs.c
    src/tbox/libc/stdio/swprintf.c
    src/tbox/algorithm/count.c
    src/tbox/algorithm/predicate.c
    src/tbox/math/random/random.c
    src/tbox/libc/stdio/vswprintf.c
    src/tbox/libc/string/wcsnirchr.c
    src/tbox/math/impl/math.c
    src/tbox/platform/ifaddrs.c
    src/tbox/algorithm/insert_sort.c
    src/tbox/libm/isqrti.c
    src/tbox/platform/print.c
    src/tbox/libc/string/wcsicmp.c
    src/tbox/memory/queue_buffer.c
    src/tbox/algorithm/rfind_if.c
    src/tbox/container/heap.c
    src/tbox/container/list_entry.c
    src/tbox/libc/string/memmov.c
    src/tbox/network/ipaddr.c
    src/tbox/libc/misc/time/time.c
    src/tbox/network/ipv4.c
    src/tbox/libc/string/strchr.c
    src/tbox/algorithm/find_if.c
    src/tbox/network/dns/server.c
    src/tbox/network/dns/looker.c
    src/tbox/libc/string/wcsirstr.c
    src/tbox/libc/string/strnirstr.c
    src/tbox/network/impl/http/method.c
    src/tbox/libc/misc/time/gmmktime.c
    src/tbox/libc/string/strnicmp.c
    src/tbox/algorithm/distance.c
    src/tbox/libc/string/memcpy.c
    src/tbox/libc/string/memmem.c
    src/tbox/libc/string/wcsnlen.c
    src/tbox/libc/string/strrchr.c
    src/tbox/libc/string/wcscat.c
    src/tbox/network/cookies.c
    src/tbox/network/ipv6.c
    src/tbox/stream/impl/stream/file.c
    src/tbox/prefix/state.c
    src/tbox/stream/impl/stream/http.c
    src/tbox/network/impl/http/date.c
    src/tbox/algorithm/count_if.c
    src/tbox/container/element/long.c
    src/tbox/libc/misc/time/mktime.c
    src/tbox/stream/impl/filter/chunked.c
    src/tbox/container/element/uint8.c
    src/tbox/libc/string/strncmp.c
    src/tbox/libc/misc/time/localtime.c
    src/tbox/libc/stdio/printf_object.c
    src/tbox/memory/string_pool.c
    src/tbox/memory/native_allocator.c
    src/tbox/libc/string/memdup.c
    src/tbox/libc/string/wcsstr.c
    src/tbox/libc/string/strlcpy.c
    src/tbox/libc/string/wcsnirstr.c
    src/tbox/libc/string/wcsdup.c
    src/tbox/libc/string/wcsncat.c
    src/tbox/algorithm/bubble_sort.c
    src/tbox/libc/string/memset.c
    src/tbox/math/random/linear.c
    src/tbox/libc/string/wcscmp.c
    src/tbox/platform/path.c
    src/tbox/libc/string/strirchr.c
    src/tbox/libm/isqrti64.c
    src/tbox/memory/large_allocator.c
    src/tbox/libc/string/strndup.c
    src/tbox/libc/stdio/wprintf.c
    src/tbox/libc/stdio/vsnprintf.c
    src/tbox/libc/string/strnlen.c
    src/tbox/libc/string/strnirchr.c
    src/tbox/libc/string/wcsndup.c
    src/tbox/libc/string/strrstr.c
    src/tbox/libc/string/strcmp.c
    src/tbox/libc/string/wcschr.c
    src/tbox/libc/string/wcsrchr.c
    src/tbox/memory/small_allocator.c
    src/tbox/libc/string/strnrchr.c
    src/tbox/libc/string/wcsncmp.c
    src/tbox/libc/string/wcsirchr.c
    src/tbox/memory/impl/prefix.c
    src/tbox/algorithm/rfind.c
    src/tbox/algorithm/find.c
    src/tbox/algorithm/binary_find_if.c
    src/tbox/libc/string/wcsncpy.c
    src/tbox/libc/string/strstr.c
    src/tbox/libc/string/wcsnrstr.c
    src/tbox/utils/singleton.c
    src/tbox/libc/string/wcscpy.c
    src/tbox/container/vector.c
    src/tbox/libc/string/wcslcpy.c
    src/tbox/libc/stdlib/wcstombs.c
    src/tbox/libc/string/wcsistr.c
    src/tbox/libc/string/wcsnicmp.c
    src/tbox/container/element/null.c
    src/tbox/libc/stdio/snprintf.c
    src/tbox/stream/impl/stream/data.c
    src/tbox/libc/string/strnistr.c
    src/tbox/stream/impl/stream/sock.c
    src/tbox/libc/string/strirstr.c
    src/tbox/memory/static_allocator.c
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
    tbox
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
add_dependencies(kon tbox)
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
target_link_directories(kon PRIVATE
    /usr/local/lib
    /usr/lib
)
target_link_options(kon PRIVATE
)
target_sources(kon PRIVATE
    src/kon/interpreter/keyword/prefix_eval.c
    src/kon/interpreter/keyword/prefix_callcc.c
    src/kon/kon.c
    src/kon/interpreter/keyword/prefix_do.c
    src/kon/kson/node.c
    src/kon/string/kx_stringbuffer.c
    src/kon/interpreter/keyword/prefix_blk.c
    src/kon/interpreter/mod/kon_table.c
    src/kon/kson/writer.c
    src/kon/interpreter/mod/kon_number.c
    src/kon/container/murmurhash.c
    src/kon/utils/number_utils.c
    src/kon/interpreter/keyword/prefix_lambda.c
    src/kon/interpreter/keyword/prefix_continue.c
    src/kon/interpreter/cps_interpreter.c
    src/kon/container/kx_vector.c
    src/kon/interpreter/keyword/prefix_set_dispatcher.c
    src/kon/interpreter/keyword/prefix_or.c
    src/kon/interpreter/keyword/prefix_break.c
    src/kon/interpreter/mod/kon_string.c
    src/kon/interpreter/mod/kon_list.c
    src/kon/interpreter/keyword/prefix_let.c
    src/kon/interpreter/mod/kon_module.c
    src/kon/interpreter/env.c
    src/kon/container/kx_hashtable.c
    src/kon/kson/tokenizer.c
    src/kon/interpreter/keyword/prefix_cond.c
    src/kon/kson/state_stack.c
    src/kon/interpreter/keyword/prefix_func.c
    src/kon/interpreter/mod/kon_vector.c
    src/kon/container/kx_list.c
    src/kon/utils/realpath.c
    src/kon/interpreter/keyword/prefix_apply.c
    src/kon/interpreter/keyword/prefix_set.c
    src/kon/interpreter/mod/kon_cell.c
    src/kon/gc.c
    src/kon/interpreter/keyword/prefix_if.c
    src/kon/kson/reader.c
    src/kon/interpreter/primary.c
    src/kon/interpreter/keyword/prefix_def_dispatcher.c
    src/kon/string/kx_stringview.c
    src/kon/kson/value_builder.c
    src/kon/interpreter/keyword/prefix_and.c
    src/kon/interpreter/keyword/prefix_sh.c
    src/kon/interpreter/keyword/prefix_def_builder.c
    src/kon/interpreter/keyword/prefix_for.c
)

