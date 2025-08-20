#ifndef _S21_TEST_UTILS_
#define _S21_TEST_UTILS_

template <typename T>
static void print_type() {
  int status;
  const char* name = typeid(T).name();
  char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
  std::cout << (status == 0 ? demangled : name) << std::endl;
  free(demangled);
}

#endif //  _S21_TEST_UTILS_
