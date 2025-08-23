#ifndef _S21_RB_TREE_UTILS_
#define _S21_RB_TREE_UTILS_

#include <memory>

namespace s21 {
namespace rb_tree {

template <typename Ptr_, typename T_>
using PtrTraitsRebind_ = std::pointer_traits<Ptr_>::template rebind<T_>;

template <typename Ptr_>
using PtrTraitsElemType_ = std::pointer_traits<Ptr_>::element_type;


enum class NodeColor : std::uint8_t {
  kBlack,
  kRed
};

template <typename Ptr_>
struct NodeBase {
  using BasePtr_ = PtrTraitsRebind_<Ptr_, NodeBase>;

  NodeColor color_;
  BasePtr_ parent_;
  BasePtr_ right_;
  BasePtr_ left_;

  static BasePtr_ Minimum(BasePtr_ elm) noexcept;
  static BasePtr_ Maximum(BasePtr_ elm) noexcept;
  BasePtr_ GetBasePtr() const noexcept;
};

template <typename NodeBase_>
struct Header {
 private:
  using BasePtr_ = typename NodeBase_::BasePtr_;

 public:
  Header() noexcept;
  Header(Header&& other) noexcept;

  void MoveData(Header&& other) noexcept;
  void Reset();

  NodeBase_ header_;
  std::size_t node_count_;
};

template <typename ValPtr_>
struct Node : public NodeBase<PtrTraitsRebind_<ValPtr_, void>> {

  using ValueType_ = PtrTraitsElemType_<ValPtr_>;
  using NodePtr_ = PtrTraitsRebind_<ValPtr_, Node>;

  Node() noexcept ;
  Node(Node&& other) = delete;

  union Storage_ {
    Storage_() noexcept {};
    ~Storage_() {};
    ValueType_ data_;
  } storage_;

  ValueType_* GetValPtr();
  ValueType_ const*  GetValPtr() const;
  NodePtr_ GetNodePtr() noexcept;
};

// template <typename KeyCompare_>
// struct KeyCompare {
//   KeyCompare_ key_compare_;

//   KeyCompare()
//     noexcept(std::is_nothrow_default_constructible_v<KeyCompare_>);
//   KeyCompare(const KeyCompare& other) = default;
//   KeyCompare(KeyCompare&& other)
//     noexcept(std::is_nothrow_default_constructible_v<KeyCompare_>);
//   explicit KeyCompare(const KeyCompare_& other);
// };

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
  constexpr Iterator(const Iterator<false, ValPtr_>& it) requires IsConst_;

  [[nodiscard]] reference operator*() const noexcept;
  [[nodiscard]] pointer operator->() const noexcept;

  constexpr Iterator& operator++() noexcept;
  constexpr Iterator& operator++(int) noexcept;

  constexpr Iterator& operator--() noexcept;
  constexpr Iterator& operator--(int) noexcept;

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

  static void RotateLeft(BasePtr_ node);
  static void RotateRight(BasePtr_ node);

  static void InsertRebalance(BasePtr_ node);
  static BasePtr_ EraseRebalance(BasePtr_ node);
};

//////////
// Node //
//////////

template <typename ValPtr_>
Node<ValPtr_>::ValueType_* Node<ValPtr_>::GetValPtr() {
  return std::addresof(storage_.data_);
}


////////////
// Header //
////////////




//////////////
// Iterator //
//////////////

template <bool IsConst_, typename ValPtr_>
constexpr Iterator<IsConst_, ValPtr_>::Iterator(BasePtr_ node) noexcept
: node_(node) {
}

template <bool IsConst_, typename ValPtr_>
[[nodiscard]]
Iterator<IsConst_, ValPtr_>::reference
Iterator<IsConst_, ValPtr_>::operator*() const noexcept {
  return *static_cast<NodePtr_>(node_)->GetValPtr();
}

template <bool IsConst_, typename ValPtr_>
[[nodiscard]]
Iterator<IsConst_, ValPtr_>::pointer
Iterator<IsConst_, ValPtr_>::operator->() const noexcept {
  return static_cast<NodePtr_>(node_)->GetValPtr();
}

template <bool B, typename T>
bool operator==(const Iterator<B, T>& first,
                const Iterator<B, T>& second) {
  return first.node_ == second.node_;
}

template <bool B, typename T>
bool operator!=(const Iterator<B, T>& first,
                const Iterator<B, T>& second) {
  return !(first == second);
}


} //  rb_tree
} //  s21

#endif //  _S21_RB_TREE_UTILS_

