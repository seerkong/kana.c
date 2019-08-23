target("kli")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kon")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
  

    -- make as a binary
    set_kind("binary")

    -- add the source files
    add_files("commander.c")
    add_files("main.c")
    
    after_build(function (target)

    end)