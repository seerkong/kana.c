-- add target
target("sregex")
    -- make as a static library
    set_kind("static")

    -- add include directories
    add_includedirs(".", {public = true})

    -- add the header files for installing
    add_headerfiles("./dynasm/*.h")
    add_headerfiles("./sregex/*.h")

    -- add options
    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cflags("-std=gnu99", "", {force = true})
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
  
    -- add the common source files
    -- add_files("./dynasm/*.c")
    add_files("./sregex/*.c")

