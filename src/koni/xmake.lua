target("koni")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kon")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
  

    -- make as a binary
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"koni\"")

    -- add the source files
    add_files("main.c")
    
    