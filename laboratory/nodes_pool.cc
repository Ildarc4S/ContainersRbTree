/*
 * Здесь представлена упрощенная версия пула для
 * переиспользования памяти на примере списка,
 * но применим и для rb_tree.
 *
 * */

#include <print>
#include <memory>
#include <pthread.h>

template <typename T>
struct Node {
  T value_;
  Node* next_;
  explicit Node(const T& value) : value_(value), next_(nullptr) {}
};

template <typename T, typename Allocator = std::allocator<T>>
class LinkedList {
 public:
    using NodePtr = Node<T>*;
 private:

  using NodeAllocator =
    std::allocator_traits<Allocator>:: template rebind_alloc<Node<T>>;
  using NodeAllocatorTraits = std::allocator_traits<NodeAllocator>;

  NodePtr head_ = nullptr;
  NodePtr free_list_ = nullptr;
  NodeAllocator alloc_;

  /* Берет ноду из пула
   * или создает новый
   * */
  NodePtr get_node(const T& value);

  /* Возвращает свободный
   * узел в пул
   * */
  void recycle_node(NodePtr node);

  std::string_view RED = "\033[31m";
  std::string_view GREEN = "\033[32m";
  std::string_view YELLOW = "\033[33m";
  std::string_view BLUE = "\033[34m";
  std::string_view MAGENTA = "\033[35m";
  std::string_view CYAN = "\033[36m";
  std::string_view RESET = "\033[0m";

 public:
  // Добавление в начало списка
  void push_front(const T& value);
  // Удаление из начала списка
  void pop_front();
  void print();

  ~LinkedList();
};

template <typename T, typename Allocator>
LinkedList<T, Allocator>::NodePtr
LinkedList<T, Allocator>::get_node(const T& value) {
  NodePtr node = nullptr;
  if (free_list_ != nullptr) {
    std::println("{}[[get_node]]: Using pool{}", GREEN, RESET);
    node = free_list_;
    free_list_ = free_list_->next_;

    node->value_ = value;
    node->next_ = nullptr;
  } else {
   std::println("{}[[get_node]]: Alloc memory{}", MAGENTA, RESET);
   node = NodeAllocatorTraits::allocate(alloc_, 1);
   NodeAllocatorTraits::construct(alloc_, node, value);
  }

  return node;
}

template <typename T, typename Allocator>
void
LinkedList<T, Allocator>::recycle_node(LinkedList<T, Allocator>::NodePtr node) {
  std::println("{}[[recycle_node]]: Node to pool{}", CYAN, RESET);
  node->next_ = free_list_;
  free_list_ = node;
}

template <typename T, typename Allocator>
void LinkedList<T, Allocator>::push_front(const T& value) {
  std::println("{}[[push_front]]: push {}{}", BLUE, value, RESET);
  NodePtr node = get_node(value);
  node->next_ = head_;
  head_ = node;
}

template <typename T, typename Allocator>
void LinkedList<T, Allocator>::pop_front() {
  if (head_ == nullptr) {
    return;
  }
  std::println("{}[[pop_front]]: pop {}{}", RED,head_->value_, RESET);

  NodePtr old_head = head_;
  head_ = head_->next_;
  recycle_node(old_head);
}

template <typename T, typename Allocator>
void LinkedList<T, Allocator>::print() {
    NodePtr current = head_;
    std::print("{}List: ", BLUE);
    while (current != nullptr) {
        std::print("{}{}", YELLOW, current->value_);
        if (current->next_ != nullptr) {
            std::print("{} -> ", RED);
        }
        current = current->next_;
    }
    std::println("{}", RESET);
}

template <typename T, typename Allocator>
LinkedList<T, Allocator>::~LinkedList() {
  while(head_) {
    NodePtr tmp = head_;
    head_ = head_->next_;
    NodeAllocatorTraits::destroy(alloc_, tmp);
    NodeAllocatorTraits::deallocate(alloc_, tmp, 1);
  }

  while(free_list_) {
    NodePtr tmp = free_list_;
    free_list_ = free_list_->next_;
    NodeAllocatorTraits::destroy(alloc_, tmp);
    NodeAllocatorTraits::deallocate(alloc_, tmp, 1);
  }
}

int main() {
  LinkedList<int> list;
  list.push_front(10);
  list.push_front(11);
  list.push_front(12);
  list.print();
  list.pop_front();
  list.push_front(13);
  list.print();
  return 0;
}

