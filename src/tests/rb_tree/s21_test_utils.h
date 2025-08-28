#ifndef _S21_TEST_UTILS_
#define _S21_TEST_UTILS_

#define EXPECT_TYPE_SAME(T1, T2) \
  static_assert(std::is_same_v<T1, T2>, "Types should be the same"); \
  ASSERT_TRUE((std::is_same_v<T1, T2>))

template <typename T>
static void print_type() {
  int status = -1;
  const char* name = typeid(T).name();
  char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
  std::cout << (status == 0 ? demangled : name) << std::endl;
  free(demangled);
}

#endif //  _S21_TEST_UTILS_
