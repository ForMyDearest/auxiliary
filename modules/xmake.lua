add_requires("u8lib")
add_requires("xxhash")
add_requires("yyjson")

target("auxiliary")
do
    set_kind("$(kind)")
    set_group("01.libraries")
    add_rules("c++.unity_build", { batchsize = 16 })

    add_deps("compile-flags", { public = true })
    add_packages("xxhash")
    add_packages("yyjson")
    add_packages("u8lib", { public = true })

    add_files("private/*.cpp")
    add_includedirs("public", { public = true })
    add_headerfiles("public/(**)")

    after_load(function(target, opt)
        import("core.project.project")
        if (target:get("kind") == "shared") then
            target:add("defines", "AUXILIARY_DLL", { public = true })
        end
    end)

    target_end()
end
