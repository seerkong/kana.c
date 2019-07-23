target("playtokenizer")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kon")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")


    -- make as a binary
    set_kind("binary")

    set_optimize("none")
    set_symbols("debug")
    add_defines("DEBUG")


    -- add defines
    add_defines("__tb_prefix__=\"tokenizer\"")

    -- add the source files
    add_files("kson/tokenizer.c")


target("playwriter")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kon")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")


    -- make as a binary
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"playwriter\"")

    -- add the source files
    add_files("kson/writer.c")
