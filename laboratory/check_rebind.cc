#include <type_traits>
#include <memory>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

template <typename ValPtr_>
struct CheckRebind {
  using RebindVoidPtr = std::pointer_traits<ValPtr_>::
                      template rebind<void>;
  static_assert(std::is_same_v<
    typename std::pointer_traits<RebindVoidPtr>::element_type,
    void>, "Error");
};

template <typename T>
void print_type() {
  int status;
  const char* name = typeid(T).name();
  char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
  std::cout << (status == 0 ? demangled : name) << std::endl;
  free(demangled);
}

template <typename Ptr_>
struct NodeBase {
    using BasePtr_ = typename std::pointer_traits<Ptr_>::
                     template rebind<NodeBase>;
    BasePtr_ parent_;
    BasePtr_ right_;
    BasePtr_ left_;
};

int main() {
  CheckRebind<int*>{};
  CheckRebind<std::shared_ptr<double>>{};

  using Ptr = std::unique_ptr<int>;
  using NodeBaseType = NodeBase<Ptr>;
  print_type<typename NodeBaseType::BasePtr_>();

  return 0;
}
