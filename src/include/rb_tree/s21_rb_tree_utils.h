#ifndef _S21_RB_TREE_UTILS_
#define _S21_RB_TREE_UTILS_

#include <memory>

namespace s21 {
namespace rb_tree {

template <typename Ptr_, typename T_>
using PtrTraitsRebind_ = std::pointer_traits<Ptr_>::template rebind<T_>;

template <typename Ptr_>
using PtrTraitsElemType_ = std::pointer_traits<Ptr_>::element_type;

template <typename ValueType, typename Iter_>
concept SameValueType =
    std::same_as<ValueType, typename std::iterator_traits<Iter_>::value_type>;

template <typename Pair_>
struct SelectFirst {
  using pair_first = Pair_::first_type;
  pair_first& operator()(Pair_& pair) const noexcept;
  const pair_first& operator()(const Pair_& pair) const noexcept;
};

template <typename T_>
struct Identity {
  using type = T_;
  type& operator()(type& value) const noexcept;
  const type& operator()(const type& value) const noexcept;
};

template <typename Pair_>
SelectFirst<Pair_>::pair_first& SelectFirst<Pair_>::operator()(
    Pair_& pair) const noexcept {
  return pair.first;
}

template <typename Pair_>
const SelectFirst<Pair_>::pair_first& SelectFirst<Pair_>::operator()(
    const Pair_& pair) const noexcept {
  return pair.first;
}

template <typename T_>
Identity<T_>::type& Identity<T_>::operator()(type& value) const noexcept {
  return value;
}

template <typename T_>
const Identity<T_>::type& Identity<T_>::operator()(
    const type& value) const noexcept {
  return value;
}

enum class NodeColor : std::uint8_t { kBlack, kRed };

template <typename Ptr_>
struct NodeBase {
  using BasePtr_ = PtrTraitsRebind_<Ptr_, NodeBase>;

  NodeColor color_;
  BasePtr_ parent_;
  BasePtr_ right_;
  BasePtr_ left_;

  static BasePtr_ Minimum(BasePtr_ node) noexcept;
  static BasePtr_ Maximum(BasePtr_ node) noexcept;
  BasePtr_ GetBasePtr() const noexcept;
};

template <typename NodeBase_>
struct Header {
 private:
  using BasePtr_ = typename NodeBase_::BasePtr_;

 public:
  Header() noexcept;
  Header(Header&& other) noexcept;

  void SwapData(Header& other) noexcept;
  void Reset() noexcept;

  NodeBase_ header_;
  std::size_t node_count_;
};

template <typename ValPtr_>
struct Node : public NodeBase<PtrTraitsRebind_<ValPtr_, void>> {
  using ValueType_ = PtrTraitsElemType_<ValPtr_>;
  using NodePtr_ = PtrTraitsRebind_<ValPtr_, Node>;

  Node() noexcept {}
  Node(Node&& other) = delete;
  ~Node() {}

  union Storage_ {
    Storage_() noexcept {};
    ~Storage_() {};
    ValueType_ data_;
  } storage_;

  ValueType_* GetValPtr();
  ValueType_ const* GetValPtr() const;
  NodePtr_ GetNodePtr() noexcept;
};

template <bool IsConst_, typename ValPtr_>
struct Iterator {
  template <typename T>
  using MaybeConst_ = std::conditional_t<IsConst_, const T, T>;

  using Node_ = Node<ValPtr_>;
  using NodeBase_ = NodeBase<PtrTraitsRebind_<ValPtr_, void>>;
  using BasePtr_ = PtrTraitsRebind_<ValPtr_, NodeBase_>;

  using value_type = PtrTraitsElemType_<ValPtr_>;
  using reference = MaybeConst_<value_type>&;
  using pointer = MaybeConst_<value_type>*;

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;

  Iterator() noexcept = default;
  Iterator(const Iterator&) = default;

  constexpr explicit Iterator(BasePtr_ node) noexcept;
  constexpr Iterator(const Iterator<false, ValPtr_>& it)
    requires IsConst_;

  [[nodiscard]] reference operator*() const noexcept;
  [[nodiscard]] pointer operator->() const noexcept;

  constexpr Iterator& operator++() noexcept;
  constexpr Iterator operator++(int) noexcept;

  constexpr Iterator& operator--() noexcept;
  constexpr Iterator operator--(int) noexcept;

  template <bool B, typename T>
  friend bool operator==(const Iterator<B, T>& first,
                         const Iterator<B, T>& second);
  template <bool B, typename T>
  friend bool operator!=(const Iterator<B, T>& first,
                         const Iterator<B, T>& second);

  BasePtr_ node_;
};

// А зачем Val_?
template <typename Val_, typename ValPtr_>
struct NodeTraits {
  using Node_ = Node<ValPtr_>;
  using NodePtr_ = PtrTraitsRebind_<ValPtr_, Node_>;
  using NodeBase_ = NodeBase<PtrTraitsRebind_<ValPtr_, void>>;
  using BasePtr_ = PtrTraitsRebind_<ValPtr_, NodeBase_>;

  using Header_ = Header<NodeBase_>;
  using Iterator_ = Iterator<false, ValPtr_>;
  using ConstIterator_ = Iterator<true, ValPtr_>;
};

//////////////
// NodeBase //
//////////////

template <typename Ptr_>
typename NodeBase<Ptr_>::BasePtr_ NodeBase<Ptr_>::Minimum(
    BasePtr_ node) noexcept {
  if (!node) {
    return nullptr;
  }

  while (node->left_) {
    node = node->left_;
  }
  return node;
}

template <typename Ptr_>
typename NodeBase<Ptr_>::BasePtr_ NodeBase<Ptr_>::Maximum(
    BasePtr_ node) noexcept {
  if (!node) {
    return nullptr;
  }

  while (node->right_) {
    node = node->right_;
  }
  return node;
}

template <typename Ptr_>
NodeBase<Ptr_>::BasePtr_ NodeBase<Ptr_>::GetBasePtr() const noexcept {
  return std::pointer_traits<BasePtr_>::pointer_to(
      *const_cast<NodeBase*>(this));
}

//////////
// Node //
//////////

template <typename ValPtr_>
Node<ValPtr_>::ValueType_* Node<ValPtr_>::GetValPtr() {
  return std::addressof(storage_.data_);
}

template <typename ValPtr_>
Node<ValPtr_>::ValueType_ const* Node<ValPtr_>::GetValPtr() const {
  return std::addressof(storage_.data_);
}

template <typename ValPtr_>
Node<ValPtr_>::NodePtr_ Node<ValPtr_>::GetNodePtr() noexcept {
  return std::pointer_traits<NodePtr_>::pointer_to(*this);
}

////////////
// Header //
////////////

template <typename NodeBase_>
Header<NodeBase_>::Header() noexcept {
  header_.color_ = NodeColor::kRed;
  Reset();
}

template <typename NodeBase_>
Header<NodeBase_>::Header(Header&& other) noexcept {
  header_.color_ = NodeColor::kRed;
  Reset();

  if (other.header_.parent_ != nullptr) {
    SwapData(other);
  }
}

template <typename NodeBase_>
void Header<NodeBase_>::SwapData(Header& other) noexcept {
  std::swap(header_.parent_, other.header_.parent_);
  std::swap(header_.left_, other.header_.left_);
  std::swap(header_.right_, other.header_.right_);
  std::swap(header_.color_, other.header_.color_);
  std::swap(node_count_, other.node_count_);

  if (header_.parent_ != nullptr) {
    header_.parent_->parent_ = header_.GetBasePtr();
  }
  if (other.header_.parent_ != nullptr) {
    other.header_.parent_->parent_ = other.header_.GetBasePtr();
  }
}

template <typename NodeBase_>
void Header<NodeBase_>::Reset() noexcept {
  header_.parent_ = nullptr;
  header_.left_ = header_.right_ = header_.GetBasePtr();
  node_count_ = 0;
}

//////////////
// Iterator //
//////////////

template <bool IsConst_, typename ValPtr_>
constexpr Iterator<IsConst_, ValPtr_>::Iterator(BasePtr_ node) noexcept
    : node_(node) {}

template <bool IsConst_, typename ValPtr_>
constexpr Iterator<IsConst_, ValPtr_>::Iterator(
    const Iterator<false, ValPtr_>& it)
  requires(IsConst_)
    : node_(it.node_) {}

template <bool IsConst_, typename ValPtr_>
[[nodiscard]]
Iterator<IsConst_, ValPtr_>::reference Iterator<IsConst_, ValPtr_>::operator*()
    const noexcept {
  return *static_cast<Node_&>(*node_).GetValPtr();
}

template <bool IsConst_, typename ValPtr_>
[[nodiscard]]
Iterator<IsConst_, ValPtr_>::pointer Iterator<IsConst_, ValPtr_>::operator->()
    const noexcept {
  return static_cast<Node_&>(*node_).GetValPtr();
}

template <bool B, typename T>
bool operator==(const Iterator<B, T>& first, const Iterator<B, T>& second) {
  return first.node_ == second.node_;
}

template <bool B, typename T>
bool operator!=(const Iterator<B, T>& first, const Iterator<B, T>& second) {
  return !(first == second);
}

template <bool IsConst_, typename ValPtr_>
constexpr Iterator<IsConst_, ValPtr_>&
Iterator<IsConst_, ValPtr_>::operator++() noexcept {
  if (node_->right_) {
    node_ = NodeBase_::Minimum(node_->right_);
  } else {
    BasePtr_ parent = node_->parent_;
    while (parent && node_ == parent->right_) {
      node_ = parent;
      parent = parent->parent_;
    }

    if (node_->right_ != parent) {
      node_ = parent;
    }
  }

  return *this;
}

template <bool IsConst_, typename ValPtr_>
constexpr Iterator<IsConst_, ValPtr_>&
Iterator<IsConst_, ValPtr_>::operator--() noexcept {
  if (node_->parent_->parent_ == node_ && node_->color_ == NodeColor::kRed) {
    node_ = node_->right_;
  } else if (node_->left_) {
    node_ = NodeBase_::Maximum(node_->left_);
  } else {
    BasePtr_ parent = node_->parent_;
    while (parent && node_ == parent->left_) {
      node_ = parent;
      parent = parent->parent_;
    }
    node_ = parent;
  }

  return *this;
}

template <bool IsConst_, typename ValPtr_>
constexpr Iterator<IsConst_, ValPtr_> Iterator<IsConst_, ValPtr_>::operator++(
    int) noexcept {
  Iterator it(this->node_);
  ++*this;
  return it;
}

template <bool IsConst_, typename ValPtr_>
constexpr Iterator<IsConst_, ValPtr_> Iterator<IsConst_, ValPtr_>::operator--(
    int) noexcept {
  Iterator it(this->node_);
  --*this;
  return it;
}

template <typename Iter_>
std::iterator_traits<Iter_>::difference_type distance(Iter_ first, Iter_ last) {
  typename std::iterator_traits<Iter_>::difference_type count = 0;
  while (first != last) {
    ++count;
    ++first;
  }
  return count;
}

}  // namespace rb_tree
}  // namespace s21

#endif  //  _S21_RB_TREE_UTILS_
