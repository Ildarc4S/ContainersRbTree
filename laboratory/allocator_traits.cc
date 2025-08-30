#include <memory>

struct SimpleAllocator {
  using value_type = int;

  int* allocate(std::size_t n) {
    return new int[n];
  }

  void deallocate(int* p, std::size_t) {
    delete[] p;
  }
};


int main() {
  SimpleAllocator alloc;
  using SimpleAllocatorTraits = std::allocator_traits<SimpleAllocator>;
  int* p = SimpleAllocatorTraits::allocate(alloc, 1);
  SimpleAllocatorTraits::construct(alloc, p, 42);
  SimpleAllocatorTraits::destroy(alloc, p);
  SimpleAllocatorTraits::deallocate(alloc, p, 1);
  return 0;
}
