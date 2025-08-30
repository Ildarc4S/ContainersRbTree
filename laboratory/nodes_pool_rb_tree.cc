#include <print>
#include <memory>
#include <vector>

enum class Color {
  kRed,
  kBlack
};

template <typename T>
struct RbNode {
  T value_;
  Color color_;
  RbNode* left_;
  RbNode* right_;
  RbNode* parent_;

  explicit RbNode(const T& value)
    : value_(value), color_(Color::kRed),
      left_(nullptr), right_(nullptr), parent_(nullptr) {}
};

template <typename T,
          typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
class RBTree {
  using Node = RbNode<T>;
  using NodeAllocator =
    typename std::allocator_traits<Allocator>::templae rebind_alloc<Node>;
  using NodeTraits = std::allocator_traits<NodeAllocator>;

  Node* root_ = nullptr;
  Node* free_list_ = nullptr;
  Compare compare_;
  NodeAllocator alloc_;

  Node* acquire_node(const T& value);
  void release_node(Node* node);
};

int main() {


  return 0;
}
