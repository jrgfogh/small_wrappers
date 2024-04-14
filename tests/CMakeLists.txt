add_executable(
  small_wrappers_tests
  "propagate_const_tests.cpp"
  "lazy_init_tests.cpp")
target_link_libraries(
  small_wrappers_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(small_wrappers_tests)