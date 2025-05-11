add_requires("doctest 2.4.11")

function TEST(name)
    target(name)
    do
        set_kind("binary")
        set_group("unit_test")
        add_deps("auxiliary")
        add_packages("doctest")
        add_files(name .. ".cpp")
        add_defines("DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN")
    end
end

TEST("hash")
TEST("json")
TEST("type_traits")
TEST("intrusive_ptr")
TEST("compressed_pair")
