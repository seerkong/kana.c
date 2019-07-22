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


-- include project sources
includes("src")
includes("test")