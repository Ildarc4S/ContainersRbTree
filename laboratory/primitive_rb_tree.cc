#include <print>
#include <memory>
#include <vector>
#include <algorithm>
#include <cstdint>

enum class Color : std::uint8_t {
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

template <typename T, typename Compare = std::less<T>>
class RBTree {
  using Node = RbNode<T>;

  Node* root_ = nullptr;
  Compare compare_;

 public:
  void insert(const T& value);
  void print_tree() const;
};

template <typename T, typename Compare>
void RBTree<T, Compare>::insert(const T& value) {
  Node* new_node = new Node(value);

  if (root_ == nullptr) {
   root_ = new_node;
   root_->color_ = Color::kBlack;
   return;
  }

  Node* current = root_;
  Node* parent = nullptr;

  while(current != nullptr) {
    parent = current;
    if (compare_(value, current->value_)) {
      current = current->left_;
    } else {
      current = current->right_;
    }
  }

  new_node->parent_ = parent;
  if (compare_(value, parent->value_)) {
    parent->left_ = new_node;
  } else {
    parent->right_ = new_node;
  }
}

template <typename T, typename Compare>
void RBTree<T, Compare>::print_tree() const {
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
          if (node->color_ == Color::kRed) {
            std::print("\x1B[91m{:2}\x1B[0m", node->value_);
          } else {
            std::print("\x1B[1m{:2}\x1B[0m", node->value_);
          }
        } else {
            std::print("  ");
        }
        offset = spacing;
      }
        std::println("");
    if (i < level_count - 1) {
      offset = spacing;
      for (Node* node : levels[i]) {
        std::print("{:>{}}", "", offset);
        if (node && (node->left_ || node->right_)) {
          std::print("/ \\ ");
        } else {
          std::print("   ");
        }
        offset = spacing;
      }
      std::println("");
    }
  }
}

int main() {
  RBTree<int> tree;
  tree.insert(10);
  tree.insert(15);
  tree.insert(15);
  tree.insert(4);
  tree.insert(5);
  tree.insert(6);
  tree.print_tree();

  return 0;
}
