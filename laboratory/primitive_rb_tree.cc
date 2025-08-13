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
  Node* getRoot() { return root_; }
  void rebalance_insert(Node* node);

  void rotate_left(Node* x);
  void rotate_right(Node* x);

  void transplant(Node* u, Node* v);
  Node* minimum(Node* node);
  Node* find_node(const T& value);
  void remove(Node* node);

  void print_tree() const;

  void insert(const T& value);
  void erase(const T& value);
};

/*
 *   x           y
 *  / \         / \
 * A   y   ->  x   C
 *    / \     / \
 *   B   C   A   B
 */
template <typename T, typename Compare>
void RBTree<T, Compare>::rotate_left(Node* x) {
  if (!x->right_) {
    return;
  }

  Node* y = x->right_;
  x->right_ = y->left_;

  if (y->left_) {
    y->left_->parent_ = x;
  }
  y->parent_ = x->parent_;

  if (!x->parent_) {
    root_ = y;
  } else if (x == x->parent_->left_) {
    x->parent_->left_ = y;
  } else {
    x->parent_->right_ = y;
  }

  y->left_ = x;
  x->parent_ = y;
}

/*
 *     x        y
 *    / \      / \
 *   y   C -> A   x
 *  / \          / \
 * A   B        B   C
*/
template <typename T, typename Compare>
void RBTree<T, Compare>::rotate_right(Node* x) {
  if (!x->left_) {
    return;
  }

  Node* y = x->left_;
  x->left_ = y->right_;

  if (y->right_) {
    y->right_->parent_ = x; // B->parent_ = x
  }

  y->parent_ = x->parent_;
  if (!x->parent_) {
    root_ = y;
  } else if (x == x->parent_->left_) {
    x->parent_->left_ = y;
  } else {
    x->parent_->right_ = y;
  }

  y->right_ = x;
  x->parent_ = y;
}

template <typename T, typename Compare>
void RBTree<T, Compare>::transplant(Node* u, Node* v) {
  if (u->parent_ == nullptr) {
    root_ = v;
  } else if (u == u->parent_->left_) {
    u->parent_->left_ = v;
  } else {
    u->parent_->right_ = v;
  }

  if (v != nullptr) {
    v->parent_ = u->parent_;
  }
}

template <typename T, typename Compare>
RBTree<T, Compare>::Node* RBTree<T, Compare>::minimum(Node* node) {
  while (node->left_ != nullptr) {
    node = node->left_;
  }
  return node;
}

/*
 *    g(b)
 *   /   \
 *  y(r) u(r)
 *  /
 * x(r)
*/
template <typename T, typename Compare>
void RBTree<T, Compare>::rebalance_insert(Node* x) {
  while(x->parent_ && x->parent_->color_ == Color::kRed ) {
    if (x->parent_ == x->parent_->parent_->left_) {
      // C - red
      Node* u = x->parent_->parent_->right_;
      if (u && u->color_ == Color::kRed) {
        x->parent_->color_ = Color::kBlack;
        u->color_ = Color::kBlack;
        x->parent_->parent_->color_ = Color::kRed;
        x = x->parent_->parent_; // move ptr to g
      } else if (x == x->parent_->right_) {
        x = x->parent_;
        rotate_left(x);
      } else {
        x->parent_->color_ = Color::kBlack;
        x->parent_->parent_->color_ = Color::kRed;
        rotate_right(x->parent_->parent_);
      }
    } else {
      Node* u = x->parent_->parent_->left_;
      if (u && u->color_ == Color::kRed) {
        x->parent_->color_ = Color::kBlack;
        u->color_ = Color::kBlack;
        x->parent_->parent_->color_ = Color::kRed;
        x = x->parent_->parent_;
      } else if(x == x->parent_->left_) {
        x = x->parent_;
        rotate_right(x);
      } else {
        x->parent_->color_ = Color::kBlack;
        x->parent_->parent_->color_ = Color::kRed;
        rotate_left(x->parent_->parent_);
      }
    }
  }
  root_->color_ = Color::kBlack;
}

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
  rebalance_insert(new_node);
}

template <typename T, typename Compare>
void RBTree<T, Compare>::print_tree() const {
  if (root_ == nullptr) {
      std::println("(empty tree)");
      return;
  }

  std::println("");
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
  tree.insert(5);
  tree.insert(15);
  tree.insert(3);
  tree.insert(5);
  tree.print_tree();
  std::println("{}", tree.minimum(tree.getRoot())->value_);
  return 0;
}
