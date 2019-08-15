-- add target
target("kon")
    -- make as a static library
    set_kind("static")

    set_optimize("none")
    set_symbols("debug")
    add_defines("DEBUG", "__debug__")

    add_deps("tbox")
    
    -- add defines
    add_defines("__kon_prefix__=\"kon\"")

    -- add include directories
    add_includedirs("..", {public = true})

    -- add the header files for installing
    add_headerfiles("./*.h")

    -- add options

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
  
    -- add the common source files
    add_files("utils/**.c")
    add_files("*.c")
    add_files("string/**.c")
    add_files("container/**.c") 
    add_files("kson/**.c")
    add_files("interpreter/**.c")  

