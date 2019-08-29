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
    exec/main.c
    exec/commander.c
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
    lib
    build/macosx/x86_64/release
)
target_include_directories(tbox INTERFACE
    lib
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
    lib/tbox/libc/stdlib/wcstombs.c
    lib/tbox/stream/impl/stream/file.c
    lib/tbox/libc/string/wcsdup.c
    lib/tbox/hash/bkdr.c
    lib/tbox/math/int32.c
    lib/tbox/platform/socket.c
    lib/tbox/network/impl/http/option.c
    lib/tbox/container/single_list_entry.c
    lib/tbox/stream/impl/filter/chunked.c
    lib/tbox/math/random/linear.c
    lib/tbox/platform/semaphore.c
    lib/tbox/algorithm/heap_sort.c
    lib/tbox/memory/allocator.c
    lib/tbox/libc/stdio/vswprintf.c
    lib/tbox/container/priority_queue.c
    lib/tbox/algorithm/distance.c
    lib/tbox/container/vector.c
    lib/tbox/libc/string/strncpy.c
    lib/tbox/utils/base32.c
    lib/tbox/container/hash_map.c
    lib/tbox/libc/string/memcpy.c
    lib/tbox/utils/base64.c
    lib/tbox/libc/stdlib/stdlib.c
    lib/tbox/network/impl/http/status.c
    lib/tbox/algorithm/remove_if.c
    lib/tbox/memory/native_allocator.c
    lib/tbox/container/element/null.c
    lib/tbox/memory/impl/memory.c
    lib/tbox/network/impl/http/date.c
    lib/tbox/string/string.c
    lib/tbox/platform/directory.c
    lib/tbox/libc/string/wcsnirstr.c
    lib/tbox/platform/hostname.c
    lib/tbox/container/stack.c
    lib/tbox/utils/url.c
    lib/tbox/platform/timer.c
    lib/tbox/libc/stdlib/random.c
    lib/tbox/platform/page.c
    lib/tbox/libc/stdio/sprintf.c
    lib/tbox/platform/mutex.c
    lib/tbox/stream/stream.c
    lib/tbox/memory/large_allocator.c
    lib/tbox/memory/impl/static_fixed_pool.c
    lib/tbox/libc/string/wcsnrstr.c
    lib/tbox/platform/syserror.c
    lib/tbox/platform/environment.c
    lib/tbox/stream/transfer.c
    lib/tbox/platform/thread_pool.c
    lib/tbox/platform/cache_time.c
    lib/tbox/platform/thread_local.c
    lib/tbox/platform/file.c
    lib/tbox/algorithm/remove_first_if.c
    lib/tbox/libc/string/wcsnlen.c
    lib/tbox/libc/string/wcsnicmp.c
    lib/tbox/platform/event.c
    lib/tbox/platform/time.c
    lib/tbox/platform/addrinfo.c
    lib/tbox/platform/impl/platform.c
    lib/tbox/libc/string/wcslen.c
    lib/tbox/stream/impl/stream/data.c
    lib/tbox/platform/poller.c
    lib/tbox/platform/ifaddrs.c
    lib/tbox/container/element/uint16.c
    lib/tbox/libc/string/strrchr.c
    lib/tbox/utils/used.c
    lib/tbox/platform/ltimer.c
    lib/tbox/platform/impl/sockdata.c
    lib/tbox/network/impl/network.c
    lib/tbox/libc/stdio/puts.c
    lib/tbox/platform/print.c
    lib/tbox/stream/static_stream.c
    lib/tbox/platform/sched.c
    lib/tbox/algorithm/binary_find.c
    lib/tbox/platform/memory.c
    lib/tbox/stream/impl/stream/sock.c
    lib/tbox/platform/process.c
    lib/tbox/platform/thread.c
    lib/tbox/libm/isqrti64.c
    lib/tbox/network/url.c
    lib/tbox/libc/string/wcsicmp.c
    lib/tbox/libm/isqrti.c
    lib/tbox/container/element/uint32.c
    lib/tbox/libm/ilog2i.c
    lib/tbox/libm/idivi8.c
    lib/tbox/libc/string/wcscmp.c
    lib/tbox/utils/bits.c
    lib/tbox/libc/stdio/wputs.c
    lib/tbox/algorithm/rfind.c
    lib/tbox/container/list_entry.c
    lib/tbox/libm/impl/libm.c
    lib/tbox/network/dns/server.c
    lib/tbox/libc/string/strrstr.c
    lib/tbox/libc/stdio/snprintf.c
    lib/tbox/libc/string/memset.c
    lib/tbox/container/circle_queue.c
    lib/tbox/container/hash_set.c
    lib/tbox/network/dns/cache.c
    lib/tbox/utils/trace.c
    lib/tbox/memory/buffer.c
    lib/tbox/container/iterator.c
    lib/tbox/container/queue.c
    lib/tbox/container/element/long.c
    lib/tbox/memory/impl/prefix.c
    lib/tbox/libc/string/strnicmp.c
    lib/tbox/algorithm/find_if.c
    lib/tbox/libc/string/memcmp.c
    lib/tbox/algorithm/binary_find_if.c
    lib/tbox/string/static_string.c
    lib/tbox/libc/string/wcsrstr.c
    lib/tbox/algorithm/rfind_if.c
    lib/tbox/container/element/str.c
    lib/tbox/libc/string/strncat.c
    lib/tbox/container/single_list.c
    lib/tbox/network/dns/looker.c
    lib/tbox/container/element/ptr.c
    lib/tbox/utils/lock_profiler.c
    lib/tbox/algorithm/count_if.c
    lib/tbox/libc/string/wcsirstr.c
    lib/tbox/algorithm/count.c
    lib/tbox/memory/default_allocator.c
    lib/tbox/libc/misc/time/localtime.c
    lib/tbox/libc/string/wcsncmp.c
    lib/tbox/libc/stdio/wprintf.c
    lib/tbox/algorithm/sort.c
    lib/tbox/algorithm/rwalk.c
    lib/tbox/algorithm/find.c
    lib/tbox/algorithm/remove_first.c
    lib/tbox/network/ipv6.c
    lib/tbox/algorithm/remove.c
    lib/tbox/network/hwaddr.c
    lib/tbox/algorithm/quick_sort.c
    lib/tbox/algorithm/walk.c
    lib/tbox/algorithm/insert_sort.c
    lib/tbox/platform/processor.c
    lib/tbox/libc/string/wcsistr.c
    lib/tbox/container/element/hash.c
    lib/tbox/libc/string/memmem.c
    lib/tbox/container/element/true.c
    lib/tbox/memory/impl/native_large_allocator.c
    lib/tbox/algorithm/bubble_sort.c
    lib/tbox/libc/string/strnistr.c
    lib/tbox/network/impl/http/method.c
    lib/tbox/network/ipaddr.c
    lib/tbox/prefix/state.c
    lib/tbox/container/element/uint8.c
    lib/tbox/container/array_iterator.c
    lib/tbox/container/element/mem.c
    lib/tbox/network/ipv4.c
    lib/tbox/network/cookies.c
    lib/tbox/libc/string/wcsndup.c
    lib/tbox/memory/impl/static_large_allocator.c
    lib/tbox/stream/impl/stream/filter.c
    lib/tbox/platform/atomic64.c
    lib/tbox/libc/stdio/swprintf.c
    lib/tbox/utils/dump.c
    lib/tbox/math/fixed16.c
    lib/tbox/libc/string/wcsichr.c
    lib/tbox/libc/stdio/vsnprintf.c
    lib/tbox/libc/string/stristr.c
    lib/tbox/stream/filter.c
    lib/tbox/stream/impl/stream/http.c
    lib/tbox/algorithm/predicate.c
    lib/tbox/container/list.c
    lib/tbox/container/element/size.c
    lib/tbox/memory/static_buffer.c
    lib/tbox/libc/string/memdup.c
    lib/tbox/platform/path.c
    lib/tbox/libc/string/strichr.c
    lib/tbox/libc/string/wcscat.c
    lib/tbox/libc/string/memmov.c
    lib/tbox/libc/string/strncmp.c
    lib/tbox/libc/misc/time/gmtime.c
    lib/tbox/libc/string/strcpy.c
    lib/tbox/libc/string/strcat.c
    lib/tbox/network/http.c
    lib/tbox/libc/string/strnrstr.c
    lib/tbox/libc/stdio/printf.c
    lib/tbox/libc/string/strlcpy.c
    lib/tbox/libc/string/strnrchr.c
    lib/tbox/libc/string/strnstr.c
    lib/tbox/libc/string/wcsncat.c
    lib/tbox/libc/stdio/printf_object.c
    lib/tbox/libc/misc/time/mktime.c
    lib/tbox/libc/string/strlen.c
    lib/tbox/libc/string/wcschr.c
    lib/tbox/libc/string/strirchr.c
    lib/tbox/libc/string/strdup.c
    lib/tbox/math/impl/math.c
    lib/tbox/libc/string/wcsirchr.c
    lib/tbox/hash/adler32.c
    lib/tbox/libc/misc/time/time.c
    lib/tbox/hash/fnv32.c
    lib/tbox/memory/small_allocator.c
    lib/tbox/tbox.c
    lib/tbox/libc/string/strchr.c
    lib/tbox/platform/impl/dns.c
    lib/tbox/libc/string/strnirstr.c
    lib/tbox/libc/misc/time/gmmktime.c
    lib/tbox/stream/impl/filter/cache.c
    lib/tbox/libc/string/strnirchr.c
    lib/tbox/container/bloom_filter.c
    lib/tbox/libc/string/strcmp.c
    lib/tbox/libc/string/wcsrchr.c
    lib/tbox/libc/string/wcsnirchr.c
    lib/tbox/memory/static_allocator.c
    lib/tbox/container/heap.c
    lib/tbox/libc/impl/libc.c
    lib/tbox/libc/string/wcsnrchr.c
    lib/tbox/math/random/random.c
    lib/tbox/libc/string/wcsncpy.c
    lib/tbox/libc/string/strnlen.c
    lib/tbox/libc/string/stricmp.c
    lib/tbox/platform/dynamic.c
    lib/tbox/libc/stdlib/mbstowcs.c
    lib/tbox/libc/string/strstr.c
    lib/tbox/libc/string/wcsstr.c
    lib/tbox/platform/backtrace.c
    lib/tbox/libc/string/strirstr.c
    lib/tbox/libc/string/wcscpy.c
    lib/tbox/memory/queue_buffer.c
    lib/tbox/memory/string_pool.c
    lib/tbox/memory/fixed_pool.c
    lib/tbox/libc/string/wcslcpy.c
    lib/tbox/utils/singleton.c
    lib/tbox/libc/string/strndup.c
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
    test/container/test_hashtable_iter.c
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
    .
)
target_include_directories(kon INTERFACE
    .
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
    core/string/kx_stringbuffer.c
    core/interpreter/keyword/prefix_eval.c
    core/gc.c
    core/interpreter/keyword/prefix_lambda.c
    core/interpreter/cps_interpreter.c
    core/module/kon_cell.c
    core/kson/reader.c
    core/container/kx_hashtable.c
    core/interpreter/keyword/prefix_let.c
    core/string/kx_stringview.c
    core/interpreter/keyword/prefix_break.c
    core/utils/realpath.c
    core/module/kon_number.c
    core/interpreter/keyword/prefix_if.c
    core/kson/value_builder.c
    core/interpreter/keyword/prefix_apply.c
    core/container/murmurhash.c
    core/module/kon_table.c
    core/container/kx_vector.c
    core/kson/writer.c
    core/module/kon_module.c
    core/interpreter/keyword/prefix_or.c
    core/node.c
    core/interpreter/keyword/prefix_for.c
    core/interpreter/keyword/prefix_set.c
    core/interpreter/keyword/prefix_do.c
    core/kon.c
    core/module/kon_accessor.c
    core/interpreter/keyword/prefix_set_dispatcher.c
    core/interpreter/keyword/prefix_callcc.c
    core/interpreter/keyword/prefix_sh.c
    core/kson/tokenizer.c
    core/utils/number_utils.c
    core/container/kx_list.c
    core/interpreter/keyword/prefix_and.c
    core/interpreter/env.c
    core/interpreter/keyword/prefix_cond.c
    core/module/kon_list.c
    core/interpreter/primary.c
    core/interpreter/keyword/prefix_continue.c
    core/module/kon_vector.c
    core/kson/state_stack.c
    core/interpreter/keyword/prefix_def_builder.c
    core/interpreter/keyword/prefix_def_dispatcher.c
    core/interpreter/keyword/prefix_blk.c
    core/module/kon_string.c
    core/interpreter/keyword/prefix_func.c
)

