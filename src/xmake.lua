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

includes(format("kon/%s.lua", "xmake")) 
includes(format("kli/%s.lua", "xmake")) 
includes(format("playground/%s.lua", "xmake")) 