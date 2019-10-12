-- project
set_project("kon")

-- set xmake minimum version
set_xmakever("2.2.5")

-- set project version
set_version("0.1.0", {build = "%Y%m%d%H%M"})

-- set warning all as error
-- set_warnings("all", "error")

-- set language: c99, c++14
set_languages("c99", "cxx14")

-- add defines to config.h
set_configvar("_GNU_SOURCE", 1)
set_configvar("_REENTRANT", 1)

-- disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined")
add_mxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined")


includes("check_cfuncs.lua")
includes("check_csnippets.lua")

-- set optimization: none, faster, fastest, smallest 
set_optimize("none")
set_symbols("debug")
add_defines("DEBUG")


-- add include search directories
add_includedirs("/usr/local/include", "/usr/include", "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include")

-- add link libraries and search directories
-- add_links("tbox", "pthread")
add_linkdirs("/usr/local/lib", "/usr/lib", "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib")

-- add compilation and link flags
add_cxflags("-stdnolib", "-fno-strict-aliasing")
add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})





-- get function name 
--
-- sigsetjmp
-- sigsetjmp((void*)0, 0)
-- sigsetjmp{sigsetjmp((void*)0, 0);}
-- sigsetjmp{int a = 0; sigsetjmp((void*)a, a);}
--
function get_function_name(func)
    local name, code = string.match(func, "(.+){(.+)}")
    if code == nil then
        local pos = func:find("%(")
        if pos then
            name = func:sub(1, pos - 1)
        else
            name = func
        end
    end
    return name:trim()
end

-- check c functions in the given module
function check_module_cfuncs(module, includes, ...)
    for _, func in ipairs({...}) do
        local funcname = get_function_name(func)
        configvar_check_cfuncs(("TB_CONFIG_%s_HAVE_%s"):format(module:upper(), funcname:upper()), func, {name = module .. "_" .. funcname, includes = includes})
    end
end

-- check interfaces
function check_interfaces()

    -- check the interfaces for libc
    check_module_cfuncs("libc", {"string.h", "stdlib.h"},           
        "memcpy",
        "memset",
        "memmove",
        "memcmp",
        "memmem",
        "strcat",
        "strncat",
        "strcpy",
        "strncpy",
        "strlcpy",
        "strlen",
        "strnlen",
        "strstr",
        "strcasestr",
        "strcmp",
        "strcasecmp",
        "strncmp",
        "strncasecmp")
    check_module_cfuncs("libc", {"wchar.h", "stdlib.h"},            
        "wcscat",
        "wcsncat",
        "wcscpy",
        "wcsncpy",
        "wcslcpy",
        "wcslen",
        "wcsnlen",
        "wcsstr",
        "wcscasestr",
        "wcscmp",
        "wcscasecmp",
        "wcsncmp",
        "wcsncasecmp",
        "wcstombs",
        "mbstowcs")
    check_module_cfuncs("libc", "time.h",                           "gmtime", "mktime", "localtime")
    check_module_cfuncs("libc", "sys/time.h",                       "gettimeofday")
    check_module_cfuncs("libc", {"signal.h", "setjmp.h"},           "signal", "setjmp", "sigsetjmp{sigjmp_buf buf; sigsetjmp(buf, 0);}", "kill")
    check_module_cfuncs("libc", "execinfo.h",                       "backtrace")
    check_module_cfuncs("libc", "locale.h",                         "setlocale")
    check_module_cfuncs("libc", "stdio.h",                          "fputs")
    check_module_cfuncs("libc", "stdlib.h",                         "srandom", "random")

    -- add the interfaces for libm
    check_module_cfuncs("libm", "math.h", 
        "sincos", 
        "sincosf", 
        "log2", 
        "log2f",
        "sqrt",
        "sqrtf",
        "acos", 
        "acosf",
        "asin",
        "asinf",
        "pow",
        "powf",
        "fmod",
        "fmodf",
        "tan",
        "tanf",
        "atan",
        "atanf",
        "atan2",
        "atan2f",
        "cos",
        "cosf",
        "sin",
        "sinf",
        "exp",
        "expf")

    -- add the interfaces for posix
    check_module_cfuncs("posix", {"sys/poll.h", "sys/socket.h"},     "poll")
    check_module_cfuncs("posix", {"sys/select.h"},                   "select")
    check_module_cfuncs("posix", "pthread.h",                        
        "pthread_mutex_init",
        "pthread_create", 
        "pthread_setspecific", 
        "pthread_getspecific",
        "pthread_key_create",
        "pthread_key_delete")
    check_module_cfuncs("posix", {"sys/socket.h", "fcntl.h"},        "socket")
    check_module_cfuncs("posix", "dirent.h",                         "opendir")
    check_module_cfuncs("posix", "dlfcn.h",                          "dlopen")
    check_module_cfuncs("posix", {"sys/stat.h", "fcntl.h"},          "open", "stat64")
    check_module_cfuncs("posix", "unistd.h",                         "gethostname")
    check_module_cfuncs("posix", "ifaddrs.h",                        "getifaddrs")
    check_module_cfuncs("posix", "semaphore.h",                      "sem_init")
    check_module_cfuncs("posix", "unistd.h",                         "getpagesize", "sysconf")
    check_module_cfuncs("posix", "sched.h",                          "sched_yield")
    check_module_cfuncs("posix", "regex.h",                          "regcomp", "regexec")
    check_module_cfuncs("posix", "sys/uio.h",                        "readv", "writev", "preadv", "pwritev")
    check_module_cfuncs("posix", "unistd.h",                         "pread64", "pwrite64")
    check_module_cfuncs("posix", "unistd.h",                         "fdatasync")
    check_module_cfuncs("posix", "copyfile.h",                       "copyfile")
    check_module_cfuncs("posix", "sys/sendfile.h",                   "sendfile")
    check_module_cfuncs("posix", "sys/epoll.h",                      "epoll_create", "epoll_wait")
    check_module_cfuncs("posix", "spawn.h",                          "posix_spawnp")
    check_module_cfuncs("posix", "unistd.h",                         "execvp", "execvpe", "fork", "vfork")
    check_module_cfuncs("posix", "sys/wait.h",                       "waitpid")
    check_module_cfuncs("posix", "unistd.h",                         "getdtablesize")
    check_module_cfuncs("posix", "sys/resource.h",                   "getrlimit")
    check_module_cfuncs("posix", "netdb.h",                          "getaddrinfo", "getnameinfo", "gethostbyname", "gethostbyaddr")

    -- add the interfaces for systemv
    check_module_cfuncs("systemv", {"sys/sem.h", "sys/ipc.h"},       "semget", "semtimedop")

    -- add the interfaces for valgrind
    check_module_cfuncs("valgrind", "valgrind/valgrind.h",           "VALGRIND_STACK_REGISTER(0, 0)")

    -- check __thread keyword
    configvar_check_csnippets("TB_CONFIG_KEYWORD_HAVE__thread", "__thread int a = 0;", {name = "keyword_thread", links = "pthread", languages = "c99"})
    configvar_check_csnippets("TB_CONFIG_KEYWORD_HAVE_Thread_local", "_Thread_local int a = 0;", {name = "keyword_thread_local", links = "pthread", languages = "c99"})
end



-- include project sources
includes("lib")
-- includes("src")
includes(format("core/%s.lua", "xmake")) 
includes(format("exec/%s.lua", "xmake"))
includes("test")