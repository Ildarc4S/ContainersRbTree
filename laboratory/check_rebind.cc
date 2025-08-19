#include <type_traits>
#include <memory>

template <typename ValPtr_>
struct CheckRebind {
  using RebindVoidPtr = std::pointer_traits<ValPtr_>::
                      template rebind<void>;
  static_assert(std::is_same_v<
    typename std::pointer_traits<RebindVoidPtr>::element_type,
    void>, "Error");
};

int main() {
   CheckRebind<int*>{};
   CheckRebind<std::shared_ptr<double>>{};
  return 0;
}
