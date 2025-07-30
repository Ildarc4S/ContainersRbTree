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

  void print_tree() const {
    if (root_ == nullptr) {
        std::println("(empty tree)");
        return;
    }

    std::vector<std::vector<Node*>> levels;
    std::vector<Node*> current_level{root_};

    while (!current_level.empty()) {
        levels.push_back(current_level);
        std::vector<Node*> next_level;
        for (Node* node : current_level) {
            if (node) {
                next_level.push_back(node->left_);
                next_level.push_back(node->right_);
            } else {
                next_level.push_back(nullptr);
                next_level.push_back(nullptr);
            }
        }
        if (std::all_of(next_level.begin(), next_level.end(), 
            [](Node* n) { return n == nullptr; })) {
            break;
        }
        current_level = next_level;
    }

    // Вычисляем позиции для каждого узла
    int level_count = levels.size();
    int last_level_width = (1 << (level_count-1)) * 3;

    for (int i = 0; i < level_count; ++i) {
        int node_count = levels[i].size();
        int spacing = last_level_width / (node_count + 1);
        int offset = spacing;

        // Выводим узлы текущего уровня
        for (Node* node : levels[i]) {
            std::print("{:>{}}", "", offset);

            if (node) {
                std::string color = (node->color_ == Color::kRed) ? "\x1B[91mR\x1B[0m" : "\x1B[1mB\x1B[0m";
                std::print("{}({})", node->value_, color);
            } else {
                std::print("   ");
            }
            offset = spacing * 2;
        }
        std::println("");
        if (i < level_count - 1) {
            offset = spacing;
            for (Node* node : levels[i]) {
                std::print("{:>{}}", "", offset);
                if (node && (node->left_ || node->right_)) {
                    std::print(" / \\ ");
                } else {
                    std::print("     ");
                }
                offset = spacing * 2;
            }
            std::println("");
        }
    }
  }
};

int main() {


  return 0;
}
