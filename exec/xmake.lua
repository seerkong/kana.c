target("kni")
    -- add the dependent target
    -- add_deps("tbox")
    add_deps("kana")

    add_cflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
    add_cxflags("-Wno-unused-variable", "-Wno-unused-function", "-fno-strict-aliasing")
  

    -- make as a binary
    set_kind("binary")

    -- add the source files
    add_files("commander.c")
    add_files("kni.c")
    
    after_build(function (target)
      -- local target_file = path.join(path.directory(target:targetfile()), target:basename() .. "")
      -- assert(os.isfile(target_file), "executable file not found, please build %s first!", target:name())
      -- show install info
      print("after_build targetfile : %s , basename: %s ..", target:targetfile(), target:basename())
      os.cp(target:targetfile(), "./kni")
    end)