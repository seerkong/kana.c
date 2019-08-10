includes("check_cfuncs.lua")
includes("check_csnippets.lua")

-- set optimization: none, faster, fastest, smallest 
set_optimize("none")
set_symbols("debug")
add_defines("DEBUG")


-- add include search directories
add_includedirs("/usr/include", "/usr/local/include")

-- add link libraries and search directories
add_links("tbox", "pthread")
add_linkdirs("/usr/local/lib", "/usr/lib")

-- add compilation and link flags
add_cxflags("-stdnolib", "-fno-strict-aliasing")
add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})

target("test_tokenizer")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kon")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")

    set_optimize("none")
    set_symbols("debug")
    add_defines("DEBUG")

    -- make as a binary
    set_kind("binary")

    -- add the source files
    add_files("kson/test_tokenizer.c")

target("test_reader")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kon")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")

    set_optimize("none")
    set_symbols("debug")
    add_defines("DEBUG")

    -- make as a binary
    set_kind("binary")

    -- add the source files
    add_files("kson/test_reader.c")

target("test_eval")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kon")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")

    set_optimize("none")
    set_symbols("debug")
    add_defines("DEBUG")

    -- make as a binary
    set_kind("binary")

    -- add the source files
    add_files("klang/test_eval.c")

target("test_table")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kon")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")

    set_optimize("none")
    set_symbols("debug")
    add_defines("DEBUG")

    -- make as a binary
    set_kind("binary")

    -- add the source files
    add_files("container/test_hashtable.c")
    add_files("container/test_table.c")